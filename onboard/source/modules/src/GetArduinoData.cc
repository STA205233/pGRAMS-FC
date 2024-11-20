#include "GetArduinoData.hh"
#include "boost/format.hpp"
#include <chrono>
#include <thread>
using namespace anlnext;

namespace gramsballoon::pgrams {
ANLStatus GetArduinoData::mod_define() {
  define_parameter("filename", &mod_class::filename_);
  define_parameter("num_ch", &mod_class::numCh_);
  define_parameter("timeout_sec", &mod_class::timeout_);
  define_parameter("timeout_usec", &mod_class::timeoutUsec_);
  define_parameter("sleep_for_msec", &mod_class::sleepForMilliSec_);
  define_parameter("num_trials", &mod_class::numTrials_);
  define_parameter("baudrate", &mod_class::baudrate_);
  define_parameter("mode", &mod_class::mode_);
  define_parameter("chatter", &mod_class::chatter_);
  return AS_OK;
}
ANLStatus GetArduinoData::mod_initialize() {
  adcData_.resize(numCh_);
  esc_ = std::make_shared<EncodedSerialCommunication>(filename_, baudrate_, mode_);
  esc_->initialize();
  for (int i = 0; i < numCh_; i++) {
    regs_.push_back(std::regex((boost::format("A%d_(\\d*)") % i).str()));
  }
  if (exist_module("SendTelemtry")) {
    get_module_NC("SendTelemetry", &sendTelemetry_);
  }
  return AS_OK;
}
ANLStatus GetArduinoData::mod_analyze() {
  std::this_thread::sleep_for(std::chrono::milliseconds(sleepForMilliSec_));
  adcData_.resize(numCh_, 0);
  std::vector<bool> failed_ch(numCh_, false);
  for (int j = 0; j < numTrials_; j++) {
    timeval timeout;
    timeout.tv_sec = timeout_;
    timeout.tv_usec = timeoutUsec_;
    const int rv = esc_->WaitForTimeOut(timeout);
    if (rv == -1) {
      std::cerr << "Error in GetArduinoData::mod_analyze: rv = -1" << std::endl;
      if (sendTelemetry_) {
        sendTelemetry_->getErrorManager()->setError(ErrorType::RTD_SERIAL_COMMUNICATION_ERROR);
      }
      continue;
    }
    if (rv == 0) {
      std::cout << "TimeOut" << std::endl;
      continue;
    }
    std::string dat;
    esc_->ReadDataUntilBreak(dat);
    if (chatter_ > 0) {
      std::cout << dat << std::endl;
    }
    bool success = true;
    for (int i = 0; i < numCh_; i++) {
      std::smatch m;
      std::regex_search(dat, m, regs_[i]);
      try {
        adcData_[i] = std::stoi(m[1].str());
        failed_ch[i] = false;
      }
      catch (const std::invalid_argument &e) {
        std::cerr << "Ch " << i << " data cannot be converted." << std::endl;
        std::cerr << "Data: " << dat << std::endl;
        adcData_[i] = 0;
        failed_ch[i] = true;
        success = false;
      }
      if (chatter_ > 0) {
        std::cout << "Ch" << i << ": " << adcData_[i] << std::endl;
      }
    }
    if (success) {
      break;
    }
  }
  if (sendTelemetry_) {
    for (int i = 0; i < numCh_; i++) {
      if (failed_ch[i]) {
        sendTelemetry_->getErrorManager()->setError(ConvertRTDError(i));
      }
    }
  }
  return AS_OK;
}
ANLStatus GetArduinoData::mod_finalize() {
  return AS_OK;
}
} // namespace gramsballoon::pgrams