#include "GetMHADCData.hh"
#include "boost/format.hpp"
#include <chrono>
#include <thread>
using namespace anlnext;

namespace gramsballoon::pgrams {
ANLStatus GetMHADCData::mod_define() {
  define_parameter("numCh", &mod_class::numCh_);
  define_parameter("filename", &mod_class::filename_);
  define_parameter("timeout", &mod_class::timeout_);
  define_parameter("baudrate", &mod_class::baudrate_);
  define_parameter("mode", &mod_class::mode_);
  return AS_OK;
}
ANLStatus GetMHADCData::mod_initialize() {
  adcData_.resize(numCh_);
  esc_ = std::make_shared<EncodedSerialCommunication>(filename_, baudrate_, mode_);
  esc_->initialize();
  return AS_OK;
}

ANLStatus GetMHADCData::mod_analyze() {
  std::this_thread::sleep_for(std::chrono::seconds(1));
  adcData_.resize(numCh_, 0);
  timeval timeout;
  timeout.tv_sec = timeout_;
  timeout.tv_usec = 0;
  const int rv = esc_->WaitForTimeOut(timeout);
  if (rv == -1) {
    std::cerr << "Error in GetMHADCData::mod_analyze: rv = -1" << std::endl;
    return AS_ERROR;
  }
  if (rv == 0) {
    std::cout << "TimeOut" << std::endl;
    return AS_OK;
  }
  esc_->WriteData("a");
  std::string dat;
  esc_->ReadDataUntilBreak(dat);
  for (int i = 0; i < numCh_; i++) {
    std::regex reg = std::regex((boost::format("A%i_(\\d*)") % i).str());
    std::smatch m;
    std::regex_search(dat, m, reg);
    try {
      adcData_[i] = std::stoi(m[1].str());
    }
    catch (const std::invalid_argument &e) {
      std::cout << "Ch " << i << " data was not read." << std::endl;
      adcData_[i] = 0;
    }
    std::cout << "Ch" << i << ": " << adcData_[i] << std::endl;
  }
  return AS_OK;
}
ANLStatus GetMHADCData::mod_finalize() {
  return AS_OK;
}
} // namespace gramsballoon::pgrams