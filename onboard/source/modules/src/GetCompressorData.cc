#include "GetCompressorData.hh"
#include <regex>
using namespace anlnext;
namespace gramsballoon::pgrams {
ANLStatus GetCompressorData::mod_define() {
  return AS_OK;
}
ANLStatus GetCompressorData::mod_initialize() {
  regTemp_ = std::regex(",(\\d*)");
  return AS_OK;
}

anlnext::ANLStatus GetCompressorData::mod_finalize() {
  return AS_OK;
}

ANLStatus GetCompressorData::mod_analyze() {
  const std::string command_temp = "$TEAA4B9\r";
  const std::string command_press = "$PRA95F7\r";
  std::string data_temp, data_press;
  communicator_->SendComAndGetData(command_temp, data_temp);
  communicator_->SendComAndGetData(command_press, data_press);
  std::cout << "data_temp: " << data_temp << std::endl;
  std::cout << "data_press: " << data_press << std::endl;
  std::smatch m;
  std::regex_search(data_temp, m, regTemp_);
  for (const auto &i: m) {
    std::cout << "i: " << i << std::endl;
  }
  return AS_OK;
}

} // namespace gramsballoon::pgrams