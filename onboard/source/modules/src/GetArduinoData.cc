#include "GetArduinoData.hh"
using namespace anlnext;

namespace gramsballoon::pgrams {
ANLStatus GetArduinoData::mod_define() {
  define_parameter("filename", &mod_class::filename_);
  return AS_OK;
}
ANLStatus GetArduinoData::mod_initialize() {
  regex_ = std::regex(R"(A(\d+)\_(\d*)_)");
  return AS_OK;
}
ANLStatus GetArduinoData::mod_analyze() {
  std::ifstream ifs(filename_);
  std::string dat;
  ifs >> dat;
  std::smatch m;
  std::regex_match(dat, m, regex_);
  for (int i = 0; i < NUM_CH; i++) {
    try {
      adcData_[i] = std::stoi(m[i + 1].str());
    }
    catch (const std::invalid_argument &e) {
      adcData_[i] = 0;
    }
    std::cout << "ch" << i << ": " << adcData_[i] << std::endl;
  }
  return AS_OK;
}
ANLStatus GetArduinoData::mod_finalize() {
  return AS_OK;
}
} // namespace gramsballoon::pgrams