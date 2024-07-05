#include "MeasureAcceleration.hh"
using namespace anlnext;
namespace gramsballoon {
ANLStatus MeasureAcceleration::mod_define() {
  GBBasicDemoModule::mod_define();
  define_parameter("accel_device_not_found", &mod_class::accelDeviceNotFound_);
  define_parameter("temperature_mean", &mod_class::temperatureMean_);
  define_parameter("device_path", &mod_class::devicePath_);
  define_parameter("calibrate_gyro", &mod_class::calibrateGyro_);
  define_parameter("chatter", &mod_class::chatter_);
  return AS_OK;
}
ANLStatus MeasureAcceleration::mod_initialize() {
  GBBasicDemoModule::mod_initialize();
  const std::string send_telem_md = "SendTelemetry";
  if (exist_module(send_telem_md)) {
    get_module_NC(send_telem_md, &sendTelemetry_);
  }
  if (accelDeviceNotFound_) {
    if (sendTelemetry_) {
      sendTelemetry_->getErrorManager()->setError(ErrorType::ACCEL_DEVICE_NOT_FOUND);
    }
  }
  return AS_OK;
}
ANLStatus MeasureAcceleration::mod_analyze() {
  for (int i = 0; i < 3; i++) {
    mAcc_[i] = SampleFromUniformDistribution();
    mGyro_[i] = SampleFromUniformDistribution();
    mMag_[i] = SampleFromUniformDistribution();
  }
  mTemp_ = SampleFromUniformDistribution() + temperatureMean_;
  return AS_OK;
}
} // namespace gramsballoon
