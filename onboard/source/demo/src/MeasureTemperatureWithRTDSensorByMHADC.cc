#include "MeasureTemperatureWithRTDSensorByMHADC.hh"
using namespace anlnext;

namespace gramsballoon::pgrams {
ANLStatus MeasureTemperatureWithRTDSensorByMHADC::mod_define() {
  define_parameter("GetMHADCData_name", &mod_class::getMHADCDataName_);
  define_parameter("channel", &mod_class::channel_);
  define_parameter("bit", &mod_class::bit_);
  define_parameter("offset", &mod_class::offset_);
  define_parameter("chatter", &mod_class::chatter_);
  return AS_OK;
}
ANLStatus MeasureTemperatureWithRTDSensorByMHADC::mod_initialize() {
  if (exist_module("SendTelemetry")) {
    get_module_NC("SendTelemetry", &sendTelemetry_);
  }
  if (exist_module(getMHADCDataName_)) {
    get_module_NC(getMHADCDataName_, &getMHADCData_);
  }
  else {
    std::cerr << getMHADCDataName_ << " does not exist." << std::endl;
    if (sendTelemetry_) {
      sendTelemetry_->getErrorManager()->setError(ErrorType::MODULE_ACCESS_ERROR);
    }
  }
  if (channel_ > getMHADCData_->NumCH()) {
    std::cerr << "Channel number must be smaller than " << getMHADCData_->NumCH() << std::endl;
    if (sendTelemetry_) {
      sendTelemetry_->getErrorManager()->setError(ErrorType::OTHER_ERRORS);
    }
    hasProblem_ = true;
  }
  else if (channel_ < 0) {
    std::cerr << "Channel number must be non-negative: ch = " << channel_ << std::endl;
    if (sendTelemetry_) {
      sendTelemetry_->getErrorManager()->setError(ErrorType::OTHER_ERRORS);
    }
    hasProblem_ = true;
  }
  return AS_OK;
}
ANLStatus MeasureTemperatureWithRTDSensorByMHADC::mod_analyze() {
  if (hasProblem_) {
    return AS_OK;
  }
  if (getMHADCData_) {
    double temp = ConvertTemperature(getMHADCData_->AdcData()[channel_], bit_, offset_);
    if (!std::isfinite(temp)) {
      std::cerr << this->module_id() << " Temperature is invalid: " << temp << std::endl;
      if (sendTelemetry_) {
        sendTelemetry_->getErrorManager()->setError(ConvertRTDError(channel_));
      }
      temp = 0;
    }
    if (chatter_ > 0) {
      std::cout << this->module_id() << " Temperature: " << temp << std::endl;
    }
    SetTemperature(temp);
    SetTemperatureADC(static_cast<int>(temp * 10));
  }
  else {
    SetTemperature(0);
    SetTemperatureADC(0);
  }
  return AS_OK;
}
float MeasureTemperatureWithRTDSensorByMHADC::ConvertTemperature(int adc, int bit, int offset) const {
  const float L_V = adc * 5.0 / (1 << bit);
  const float L_R = L_V * 1000. / (5.0 - L_V);
  const float L_tmp = -(std::sqrt(17.59246 - 0.00232 * L_R) - 3.908) / 0.00116;
  const float L_tmpK = L_tmp + 273.15;
  const float L_correction = L_tmpK + offset; // No correction for now
  return std::round(L_correction * 100) / 100; // Round to 2 digits
}
} // namespace gramsballoon::pgrams