#include "EncodedSerialCommunicator.hh"
#include <chrono>
#include <thread>
using namespace anlnext;
namespace gramsballoon::pgrams {

ANLStatus EncodedSerialCommunicator::mod_define() {
  define_parameter("filename", &mod_class::filename_);
  define_parameter("timeout_sec", &mod_class::timeout_);
  define_parameter("timeout_usec", &mod_class::timeoutUsec_);
  define_parameter("baudrate", &mod_class::baudrate_);
  define_parameter("mode", &mod_class::mode_);
  return AS_OK;
}
ANLStatus EncodedSerialCommunicator::mod_initialize() {
  if (timeout_ < 0 || timeoutUsec_ < 0) {
    std::cerr << "Error in EncodedSerialCommunicator::mod_initialize: timeout_ < 0" << std::endl;
    return AS_ERROR;
  }
  esc_ = std::make_shared<EncodedSerialCommunication>(filename_, baudrate_, mode_);
  esc_->initialize();
  return AS_OK;
}
ANLStatus EncodedSerialCommunicator::mod_finalize() {
  esc_.reset();
  return AS_OK;
}
int EncodedSerialCommunicator::SendComAndGetData(const std::string &command, std::string &data, int sleepfor) {
  esc_->WriteData(command);
  std::this_thread::sleep_for(std::chrono::milliseconds(sleepfor));
  timeval timeout;
  timeout.tv_sec = timeout_;
  timeout.tv_usec = timeoutUsec_;
  const int rv = esc_->WaitForTimeOut(timeout);
  if (rv == -1) {
    return -1;
  }
  if (rv == 0) {
    std::cout << "TimeOut" << std::endl;
    return 0;
  }
  const int ret = esc_->ReadDataUntilSpecificStr(data, "\r");
  std::cout << "data: " << data << std::endl;
  return ret;
}
int EncodedSerialCommunicator::SendCommand(const std::string &command) {
  const int ret = esc_->WriteData(command);
  return ret;
}
} // namespace gramsballoon::pgrams