/**
 * Demo module to measure temperature with RTD sensor.
 *
 * @author Shota Arai
 * @date 2024-09-17
 */
#ifndef GRAMSBalloon_MeasureTemperatureWithRTDSensorByArduino_hh
#define GRAMSBalloon_MeasureTemperatureWithRTDSensorByArduino_hh 1
#include "GetArduinoData.hh"
#include "MeasureTemperatureWithRTDSensor.hh"
#include "SendTelemetry.hh"

namespace gramsballoon {
class SendTelemetry;
namespace pgrams {
class MeasureTemperatureWithRTDSensorByArduino: public MeasureTemperatureWithRTDSensor {
  DEFINE_ANL_MODULE(MeasureTemperatureWithRTDSensorByArduino, 1.0);
  ENABLE_PARALLEL_RUN();

public:
  MeasureTemperatureWithRTDSensorByArduino() = default;
  virtual ~MeasureTemperatureWithRTDSensorByArduino() = default;
  int16_t TemperatureADC() override { return singleton_self()->GetArduinoData_->AdcData()[ch_]; }
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_pre_initialize() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_finalize() override;

protected:
  MeasureTemperatureWithRTDSensorByArduino(const MeasureTemperatureWithRTDSensorByArduino &r) = default;

private:
  std::string GetArduinoDataName_ = "GetArduinoData";
  int ch_ = 0;
  std::shared_ptr<GetArduinoData> GetArduinoData_ = nullptr;
  std::shared_ptr<SendTelemetry> sendTelemetry_ = nullptr;
};
} // namespace pgrams
} // namespace gramsballoon

#endif //GRAMSBalloon_MeasureTemperatureWithRTDSensorByArduino_hh