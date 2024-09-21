#include "GetPressure.hh"
#include "boost/format.hpp"
using namespace anlnext;
namespace gramsballoon::pgrams {
ANLStatus GetPressure::mod_define() {
  define_parameter("channel", &mod_class::channel_);
  define_parameter("EncodedSerialCommunicator_name", &mod_class::encodedSerialCommunicatorName_);
  return AS_OK;
}
ANLStatus GetPressure::mod_initialize() {
  if (exist_module(encodedSerialCommunicatorName_)) {
    get_module_NC(encodedSerialCommunicatorName_, &encodedSerialCommunicator_);
  }
  else {
    std::cerr << encodedSerialCommunicatorName_ << " does not exist." << std::endl;
    encodedSerialCommunicator_ = nullptr;
    return AS_ERROR;
  }
  command_ = (boost::format("@%03dPR3?;FF") % channel_).str();
  std::string pat((boost::format("@%03dACK([0-9\\-\\+E\\.]*?);FF") % channel_).str());
  reg_ = std::regex(pat);
  return AS_OK;
}
ANLStatus GetPressure::mod_analyze() {
  if (!encodedSerialCommunicator_) {
    return AS_OK;
  }
  std::string dat;
  const int byte_read = encodedSerialCommunicator_->SendComAndGetData(command_, dat, 500);
  if (byte_read < 0) {
    std::cerr << "Error in GetPressure::mod_analyze: byte_read = " << byte_read << std::endl;
    return AS_OK;
  }
  else if (byte_read == 0) {
    return AS_OK;
  }
  std::smatch m;
  const bool result = std::regex_search(dat, m, reg_);
  if (!result) {
    std::cout << "Pressure data was not read." << std::endl;
    return AS_OK;
  }
  std::cout << "read: " << m[1].str() << std::endl;
  try {
    pressure_ = std::stof(m[1].str());
  }
  catch (const std::invalid_argument &e) {
    std::cout << "Pressure data was not read." << std::endl;
    pressure_ = 0;
  }
  std::cout << "Pressure: " << pressure_ << std::endl;
  return AS_OK;
}
} // namespace gramsballoon::pgrams