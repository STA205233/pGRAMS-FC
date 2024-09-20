#ifndef GRAMSBalloon_GetMHADCData_hh
#define GRAMSBalloon_GetMHADCData_hh 1
#include "EncodedSerialCommunicator.hh"
#include "anlnext/BasicModule.hh"
#include <regex>

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
    return adcData_;
  }
  int NumCH() const {
    return numCh_;
  }

private:
  int numCh_ = 32;
  std::vector<int> adcData_;
  std::shared_ptr<EncodedSerialCommunicator> encodedSerialCommunicator_ = nullptr;
  std::string encodedSerialCommunicatorName_ = "EncodedSerialCommunicator";
};
} // namespace gramsballoon::pgrams
#endif //GRAMSBalloon_GetMHADCData_hh