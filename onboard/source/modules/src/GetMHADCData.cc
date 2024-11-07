#include "GetMHADCData.hh"
#include "boost/format.hpp"
#include <chrono>
#include <thread>
using namespace anlnext;

namespace gramsballoon::pgrams {
ANLStatus GetMHADCData::mod_define() {
  define_parameter("num_ch", &mod_class::numCh_);
  define_parameter("MHADCManager_name", &mod_class::encodedSerialCommunicatorName_);
  define_parameter("sleep_for_msec", &mod_class::sleepForMsec_);
  define_parameter("chatter", &mod_class::chatter_);
  return AS_OK;
}
ANLStatus GetMHADCData::mod_initialize() {
  adcData_.resize(numCh_);
  for (int i = 0; i < numCh_; i++) {
    regs_.push_back(std::regex((boost::format("A%02d_(\\d*)") % i).str()));
  }
  if (exist_module(encodedSerialCommunicatorName_)) {
    get_module_NC(encodedSerialCommunicatorName_, &encodedSerialCommunicator_);
  }
  else {
    std::cerr << encodedSerialCommunicatorName_ << " does not exist." << std::endl;
    encodedSerialCommunicator_ = nullptr;
    return AS_ERROR;
  }
  return AS_OK;
}

ANLStatus GetMHADCData::mod_analyze() {
  if (chatter_ > 0) {
    std::cout << "GetMHADCData::mod_analyze" << std::endl;
  }
  adcData_.resize(numCh_, 0);
  std::string dat;
  if (!encodedSerialCommunicator_) {
    return AS_OK;
  }
  const int byte_read = encodedSerialCommunicator_->SendComAndGetData("a", dat, sleepForMsec_);
  if (byte_read < 0) {
    std::cerr << "Error in GetMHADCData::mod_analyze: byte_read = " << byte_read << std::endl;
    return AS_OK;
  }
  else if (byte_read == 0) {
    return AS_OK;
  }
  for (int i = 0; i < numCh_; i++) {
    std::smatch m;
    std::regex_search(dat, m, regs_[i]);
    try {
      adcData_[i] = std::stoi(m[1].str());
    }
    catch (const std::invalid_argument &e) {
      std::cerr << "Ch " << i << " data cannot be converted." << std::endl;
      std::cerr << "Data: " << dat << std::endl;
      adcData_[i] = 0;
    }
    if (chatter_ > 0) {
      std::cout << "Ch" << i << ": " << adcData_[i] << std::endl;
    }
  }
  return AS_OK;
}
ANLStatus GetMHADCData::mod_finalize() {
  return AS_OK;
}
} // namespace gramsballoon::pgrams