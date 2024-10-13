#include "MeasureTemperatureWithRTDSensorByArduino.hh"
using namespace anlnext;
namespace gramsballoon::pgrams {
ANLStatus MeasureTemperatureWithRTDSensorByArduino::mod_define() {
  define_parameter("GetArduinoData_name", &mod_class::GetArduinoDataName_);
  define_parameter("channel", &mod_class::ch_);
  define_parameter("bit", &mod_class::bit_);
  define_parameter("offset", &mod_class::offset_);
  define_parameter("chatter", &mod_class::chatter_);
  return AS_OK;
}
ANLStatus MeasureTemperatureWithRTDSensorByArduino::mod_pre_initialize() {
  const std::string send_telem_md = "SendTelemetry";
  if (exist_module(send_telem_md)) {
    get_module_NC(send_telem_md, &sendTelemetry_);
  }
  return AS_OK;
}
ANLStatus MeasureTemperatureWithRTDSensorByArduino::mod_initialize() {
  if (exist_module(GetArduinoDataName_)) {
    get_module(GetArduinoDataName_, &getArduinoData_);
  }
  else {
    std::cerr << "GetArduinoData does not exist. Module name = " << GetArduinoDataName_ << std::endl;
    if (sendTelemetry_) {
      sendTelemetry_->getErrorManager()->setError(ErrorType::MODULE_ACCESS_ERROR);
    }
  }
  if (ch_ > getArduinoData_->NumCH()) {
    std::cerr << "Channel number must be smaller than " << getArduinoData_->NumCH() << std::endl;
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
  if (getArduinoData_) {
    const double temp = ConvertTemperature(getArduinoData_->AdcData()[ch_], bit_, offset_);
    if (chatter_ > 0){
      std::cout << this->module_id() << " Temperature: " << temp << std::endl;
    }
    SetTemperature(temp);
    SetTemperatureADC(static_cast<int>(temp * 10));
  }
  else {
    if (chatter_ > 0) {
      std::cout << this->module_id() << " Temperature: " << 0 << std::endl;
    }
    SetTemperature(0);
    SetTemperatureADC(0);
  }
  return AS_OK;
}
ANLStatus MeasureTemperatureWithRTDSensorByArduino::mod_finalize() {
  return AS_OK;
}
float MeasureTemperatureWithRTDSensorByArduino::ConvertTemperature(int adc, int bit, int offset) const {
  const float L_V = adc * 5.0 / (1 << bit);
  const float L_R = L_V * 1000. / (5.0 - L_V);
  const float L_tmp = -(std::sqrt(17.59246 - 0.00232 * L_R) - 3.908) / 0.00116;
  const float L_tmpK = L_tmp + 273.15;
  const float L_correction = L_tmpK + offset; // No correction for now
  return std::round(L_correction * 100) / 100; // Round to 2 digits
}
} // namespace gramsballoon::pgrams