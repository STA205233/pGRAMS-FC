#include "MeasureTemperatureWithRTDSensorByArduino.hh"
using namespace anlnext;
namespace gramsballoon::pgrams {
ANLStatus MeasureTemperatureWithRTDSensorByArduino::mod_define() {
  define_parameter("GetArduinoData_name", &mod_class::GetArduinoDataName_);
  define_parameter("ch", &mod_class::ch_);
  return AS_OK;
}
ANLStatus MeasureTemperatureWithRTDSensorByArduino::mod_pre_initialize() {
  const std::string send_telem_md = "SendTelemetry";
  if (exist_module(send_telem_md)) {
    sendTelemetry_.reset(get_module_NC<SendTelemetry>(send_telem_md));
  }
  return AS_OK;
}
ANLStatus MeasureTemperatureWithRTDSensorByArduino::mod_initialize() {
  if (exist_module(GetArduinoDataName_)) {
    GetArduinoData_.reset(get_module_NC<GetArduinoData>(GetArduinoDataName_));
  }
  else {
    std::cerr << "GetArduinoData does not exist. Module name = " << GetArduinoDataName_ << std::endl;
    if (sendTelemetry_) {
      sendTelemetry_->getErrorManager()->setError(ErrorType::MODULE_ACCESS_ERROR);
    }
  }
  if (ch_ > GetArduinoData_->GetNUMCH()) {
    std::cerr << "Channel number must be smaller than " << GetArduinoData_->GetNUMCH() << std::endl;
    if (sendTelemetry_) {
      sendTelemetry_->getErrorManager()->setError(ErrorType::OTHER_ERRORS);
    }
  }
  else if (ch_ < 0) {
    std::cerr << "Channel number must be non-negative: ch = " << ch_ << std::endl;
    if (sendTelemetry_) {
      sendTelemetry_->getErrorManager()->setError(ErrorType::OTHER_ERRORS);
    }
  }
  return AS_OK;
}
ANLStatus MeasureTemperatureWithRTDSensorByArduino::mod_analyze() {
  return AS_OK;
}
ANLStatus MeasureTemperatureWithRTDSensorByArduino::mod_finalize() {
  return AS_OK;
}
} // namespace gramsballoon::pgrams