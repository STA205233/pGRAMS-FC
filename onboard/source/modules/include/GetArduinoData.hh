/**
 *  Module for communicating with Arduino.
 *
 * @author Shota Arai
 * @date 2024-09-19
 * 
 */

#ifndef GRAMSBalloon_GetArduinoData_hh
#define GRAMSBalloon_GetArduinoData_hh 1
#include "EncodedSerialCommunication.hh"
#include "SendTelemetry.hh"
#include "anlnext/BasicModule.hh"
#include <fstream>
#include <regex>
#include <string>
#include <vector>

namespace gramsballoon {
class SendTelemetry;
} // namespace gramsballoon
namespace gramsballoon::pgrams {
class GetArduinoData: public anlnext::BasicModule {
  DEFINE_ANL_MODULE(GetArduinoData, 1.0);
  ENABLE_PARALLEL_RUN();

public:
  GetArduinoData() = default;
  virtual ~GetArduinoData() = default;

protected:
  GetArduinoData(const GetArduinoData &r) = default;

private:
  static constexpr int BUFFER_SIZE = 500;
  int numCh_ = 32;
  SendTelemetry *sendTelemetry_ = nullptr;
  std::vector<int> adcData_;
  std::vector<std::regex> regs_;
  std::string filename_ = "/dev/ttyACM0";
  std::shared_ptr<EncodedSerialCommunication> esc_ = nullptr;
  int timeout_ = 1;
  speed_t baudrate_ = B9600;
  mode_t mode_ = O_RDWR | O_NONBLOCK;
  int numTrials_ = 10;
  int timeoutUsec_ = 0;
  int sleepForMilliSec_ = 0;
  int chatter_ = 0;

public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_finalize() override;
  const std::vector<int> &AdcData() const {
    return singleton_self()->adcData_;
  }
  int NumCH() const {
    return singleton_self()->numCh_;
  }
};
} // namespace gramsballoon::pgrams
#endif //GRAMSBalloon_GetArduinoData_hh