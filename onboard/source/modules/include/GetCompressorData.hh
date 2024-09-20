#ifndef GRAMSBalloon_GetCompressorData_hh
#define GRAMSBalloon_GetCompressorData_hh 1
#include "EncodedSerialCommunicator.hh"
#include "anlnext/BasicModule.hh"
#include <regex>
#include <string>

namespace gramsballoon::pgrams {
class GetCompressorData: public anlnext::BasicModule {
  DEFINE_ANL_MODULE(GetCompressorData, 1.0);
  ENABLE_PARALLEL_RUN();

public:
  GetCompressorData() = default;
  virtual ~GetCompressorData() = default;

protected:
  GetCompressorData(const GetCompressorData &r) = default;

public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_finalize() override;

private:
  static constexpr int NUM_PRESSURE = 2;
  static constexpr int NUM_TEMPERATURE = 4;
  std::string encodedSerialCommunicatorName_ = "EncodedSerialCommunicator";
  std::shared_ptr<EncodedSerialCommunicator> communicator_ = nullptr;
  std::array<int, NUM_TEMPERATURE> temperature_;
  std::array<int, NUM_PRESSURE> pressure_;
  std::regex regTemp_;
  std::regex regPress_;
};
} // namespace gramsballoon::pgrams
#endif //GRAMSBalloon_GetCompressorData_hh