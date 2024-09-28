#include "GetSlowADCDataIncludingPressure.hh"
using namespace anlnext;

namespace gramsballoon::pgrams {
ANLStatus GetSlowADCDataIncludingPressure::mod_define() {
  GetSlowADCData::mod_define();
  define_parameter("GetPressure_names", &mod_class::getPressureNames_);
  define_parameter("channels", &mod_class::channels_);
  return AS_OK;
}
ANLStatus GetSlowADCDataIncludingPressure::mod_initialize() {
  SendTelemetry *send_telemetry = GetSendTelemetry();
  GetSlowADCData::mod_initialize();
  for (int i = 0; i < static_cast<int>(getPressureNames_.size()); i++) {
    if (exist_module(getPressureNames_[i])) {
      getPressures_.push_back(get_module_NC<GetPressure>(getPressureNames_[i]));
    }
    else {
      std::cerr << getPressureNames_[i] << " does not exist." << std::endl;
      if (send_telemetry) {
        send_telemetry->getErrorManager()->setError(ErrorType::MODULE_ACCESS_ERROR);
      }
      getPressures_.push_back(nullptr);
    }
  }
  return AS_OK;
}
ANLStatus GetSlowADCDataIncludingPressure::mod_analyze() {
  GetSlowADCData::mod_analyze();
  for (int i = 0; i < static_cast<int>(getPressureNames_.size()); i++) {
    if (getPressures_[i]) {
      SetADCData(channels_[i], static_cast<uint16_t>(getPressures_[i]->Pressure() * 1e3)); // TODO: 1e3 means 1e6 * 1e-3 (to match the unit of HV setting)
      SetVoltage(channels_[i], getPressures_[i]->Pressure() * 1e3); // TODO: 1e3 means 1e6 * 1e-3 (to match the unit of HV setting)
    }
  }
  return AS_OK;
}
} // namespace gramsballoon::pgrams