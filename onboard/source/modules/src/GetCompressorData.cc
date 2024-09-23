#include "GetCompressorData.hh"
#include <regex>
using namespace anlnext;
namespace gramsballoon::pgrams {
ANLStatus GetCompressorData::mod_define() {
  define_parameter("EncodedSerialCommunicator_name", &mod_class::encodedSerialCommunicatorName_);
  return AS_OK;
}
ANLStatus GetCompressorData::mod_initialize() {
  if (exist_module(encodedSerialCommunicatorName_)) {
    get_module_NC(encodedSerialCommunicatorName_, &communicator_);
  }
  else {
    std::cerr << encodedSerialCommunicatorName_ << " does not exist." << std::endl;
    communicator_ = nullptr;
    return AS_ERROR;
  }
  regTemp_ = std::regex("\\$.*?,(\\d*),(\\d*),(\\d*),(\\d*),.*?");
  regPress_ = std::regex("\\$.*?,(\\d*),(\\d*),.*?");
  return AS_OK;
}

anlnext::ANLStatus GetCompressorData::mod_finalize() {
  return AS_OK;
}

ANLStatus GetCompressorData::mod_analyze() {
  if (!communicator_) {
    return AS_OK;
  }
  const std::string command_temp = "$TEAA4B9\r";
  const std::string command_press = "$PRA95F7\r";
  std::string data_temp, data_press;
  const int res_temp = communicator_->SendComAndGetData(command_temp, data_temp);
  const int res_press = communicator_->SendComAndGetData(command_press, data_press);
  if (res_press <= 0 && res_temp <= 0) {
    return AS_ERROR;
  }
  std::cout << "data_temp: " << data_temp << std::endl;
  std::cout << "data_press: " << data_press << std::endl;
  std::smatch m_temp;
  std::regex_search(data_temp, m_temp, regTemp_);
  const int sz = static_cast<int>(m_temp.size());
  if (sz != NUM_TEMPERATURE + 1) {
    std::cerr << "Data size is incorrect" << std::endl;
    return AS_ERROR;
  }
  for (int i = 0; i < sz - 1; i++) {
    try {
      temperature_[i] = std::stoi(m_temp[i + 1].str());
    }
    catch (const std::invalid_argument &e) {
      std::cout << "TP " << i + 1 << " data was not read." << std::endl;
      temperature_[i] = 0;
    }
    std::cout << "TP" << i + 1 << ": " << temperature_[i] << std::endl;
  }
  std::smatch m_press;
  std::regex_search(data_press, m_press, regPress_);
  const int sz2 = static_cast<int>(m_press.size());
  if (sz2 != NUM_PRESSURE + 1) {
    std::cerr << "Data size is incorrect" << std::endl;
    return AS_ERROR;
  }
  for (int i = 0; i < sz2 - 1; i++) {
    try {
      pressure_[i] = std::stoi(m_press[i + 1].str());
    }
    catch (const std::invalid_argument &e) {
      std::cout << "PR " << i + 1 << " data was not read." << std::endl;
      pressure_[i] = 0;
    }
    std::cout << "PR" << i + 1 << ": " << pressure_[i] << std::endl;
  }
  return AS_OK;
}

} // namespace gramsballoon::pgrams