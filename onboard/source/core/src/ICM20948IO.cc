#ifndef GB_DEMO_MODE
#include "ICM20948IO.hh"

namespace gramsballoon {

ICM20948IO::ICM20948IO(): devicePath_("/dev/i2c-1")
{
}

ICM20948IO::~ICM20948IO() = default;

bool ICM20948IO::initialize()
{
  strcpy(conf_.mDevice, devicePath_.c_str());
  conf_.mAcc.mRange = ICM20948::ACC_RANGE::ACC_RANGE_16G;
  conf_.mGyro.mRange = ICM20948::GYRO_RANGE::GYRO_RANGE_1000DPS;
  bool rslt = icm_.initialise(conf_);
  if (!rslt) {
    std::cerr << "Device not found" << std::endl;
  }
  return rslt;
}

void ICM20948IO::measure()
{
  data_ = &icm_.imuDataGet();
  waitForProcess();
}


void ICM20948IO::waitForProcess()
{
  const double frame_time = 1.0 / icm_.getConfig().mFramerate;
  int sleep_time = static_cast<int>(frame_time * 1.0E6);
  if (sleep_time > 0) {
    std::this_thread::sleep_for(std::chrono::microseconds(sleep_time));
  }
}

/**
 * @brief Calibrates the gyroscope by averaging 1024 samples of gyro and uploading them as a bias to the device.
 * @note Please make sure to be stationary!
 */
void ICM20948IO::calibrateGyro()
{
  icm_.calibrateGyro();
}

} /* namespace gramsballoon */
#endif // GB_DEMO_MODE
