#ifndef GRAMSBalloon_GetPressure_hh
#define GRAMSBalloon_GetPressure_hh 1
#include "EncodedSerialCommunicator.hh"
#include "anlnext/BasicModule.hh"
#include <regex>

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

private:
  EncodedSerialCommunicator *encodedSerialCommunicator_ = nullptr;
  std::string encodedSerialCommunicatorName_ = "EncodedSerialCommunicator";
  float pressure_ = 0;
  int channel_ = 0;
  std::string command_ = "";
  std::regex reg_;
};

} // namespace gramsballoon::pgrams

#endif //GRAMSBalloon_GetPressure_hh