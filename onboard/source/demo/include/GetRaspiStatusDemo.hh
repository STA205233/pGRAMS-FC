#ifndef GB_GetRaspiStatusDemo_hh
#define GB_GetRaspiStatusDemo_hh 1
#include "GBBasicDemoModule.hh"
#include "SendTelemetry.hh"
namespace gramsballoon {
class GetRaspiStatus: public GBBasicDemoModule {
  DEFINE_ANL_MODULE(GetRaspiStatus, 1.0);
  ENABLE_PARALLEL_RUN();

public:
  GetRaspiStatus() = default;
  virtual ~GetRaspiStatus() = default;

protected:
  GetRaspiStatus(const GetRaspiStatus &r) = default;

public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;

  int getCapacity();

  int CPUTemperatureADC() { return singleton_self()->CPUTemperatureADC_; }
  double CPUTemperature() { return singleton_self()->CPUTemperature_; }
  uint64_t CapacityFree() { return singleton_self()->capacityFree_; }
  uint64_t CapacityAll() { return singleton_self()->capacityAll_; }

private:
  int CPUTemperatureADC_;
  double CPUTemperature_;
  int CPUTempertaureWidthADC_ = 0;
  double CPUTemperatureWidth_ = 2.0;
  int CPUTemperatureMeanADC_ = 0;
  double CPUTemperatureMean_ = 58.0;
  uint64_t capacityFree_ = 0;
  uint64_t capacityAll_ = 0;
  SendTelemetry *sendTelemetry_ = nullptr;
  int chatter_ = 0;
  double TConversion(int adc) const;
  int TInverseConversion(double temperature) const;
};
} // namespace gramsballoon
#endif //GB_GetRaspiStatusDemo_hh