/**
 * Demo module to measure temperature with RTD sensor.
 *
 * @author Shota Arai
 * @date 2024-07-02
 */
#ifndef GB_MeasureTemperatureWithRTDSensorDemo_hh
#define GB_MeasureTemperatureWithRTDSensorDemo_hh 1
#include "GBBasicDemoModule.hh"
#include "SendTelemetry.hh"
#include "anlnext/BasicModule.hh"
#include <chrono>
namespace gramsballoon {
class MeasureTemperatureWithRTDSensor: public GBBasicDemoModule {
  DEFINE_ANL_MODULE(MeasureTemperatureWithRTDSensor, 1.0);
  ENABLE_PARALLEL_RUN();

public:
  MeasureTemperatureWithRTDSensor() = default;
  virtual ~MeasureTemperatureWithRTDSensor() = default;

protected:
  MeasureTemperatureWithRTDSensor(const MeasureTemperatureWithRTDSensor &r) = default;
  double SetTemperature(double temperature) {
    return singleton_self()->temperature_ = temperature;
  }
  double SetTemperatureADC(int temperatureADC) {
    return singleton_self()->temperatureADC_ = temperatureADC;
  }

public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_pre_initialize() override;
  void setDataAquisitionError();
  virtual int16_t TemperatureADC() { return singleton_self()->temperatureADC_; }
  double TConversion(int adc) const;
  int TInverseConversion(double temperature) const;

private:
  std::shared_ptr<SendTelemetry> sendTelemetry_ = nullptr;
  int chatter_ = 0;
  double temperature_ = 0.0;
  const int rRef_ = 430;
  int temperatureADC_ = 0;
  double meanTemperature_ = 25.0;
  double temperatureWidth_ = 1.0;
  bool rtdDataAquisitionError_ = false;
  bool SPIManagerNotFound_ = false;
  int chipSelect_ = 8;
  std::string SPIManagerName_ = "SPIManager";
};
} // namespace gramsballoon
#endif //GB_MeasureTemperatureWithRTDDemo_hh