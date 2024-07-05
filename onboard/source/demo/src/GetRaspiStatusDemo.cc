#include "GetRaspiStatus.hh"

using namespace anlnext;
namespace gramsballoon {
ANLStatus GetRaspiStatus::mod_define() {
  GBBasicDemoModule::mod_define();
  define_parameter("chatter", &mod_class::chatter_);
  define_parameter("CPU_temperature_mean", &mod_class::CPUTemperatureMean_);
  define_parameter("CPU_temperature_width", &mod_class::CPUTemperatureWidth_);
  define_parameter("SD_capacity_MB", &mod_class::capacityFree_);
  return AS_OK;
}
ANLStatus GetRaspiStatus::mod_initialize() {
  GBBasicDemoModule::mod_initialize();
  const std::string send_telem_md = "SendTelemetry";
  if (exist_module(send_telem_md)) {
    get_module_NC(send_telem_md, &sendTelemetry_);
  }
  CPUTemperatureMeanADC_ = TInverseConversion(CPUTemperatureMean_);
  CPUTempertaureWidthADC_ = TInverseConversion(CPUTemperatureWidth_);
  const uint64_t one = 1;
  capacityFree_ = capacityFree_ * (one << 20);
  capacityAll_ = 5000 * (one << 20);
  return AS_OK;
}
ANLStatus GetRaspiStatus::mod_analyze() {
  CPUTemperatureADC_ = static_cast<int>(SampleFromUniformDistribution() * CPUTempertaureWidthADC_ + CPUTemperatureMeanADC_);
  CPUTemperature_ = TConversion(CPUTemperatureADC_);
  if (chatter_ >= 1) {
    std::cout << "CPU temperature ADC: " << CPUTemperatureADC_ << std::endl;
    std::cout << "CPU temperature: " << CPUTemperature_ << std::endl;
  }
  return AS_OK;
}
double GetRaspiStatus::TConversion(int adc) const {
  const double res = adc / 1000.0;
  return res;
}
int GetRaspiStatus::TInverseConversion(double temperature) const {
  const int res = static_cast<int>(temperature * 1000.0);
  return res;
}
} // namespace gramsballoon