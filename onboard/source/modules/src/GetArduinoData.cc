#include "GetArduinoData.hh"
#include "boost/format.hpp"
using namespace anlnext;

namespace gramsballoon::pgrams {
ANLStatus GetArduinoData::mod_define() {
  define_parameter("filename", &mod_class::filename_);
  return AS_OK;
}
ANLStatus GetArduinoData::mod_initialize() {
  ifs_ = std::ifstream(filename_);
  return AS_OK;
}
ANLStatus GetArduinoData::mod_analyze() {
  std::string dat;
  ifs_ >> dat;
  for (int i = 0; i < NUM_CH; i++) {
    regex_ = std::regex((boost::format("A%i_(\\d*)") % i).str());
    std::smatch m;
    std::regex_search(dat, m, regex_);
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
ANLStatus GetArduinoData::mod_finalize() {
  return AS_OK;
}
} // namespace gramsballoon::pgrams