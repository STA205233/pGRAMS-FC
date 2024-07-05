#ifndef GB_DEMO_MODE
#include "MeasureAcceleration.hh"

using namespace anlnext;

namespace gramsballoon {

MeasureAcceleration::MeasureAcceleration()
{
  icmIO_ = std::make_shared<ICM20948IO>();
  devicePath_ = "/dev/i2c-1";
}

MeasureAcceleration::~MeasureAcceleration() = default;

ANLStatus MeasureAcceleration::mod_define()
{
  define_parameter("device_path", &mod_class::devicePath_);
  define_parameter("calibrate_gyro", &mod_class::calibrateGyro_);
  define_parameter("chatter", &mod_class::chatter_);

  return AS_OK;
}

ANLStatus MeasureAcceleration::mod_initialize()
{
  const std::string send_telem_md = "SendTelemetry";
  if (exist_module(send_telem_md)) {
    get_module_NC(send_telem_md, &sendTelemetry_);
  }

  const bool status = icmIO_->initialize();
  if (!status) {
    std::cerr << "Error in MeasureAcceleration::mod_initialize: Device not found." << std::endl;
    if (sendTelemetry_) {
      sendTelemetry_->getErrorManager()->setError(ErrorType::ACCEL_DEVICE_NOT_FOUND);
    }
  }
  if (calibrateGyro_) {
    icmIO_->calibrateGyro();
  }
  return AS_OK;
}

ANLStatus MeasureAcceleration::mod_analyze()
{
  icmIO_->measure();

  if (chatter_>=1) {
    debug();
  }

  return AS_OK;
}

ANLStatus MeasureAcceleration::mod_finalize()
{
  return AS_OK;
}

void MeasureAcceleration::debug()
{
  const IMUData* data = getData();
  std::cout << "Accel (g): ";
  for (int j = 0; j < 3; j++) {
    std::cout << data->mAcc[j] << " ";
  }
  std::cout << std::endl;
  std::cout << "Gyro (dps): ";
  for (int j = 0; j < 3; j++) {
    std::cout << data->mGyro[j] << " ";
  }
  std::cout << std::endl;
  std::cout << "Magnet (uT)";
  for (int j = 0; j < 3; j++) {
    std::cout << data->mMag[j] << " ";
  }
  std::cout << std::endl;
}

} /* namespace gramsballoon */
#endif /* GB_DEMO_MODE */