#include "GetMHADCData.hh"
#include "boost/format.hpp"
#include <chrono>
#include <thread>
using namespace anlnext;

namespace gramsballoon::pgrams {
ANLStatus GetMHADCData::mod_define() {
  define_parameter("num_ch", &mod_class::numCh_);
  define_parameter("MHADCManager_name", &mod_class::encodedSerialCommunicatorName_);
  return AS_OK;
}
ANLStatus GetMHADCData::mod_initialize() {
  adcData_.resize(numCh_);
  if (exist_module(encodedSerialCommunicatorName_)) {
    encodedSerialCommunicator_.reset(get_module_NC<EncodedSerialCommunicator>(encodedSerialCommunicatorName_));
  }
  else {
    std::cerr << encodedSerialCommunicatorName_ << " does not exist." << std::endl;
    encodedSerialCommunicator_.reset();
    return AS_ERROR;
  }
  return AS_OK;
}

ANLStatus GetMHADCData::mod_analyze() {
  adcData_.resize(numCh_, 0);
  std::string dat;
  if (!encodedSerialCommunicator_) {
    return AS_OK;
  }
  const int byte_read = encodedSerialCommunicator_->SendComAndGetData("a", dat, 500);
  if (byte_read < 0) {
    std::cerr << "Error in GetMHADCData::mod_analyze: byte_read = " << byte_read << std::endl;
    return AS_OK;
  }
  else if (byte_read == 0) {
    return AS_OK;
  }
  for (int i = 0; i < numCh_; i++) {
    std::regex reg = std::regex((boost::format("A%02d_(\\d*)") % i).str());
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