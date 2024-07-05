#ifndef GB_DEMO_MODE
#include <sstream>
#include <chrono>
#include <thread>
#include "ReadWaveform.hh"

using namespace anlnext;

namespace gramsballoon {

ReadWaveform::ReadWaveform()
{
  ADManagerName_ = "AnalogDiscoveryManager";
  daqio_ = std::make_shared<DAQIO>();
  adcRangeList_ = std::vector<double>(4, 1.0);
  adcOffsetList_ = std::vector<double>(4, 0.0);
  ofs_ = std::make_shared<std::ofstream>();
  outputFilenameBase_ = "output";
}

ReadWaveform::~ReadWaveform() = default;

ANLStatus ReadWaveform::mod_define()
{
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
  define_parameter("chatter", &mod_class::chatter_);

  return AS_OK;
}

ANLStatus ReadWaveform::mod_initialize()
{
  const std::string send_telemetry_md = "SendTelemetry";
  if (exist_module(send_telemetry_md)) {
    get_module_NC(send_telemetry_md, &sendTelemetry_);
  }

  if (exist_module(ADManagerName_)) {
    get_module_NC(ADManagerName_, &ADManager_);
  }
  else {
    std::cerr << "Error in ReadWaveform::mod_initialize." << std::endl;
    std::cerr << "Analog Discovery manager does not exist. Module name = " << ADManagerName_ << std::endl;
    if (sendTelemetry_) {
      sendTelemetry_->getErrorManager()->setError(ErrorType::MODULE_ACCESS_ERROR);
    }
  }

  const std::string run_id_manager_md = "RunIDManager";
  if (exist_module(run_id_manager_md)) {
    get_module_NC(run_id_manager_md, &runIDManager_);
  }
  
  setupAnalogIn();

  AnalogDiscoveryIO* adio = ADManager_->ADIO();
  daqio_->setAnalogDiscoveryIO(adio);
  daqio_->initialize();
  daqio_->setTriggerParameters(trigDevice_, trigChannel_, trigMode_, trigLevel_, trigPosition_);
  daqio_->setSampleParameters(sampleFreq_, timeWindow_);
  const int status = daqio_->setupTrigger();
  if (status!=0) {
    std::cerr << "Trigger setup failed in ReadWaveform::mod_initialize" << std::endl;
    if (sendTelemetry_) {
      sendTelemetry_->getErrorManager()->setError(ErrorType::TRIGGER_SETUP_ERROR);
    }
  }
  
  return AS_OK;
}

ANLStatus ReadWaveform::mod_analyze()
{
  AnalogDiscoveryIO* adio = ADManager_->ADIO();
  const int num_devices = adio->NumDevices();
  if (num_devices<=0) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    return AS_OK;
  }

  if (triggerChanged_) {
    daqio_->setTriggerParameters(trigDevice_, trigChannel_, trigMode_, trigLevel_, trigPosition_);
    const int status = daqio_->setupTrigger();
    if (status!=0) {
      std::cerr << "Trigger setup failed in ReadWaveform::mod_analyze" << std::endl;
      if (sendTelemetry_) {
        sendTelemetry_->getErrorManager()->setError(ErrorType::TRIGGER_SETUP_ERROR);
      }
    }
    triggerChanged_ = false;
  }

  if (analogInSettingChanged_) {
    setupAnalogIn();
    analogInSettingChanged_ = false;
  }

  if (!startReading_) {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    return AS_OK;
  }

  if (eventID_%numEventsPerFile_ == 0) {
    if (eventID_ != 0) {
      closeOutputFile();
    }
    createNewOutputFile();
  }

  DAQResult res = daqio_->getData(eventID_, eventHeader_, eventData_);
  if (recentEventHeader_.size()==0) {
    recentEventHeader_ = eventHeader_;
  }
  if (recentEventData_.size()==0) {
    recentEventData_ = eventData_;
  }
  if (res==DAQResult::NON_DETECTION) {
    nonDetectionCounter_++;
  }
  if (res==DAQResult::TRIGGERED) {
    nonDetectionCounter_ = 0;
  }

