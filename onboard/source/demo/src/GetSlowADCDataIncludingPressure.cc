#include "GetSlowADCDataIncludingPressure.hh"
using namespace anlnext;

namespace gramsballoon::pgrams {
ANLStatus GetSlowADCDataIncludingPressure::mod_define() {
  GetSlowADCData::mod_define();
  define_parameter("GetPressure_name", &mod_class::getPressureName_);
  define_parameter("channel", &mod_class::channel_);
  return AS_OK;
}
ANLStatus GetSlowADCDataIncludingPressure::mod_initialize() {
  SendTelemetry *send_telemetry = GetSendTelemetry();
  GetSlowADCData::mod_initialize();
  if (exist_module(getPressureName_)) {
    get_module_NC(getPressureName_, &getPressure_);
  }
  else {
    std::cerr << getPressureName_ << " does not exist." << std::endl;
    if (send_telemetry) {
      send_telemetry->getErrorManager()->setError(ErrorType::MODULE_ACCESS_ERROR);
    }
  }
  return AS_OK;
}
ANLStatus GetSlowADCDataIncludingPressure::mod_analyze() {
  GetSlowADCData::mod_analyze();
  if (getPressure_) {
    SetADCData(channel_, static_cast<uint16_t>(getPressure_->Pressure()));
    SetVoltage(channel_, getPressure_->Pressure());
  }
  return AS_OK;
}
} // namespace gramsballoon::pgrams