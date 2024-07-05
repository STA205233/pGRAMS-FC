#ifndef MeasureAccelerationDemo_hh
#define MeasureAccelerationDemo_hh 1
#include "GBBasicDemoModule.hh"
#include "SendTelemetry.hh"
#include <string>

namespace gramsballoon {
class SendTelemetry;
class MeasureAcceleration: public GBBasicDemoModule {
  DEFINE_ANL_MODULE(MeasureAcceleration, 1.0);
  ENABLE_PARALLEL_RUN();

public:
  MeasureAcceleration() = default;
  virtual ~MeasureAcceleration() = default;

protected:
  MeasureAcceleration(const MeasureAcceleration &r) = default;

public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;

  const float *getAcceleration() const { return mAcc_; }
  float getAcceleration(int index) { return (index >= 0 && index < 3) ? mAcc_[index] : 0; }
  const float *getGyro() const { return mGyro_; }
  float getGyro(int index) { return (index >= 0 && index < 3) ? mGyro_[index] : 0; }
  const float *getMagnet() const { return mMag_; }
  float getMagnet(int index) { return (index >= 0 && index < 3) ? mMag_[index] : 0; }
  float getTemperature() { return mTemp_; }

private:
  float mAcc_[3] = {0.0, 0.0, 1.0};
  float mMag_[3] = {0.0, 0.0, 0.0};
  float mGyro_[3] = {0.0, 0.0, 0.0};
  float mTemp_ = 0.0;
  SendTelemetry *sendTelemetry_ = nullptr;
  bool accelDeviceNotFound_ = false;
  double temperatureMean_ = 20.0;
  int chatter_ = 0;
  std::string devicePath_ = "";
  bool calibrateGyro_ = false;
};
} // namespace gramsballoon
#endif //MeasureAccelerationDemo_hh