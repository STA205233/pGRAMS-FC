#include "DAQIO.hh"
#include "ReadWaveform.hh"
#include <chrono>
#include <sstream>
#include <thread>

using namespace anlnext;
namespace gramsballoon {
ANLStatus ReadWaveform::mod_define() {
  GBBasicDemoModule::mod_define();
  define_parameter("ad_manager_name", &mod_class::ADManagerName_);
  define_parameter("trig_device", &mod_class::trigDevice_);
  define_parameter("trig_channel", &mod_class::trigChannel_);
  define_parameter("trig_mode", &mod_class::trigMode_);
  define_parameter("trig_level", &mod_class::trigLevel_);
  define_parameter("trig_position", &mod_class::trigPosition_);
  define_parameter("time_window", &mod_class::timeWindow_);
  define_parameter("sample_frequency", &mod_class::sampleFreq_);
  define_parameter("adc_range_list", &mod_class::adcRangeList_);
  define_parameter("adc_offset_list", &mod_class::adcOffsetList_);
  define_parameter("output_filename_base", &mod_class::outputFilenameBase_);
  define_parameter("num_events_per_file", &mod_class::numEventsPerFile_);
  define_parameter("start_reading", &mod_class::startReading_);
  define_parameter("max_non_detection_count", &mod_class::maxNonDetectionCount_);
  define_parameter("non_detection_rate", &mod_class::nonDetectionRate_);
  define_parameter("chatter", &mod_class::chatter_);
  return AS_OK;
}
ANLStatus ReadWaveform::mod_initialize() {
  GBBasicDemoModule::mod_initialize();
  if (exist_module("SendTelemetry")) {
    get_module_NC("SendTelemetry", &sendTelemetry_);
  }
  const std::string run_id_manager_md = "RunIDManager";
  if (exist_module(run_id_manager_md)) {
    get_module_NC(run_id_manager_md, &runIDManager_);
  }

  return AS_OK;
}
ANLStatus ReadWaveform::mod_analyze() {
  if (triggerChanged_) {
    const std::string msg = "Trigger parameters set: trigDevice = " + std::to_string(trigDevice_) + ", trigChannel = " + std::to_string(trigChannel_) + ", trigMode = " + std::to_string(trigMode_) + ", trigLevel = " + std::to_string(trigLevel_) + ", trigPosition = " + std::to_string(trigPosition_);
    PrintInfo(msg);
    triggerChanged_ = false;
  }
  if (analogInSettingChanged_) {
    std::stringstream msg;
    msg << "ADC offset has been changed: ";
    for (size_t i = 0; i < adcOffsetList_.size(); i++) {
      msg << adcOffsetList_[i] << " ";
    }
    PrintInfo(msg.str());
    analogInSettingChanged_ = false;
  }
  if (!startReading_) {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    return AS_OK;
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  eventHeader_.resize(5);
  if (eventID_ % numEventsPerFile_ == 0) {
    if (eventID_ != 0) {
      closeOutputFile();
    }
    createNewOutputFile();
    if (!(*ofs_)) {
      return AS_QUIT_ALL_ERROR;
    }
  }
  DAQResult res = DAQResult::TRIGGERED;
  if (SampleFromUniformDistribution() + 0.5 < nonDetectionRate_) {
    res = DAQResult::NON_DETECTION;
  }
  else {
    res = DAQResult::TRIGGERED;
  }
  GenerateFakeEvent();

  if (recentEventHeader_.size() == 0) {
    recentEventHeader_ = eventHeader_;
  }
  if (recentEventData_.size() == 0) {
    recentEventData_ = eventData_;
  }
  if (res == DAQResult::NON_DETECTION) {
    GenerateEventHeader();
    nonDetectionCounter_++;
  }
  if (res == DAQResult::TRIGGERED) {
    nonDetectionCounter_ = 0;
  }

  if (nonDetectionCounter_ >= maxNonDetectionCount_) {
    std::cout << "Failed to detect events " << nonDetectionCounter_ << "times in a row." << std::endl;
    std::cout << "Probably trigger level is too high." << std::endl;
    //if (sendTelemetry_) {
    //sendTelemetry_->getErrorManager()->setError(ErrorType::TOO_FEW_EVENTS_DETECTED);
    //}
    nonDetectionCounter_ = 0;
  }
  writeData();

  if (res == DAQResult::TRIGGERED) {
    std::swap(eventHeader_, recentEventHeader_);
    std::swap(eventData_, recentEventData_);
    recentEventID_ = eventID_;
  }
  if (ondemand_) {
    sendTelemetry_->setEventID(recentEventID_);
    sendTelemetry_->setEventHeader(recentEventHeader_);
    sendTelemetry_->setEventData(recentEventData_);
    sendTelemetry_->setTelemetryType(2);
    ondemand_ = false;
  }
  eventID_++;
  return AS_OK;
}
ANLStatus ReadWaveform::mod_finalize() {
  closeOutputFile();
  return AS_OK;
}
void ReadWaveform::createNewOutputFile() {
  int run_id = 0;
  std::string time_stamp_str = "YYYYMMDDHHMMSS";
  if (runIDManager_) {
    run_id = runIDManager_->RunID();
    time_stamp_str = runIDManager_->TimeStampStr();
  }
  std::ostringstream id_sout;
  id_sout << std::setfill('0') << std::right << std::setw(6) << fileID_;
  const std::string id_str = id_sout.str();
  std::ostringstream run_id_sout;
  run_id_sout << std::setfill('0') << std::right << std::setw(6) << run_id;
  const std::string run_id_str = run_id_sout.str();
  const std::string filename = outputFilenameBase_ + "_" + run_id_str + "_" + time_stamp_str + "_" + id_str + ".dat";
  ofs_ = std::make_shared<std::ofstream>(filename, std::ios::out | std::ios::binary);
  if (!(*ofs_)) {
    std::cerr << "File open error." << std::endl;
    return;
  }
  fileID_++;

  std::vector<int16_t> file_header;
  GenerateFileHeader(file_header, numEventsPerFile_);
  std::vector<char> vec;
  const int n = file_header.size();
  for (int i = 0; i < n; i++) {
    const int byte = sizeof(int16_t);
    for (int j = 0; j < byte; j++) {
      const int shift = 8 * (byte - 1 - j);
      char c = static_cast<char>((file_header[i] >> shift) & 0xff);
      vec.push_back(c);
    }
  }
  ofs_->write(&vec[0], static_cast<int>(vec.size()));
  ofs_->flush();
}
void ReadWaveform::closeOutputFile() {
  if (!ofs_) {
    return;
  }
  std::vector<int16_t> file_footer;
  GenerateFileFooter(file_footer);
  std::vector<char> vec;
  const int n = file_footer.size();
  for (int i = 0; i < n; i++) {
    const int byte = sizeof(int16_t);
    for (int j = 0; j < byte; j++) {
      const int shift = 8 * (byte - 1 - j);
      char c = static_cast<char>((file_footer[i] >> shift) & 0xff);
      vec.push_back(c);
    }
  }

  ofs_->write(&vec[0], static_cast<int>(vec.size()));
  ofs_->flush();
  ofs_->close();
}
void ReadWaveform::writeData() {
  if (!ofs_) {
    return;
  }
  std::vector<char> vec;

  const int n1 = eventHeader_.size();
  for (int i = 0; i < n1; i++) {
    const int byte = sizeof(int16_t);
    for (int j = 0; j < byte; j++) {
      const int shift = 8 * (byte - 1 - j);
      char c = static_cast<char>((eventHeader_[i] >> shift) & 0xff);
      vec.push_back(c);
    }
  }

  const int n2 = eventData_.size();
  for (int i = 0; i < n2; i++) {
    const int byte = sizeof(int16_t);
    for (int16_t ph: eventData_[i]) {
      for (int j = 0; j < byte; j++) {
        const int shift = 8 * (byte - 1 - j);
        char c = static_cast<char>((ph >> shift) & 0xff);
        vec.push_back(c);
      }
    }
  }

  ofs_->write(&vec[0], static_cast<int>(vec.size()));
  ofs_->flush();
}
void ReadWaveform::GenerateFileHeader(std::vector<int16_t> &header, int16_t num_event) {
  const int sz_header = 32;
  header.resize(sz_header);
  timeval time_now;
  gettimeofday(&time_now, NULL);
  int numSample = 8000;
  header[0] = static_cast<int16_t>(trigDevice_);
  header[1] = static_cast<int16_t>(trigChannel_);
  header[2] = static_cast<int16_t>(trigSrc_);
  header[3] = static_cast<int16_t>(trigSlope_);
  header[4] = static_cast<int16_t>(trigLevel_ * 1E3);
  header[5] = static_cast<int16_t>(trigPosition_ * 1E3);
  header[6] = static_cast<int16_t>(sampleFreq_ * 1E2);
  header[7] = static_cast<int16_t>(timeWindow_ * 1E3);
  header[8] = static_cast<int16_t>(numSample);
  header[9] = static_cast<int16_t>(num_event);

  header[10] = static_cast<int16_t>((time_now.tv_sec >> 16) & (0xffff));
  header[11] = static_cast<int16_t>((time_now.tv_sec) & (0xffff));
  header[12] = static_cast<int16_t>((time_now.tv_usec >> 16) & (0xffff));
  header[13] = static_cast<int16_t>((time_now.tv_usec) & (0xffff));

  int index = 14;
  for (int i = 0; i < 4; i++) {
    const double scale = 1.0E3;
    header[index] = static_cast<int16_t>(((static_cast<int>(range_[i] * scale)) >> 16) & (0xffff));
    header[index + 1] = static_cast<int16_t>((static_cast<int>(range_[i] * scale)) & (0xffff));
    index += 2;
  }

  for (int i = 0; i < 4; i++) {
    const double scale = 1.0E3;
    header[index] = static_cast<int16_t>(((static_cast<int>(offset_[i] * scale)) >> 16) & (0xffff));
    header[index + 1] = static_cast<int16_t>((static_cast<int>(offset_[i] * scale)) & (0xffff));
    index += 2;
  }
}
void ReadWaveform::GenerateFileFooter(std::vector<int16_t> &footer) {
  const int sz_footer = 16;
  footer.resize(sz_footer);
  timeval time_now;
  gettimeofday(&time_now, NULL);
  footer[0] = static_cast<int16_t>((time_now.tv_sec >> 16) & (0xffff));
  footer[1] = static_cast<int16_t>((time_now.tv_sec) & (0xffff));
  footer[2] = static_cast<int16_t>((time_now.tv_usec >> 16) & (0xffff));
  footer[3] = static_cast<int16_t>((time_now.tv_usec) & (0xffff));
}

void ReadWaveform::GenerateEventHeader() {
  timeval event_time;
  gettimeofday(&event_time, NULL);
  eventHeader_[0] = static_cast<int16_t>(eventID_);
  eventHeader_[1] = static_cast<int16_t>((event_time.tv_sec >> 16) & (0xffff));
  eventHeader_[2] = static_cast<int16_t>((event_time.tv_sec) & (0xffff));
  eventHeader_[3] = static_cast<int16_t>((event_time.tv_usec >> 16) & (0xffff));
  eventHeader_[4] = static_cast<int16_t>((event_time.tv_usec) & (0xffff));
}
void ReadWaveform::GenerateFakeEvent() {
  eventData_.clear();
  const int num_sample = static_cast<int>(sampleFreq_ * timeWindow_);
  for (int i = 0; i < 4; i++) {
    std::vector<int16_t> ph;
    for (int j = 0; j < num_sample; j++) {
      ph.push_back(static_cast<int16_t>(j));
    }
    eventData_.push_back(ph);
  }
}
} // namespace gramsballoon