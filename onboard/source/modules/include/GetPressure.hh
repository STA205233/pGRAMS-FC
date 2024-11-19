#ifndef GRAMSBalloon_GetPressure_hh
#define GRAMSBalloon_GetPressure_hh 1
#include "EncodedSerialCommunicator.hh"
#include "SendTelemetry.hh"
#include "anlnext/BasicModule.hh"
#include <regex>
#include <vector>

namespace gramsballoon {
class SendTelemetry;
} // namespace gramsballoon
namespace gramsballoon::pgrams {
class GetPressure: public anlnext::BasicModule {
  DEFINE_ANL_MODULE(GetPressure, 1.0);
  ENABLE_PARALLEL_RUN();

public:
  GetPressure() = default;
  virtual ~GetPressure() = default;
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
  const std::vector<float> &Pressure() const { return singleton_self()->pressure_; }

private:
  static constexpr int MAX_PRESSURE_NUM = 3;
  EncodedSerialCommunicator *encodedSerialCommunicator_ = nullptr;
  std::string encodedSerialCommunicatorName_ = "EncodedSerialCommunicator";
  std::vector<float> pressure_;
  SendTelemetry *sendTelemetry_ = nullptr;
  int channel_ = 0;
  int sleepForMsec_ = 500;
  std::vector<std::string> commands_;
  int num_trials_ = 10;
  std::regex reg_;
  std::string type_ = "jp";
  int chatter_ = 0;
};

} // namespace gramsballoon::pgrams

#endif //GRAMSBalloon_GetPressure_hh