  if (nonDetectionCounter_>=maxNonDetectionCount_) {
    std::cout << "Failed to detect events " << nonDetectionCounter_ << "times in a row." << std::endl;
    std::cout << "Probably trigger level is too high." << std::endl;
    //if (sendTelemetry_) {
    //sendTelemetry_->getErrorManager()->setError(ErrorType::TOO_FEW_EVENTS_DETECTED);
    //}
    nonDetectionCounter_ = 0;
  }

  writeData();

  if (res==DAQResult::TRIGGERED) {
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

ANLStatus ReadWaveform::mod_finalize()
{
  closeOutputFile();
  return AS_OK;
}

void ReadWaveform::setupAnalogIn()
{
  AnalogDiscoveryIO* adio = ADManager_->ADIO();
  const int num_devices = adio->NumDevices();
  const int num_sample = static_cast<int>(sampleFreq_*timeWindow_);

  int k = 0;
  for (int i=0; i<num_devices; i++) {
    for (int j=0; j<2; j++) {
      adio -> setupAnalogIn(i, j, sampleFreq_*1.0E6, num_sample, adcRangeList_[k], adcOffsetList_[k]);
      k++;
    }
  }
}

void ReadWaveform::createNewOutputFile()
{
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
  ofs_ = std::make_shared<std::ofstream>(filename, std::ios::out|std::ios::binary);
  if (!(*ofs_)) {
    std::cerr << "File open error." << std::endl;
    return;
  }
  fileID_++;

  std::vector<int16_t> file_header;
  daqio_->generateFileHeader(file_header, numEventsPerFile_);
  std::vector<char> vec;
  const int n = file_header.size();
  for (int i=0; i<n; i++) {
    const int byte = sizeof(int16_t);
    for (int j=0; j<byte; j++) {
      const int shift = 8 * (byte-1-j);
      char c = static_cast<char>((file_header[i]>>shift) & 0xff);
      vec.push_back(c);
    }
  }
  ofs_->write(&vec[0], static_cast<int>(vec.size()));
  ofs_->flush();
}

void ReadWaveform::closeOutputFile()
{
  std::vector<int16_t> file_footer;
  daqio_->generateFileFooter(file_footer);
  std::vector<char> vec;
  const int n = file_footer.size();
  for (int i=0; i<n; i++) {
    const int byte = sizeof(int16_t);
    for (int j=0; j<byte; j++) {
      const int shift = 8 * (byte-1-j);
      char c = static_cast<char>((file_footer[i]>>shift) & 0xff);
      vec.push_back(c);
    }
  }

  ofs_->write(&vec[0], static_cast<int>(vec.size()));
  ofs_->flush();
  ofs_->close();
}

void ReadWaveform::writeData()
{
  std::vector<char> vec;
  
  const int n1 = eventHeader_.size();
  for (int i=0; i<n1; i++) {
    const int byte = sizeof(int16_t);
    for (int j=0; j<byte; j++) {
      const int shift = 8 * (byte-1-j);
      char c = static_cast<char>((eventHeader_[i]>>shift) & 0xff);
      vec.push_back(c);
    }
  }

  const int n2 = eventData_.size();
  for (int i=0; i<n2; i++) {
    const int byte = sizeof(int16_t);
    for (int16_t ph: eventData_[i]) {
      for (int j=0; j<byte; j++) {
        const int shift = 8 * (byte-1-j);
        char c = static_cast<char>((ph>>shift) & 0xff);
        vec.push_back(c);
      }
    }
  }
  
  ofs_->write(&vec[0], static_cast<int>(vec.size()));
  ofs_->flush();
}

} /* namespace gramsballoon */
#endif /* GB_DEMO_MODE */
