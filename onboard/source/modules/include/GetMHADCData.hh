/**
 *  Module for communicating with MHADC.
 *
 * @author Shota Arai
 * @date 2024-09-19
 * 
 */
#ifndef GRAMSBalloon_GetMHADCData_hh
#define GRAMSBalloon_GetMHADCData_hh 1
#include "EncodedSerialCommunicator.hh"
#include "SendTelemetry.hh"
#include "anlnext/BasicModule.hh"
#include <regex>

namespace gramsballoon{
class SendTelemetry;
}

namespace gramsballoon::pgrams {
class GetMHADCData: public anlnext::BasicModule {
  DEFINE_ANL_MODULE(GetMHADCData, 1.0);
  ENABLE_PARALLEL_RUN();

public:
  GetMHADCData() = default;
  virtual ~GetMHADCData() = default;

protected:
  GetMHADCData(const GetMHADCData &r) = default;

public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_finalize() override;
  const std::vector<int> &AdcData() const {
    return singleton_self()->adcData_;
  }
  int NumCH() const {
    return numCh_;
  }

private:
  int numCh_ = 32;
  SendTelemetry* sendTelemetry_ = nullptr;
  std::vector<int> adcData_;
  std::vector<std::regex> regs_;
  int sleepForMsec_ = 500;
  int chatter_ = 0;
  int numTrials_ = 10;
  EncodedSerialCommunicator *encodedSerialCommunicator_ = nullptr;
  std::string encodedSerialCommunicatorName_ = "EncodedSerialCommunicator";
};
} // namespace gramsballoon::pgrams
#endif //GRAMSBalloon_GetMHADCData_hh