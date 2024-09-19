#include "GetArduinoData.hh"
#include "boost/format.hpp"
using namespace anlnext;

namespace gramsballoon::pgrams {
ANLStatus GetArduinoData::mod_define() {
  define_parameter("filename", &mod_class::filename_);
  define_parameter("num_ch", &mod_class::numCh_);
  define_parameter("timeout_sec", &mod_class::timeout_);
  return AS_OK;
}
ANLStatus GetArduinoData::mod_initialize() {
  adcData_.resize(numCh_);
  esc_ = std::make_shared<EncodedSerialCommunication>(filename_, B9600, O_RDWR | O_NONBLOCK);
  esc_->initialize();
  return AS_OK;
}
ANLStatus GetArduinoData::mod_analyze() {
  timeval timeout;
  timeout.tv_sec = timeout_;
  timeout.tv_usec = 0;
  const int rv = esc_->WaitForTimeOut(timeout);
  if (rv == -1) {
    std::cerr << "Error in GetArduinoData::mod_analyze: rv = -1" << std::endl;
    return AS_ERROR;
  }
  if (rv == 0) {
    std::cout << "TimeOut" << std::endl;
    return AS_OK;
  }
  std::string dat;
  esc_->ReadData(dat, BUFFER_SIZE);
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
ANLStatus GetArduinoData::mod_finalize() {
  return AS_OK;
}
} // namespace gramsballoon::pgrams