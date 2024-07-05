/**
 * Get data of acceleration, gyro and magnet from ICM20948
 *
 * @author Shota Arai, Tsubasa Tamba
 * @date 2023-04-04
 *
 */

#ifndef MeasureAcceleration_H
#define MeasureAcceleration_H 1
#ifdef GB_DEMO_MODE
#include "MeasureAccelerationDemo.hh"
#else /* GB_DEMO_MODE */

#include <anlnext/BasicModule.hh>
#include "ICM20948IO.hh"
#include "SendTelemetry.hh"
#include <string.h>
#include <thread>
#include <chrono>
#include <string>

namespace gramsballoon {

class SendTelemetry;

class MeasureAcceleration : public anlnext::BasicModule
{
  DEFINE_ANL_MODULE(MeasureAcceleration, 1.0);
  ENABLE_PARALLEL_RUN();

public:
  MeasureAcceleration();
  virtual ~MeasureAcceleration();

protected:
  MeasureAcceleration(const MeasureAcceleration &r) = default;

public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_finalize() override;

  void debug();

  const IMUData* getData() const { return icmIO_->getData(); }
  const float* getAcceleration() const { return icmIO_->getData()->mAcc; }
  float getAcceleration(int index) { return (index>=0 && index<3) ? icmIO_->getData()->mAcc[index] : 0; }
  const float* getGyro() const { return icmIO_->getData()->mGyro; }
  float getGyro(int index) { return (index>=0 && index<3) ? icmIO_->getData()->mGyro[index] : 0; }
  const float* getMagnet() const { return icmIO_->getData()->mMag; }
  float getMagnet(int index) { return (index>=0 && index<3) ? icmIO_->getData()->mMag[index] : 0; }
  float getTemperature() { return icmIO_->getData()->mTemp; }

private:
  std::shared_ptr<ICM20948IO> icmIO_;
  std::string devicePath_ = "";
  bool calibrateGyro_ = false;
  int chatter_ = 0;
  SendTelemetry* sendTelemetry_ = nullptr;
};

} /* namespace gramsballoon */
#endif /* GB_DEMO_MODE */
#endif /*MeasureAcceleration_H*/
