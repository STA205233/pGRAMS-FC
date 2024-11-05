#include "GetPressure.hh"
#include "boost/format.hpp"
using namespace anlnext;
namespace gramsballoon::pgrams {
ANLStatus GetPressure::mod_define() {
  define_parameter("channel", &mod_class::channel_);
  define_parameter("EncodedSerialCommunicator_name", &mod_class::encodedSerialCommunicatorName_);
  define_parameter("sleep_for_msec", &mod_class::sleepForMsec_);
  define_parameter("chatter", &mod_class::chatter_);
  return AS_OK;
}
ANLStatus GetPressure::mod_initialize() {
  //if (exist_module("SendTelemetry")) {
  //  get_module_NC("SendTelemetry", &sendTelemetry_);
  //}
  if (exist_module(encodedSerialCommunicatorName_)) {
    get_module_NC(encodedSerialCommunicatorName_, &encodedSerialCommunicator_);
  }
  else {
    std::cerr << encodedSerialCommunicatorName_ << " does not exist." << std::endl;
    encodedSerialCommunicator_ = nullptr;
    //if (sendTelemetry_) {
    //  sendTelemetry_->getErrorManager()->setError(ErrorType::MODULE_ACCESS_ERROR);
    //}
    return AS_ERROR;
  }
  for (int i = 1; i <= MAX_PRESSURE_NUM; i++) {
    commands_.push_back((boost::format("@%03dPR%d?;FF") % channel_ % i).str());
  }
  std::string pat((boost::format("@%03dACK([0-9\\-\\+E\\.]*?);FF") % channel_).str());
  reg_ = std::regex(pat);
  return AS_OK;
}
ANLStatus GetPressure::mod_analyze() {
  if (!encodedSerialCommunicator_) {
    return AS_OK;
  }
  std::string dat;
  pressure_.resize(MAX_PRESSURE_NUM);
  for (int i = 0; i < static_cast<int>(commands_.size()); i++) {
    std::cout << "Sent Command: " << commands_[i] << std::endl;
    const int byte_read = encodedSerialCommunicator_->SendComAndGetData(commands_[i], dat, sleepForMsec_);
    if (byte_read < 0) {
      std::cerr << "Error in GetPressure::mod_analyze: byte_read = " << byte_read << std::endl;
      continue;
    }
    else if (byte_read == 0) {
      continue;
    }
    std::smatch m;
    const bool result = std::regex_search(dat, m, reg_);
    if (!result) {
      std::cout << "Pressure data was not read." << std::endl;
    }
    if (chatter_ > 0) {
      std::cout << "read: " << m[1].str() << std::endl;
    }
    try {
      pressure_[i] = std::stof(m[1].str());
    }
    catch (const std::invalid_argument &e) {
      std::cout << "Pressure data was not read." << std::endl;
      pressure_[i] = 0;
    }
    if (chatter_ > 0) {
      std::cout << "Pressure: " << pressure_[i] << std::endl;
    }
  }
  return AS_OK;
}
} // namespace gramsballoon::pgrams