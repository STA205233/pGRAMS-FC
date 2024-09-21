#include "PressureGaugeManager.hh"
#include <chrono>
#include <thread>
namespace gramsballoon::pgrams {
int PressureGaugeManager::SendComAndGetData(const std::string &command, std::string &data, int sleepfor_msec) {
  std::shared_ptr<EncodedSerialCommunication> esc = GetEncodedSerialCommunication();
  esc->WriteData(command);
  std::this_thread::sleep_for(std::chrono::milliseconds(sleepfor_msec));
  timeval timeout;
  timeout.tv_sec = GetTimeout();
  timeout.tv_usec = 0;
  const int rv = esc->WaitForTimeOut(timeout);
  if (rv == -1) {
    return -1;
  }
  if (rv == 0) {
    std::cout << "TimeOut" << std::endl;
    return 0;
  }
  const int ret = esc->ReadDataUntilSpecificStr(data, "FF");
  std::cout << "data: " << data << std::endl;
  return ret;
}
} // namespace gramsballoon::pgrams