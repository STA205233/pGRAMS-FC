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

private:
  static constexpr int NUM_PRESSURE = 2;
  static constexpr int NUM_TEMPERATURE = 4;

public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_finalize() override;
  int Temperature(int index) const {
    return singleton_self()->temperature_[index];
  }
  int Pressure(int index) const {
    return singleton_self()->pressure_[index];
  }
  constexpr int NumPressure() const { return NUM_PRESSURE; }
  constexpr int NumTemperature() const { return NUM_TEMPERATURE; }

private:
  std::string encodedSerialCommunicatorName_ = "EncodedSerialCommunicator";
  EncodedSerialCommunicator *communicator_ = nullptr;
  std::array<int, NUM_TEMPERATURE> temperature_;
  std::array<int, NUM_PRESSURE> pressure_;
  std::regex regTemp_;
  std::regex regPress_;
  int sleepForMsec_ = 500;
  int chatter_ = 0;
};
} // namespace gramsballoon::pgrams
#endif //GRAMSBalloon_GetCompressorData_hh