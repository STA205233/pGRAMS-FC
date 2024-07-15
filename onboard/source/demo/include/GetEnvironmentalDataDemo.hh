#ifndef GB_GetEnvironmentalDataDemo_H
#define GB_GetEnvironmentalDataDemo_H 1
#include "GBBasicDemoModule.hh"
#include "SendTelemetry.hh"
#include <chrono>

namespace gramsballoon {
class SendTelemetry;
class GetEnvironmentalData: public GBBasicDemoModule {
  DEFINE_ANL_MODULE(GetEnvironmentalData, 1.0);
  ENABLE_PARALLEL_RUN();

public:
  GetEnvironmentalData() = default;
  virtual ~GetEnvironmentalData() = default;

protected:
  GetEnvironmentalData(const GetEnvironmentalData &r) = default;

public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_pre_initialize() override;
  anlnext::ANLStatus mod_analyze() override;

  void setDataAquisitionError();

  double Temperature() { return singleton_self()->temperature_; }
  double Humidity() { return singleton_self()->humidity_; }
  double Pressure() { return singleton_self()->pressure_; }

private:
  int chipSelect_ = 8;
  SendTelemetry *sendTelemetry_ = nullptr;
  std::string SPIManagerName_ = "SPIManager";
  double pressure_;
  double humidity_;
  double temperature_;
  int chatter_ = 0;
  bool SPIManagerNotFound_ = false;
  double pressureMean_ = 101525;
  double humidityMean_ = 4.0;
  double temperatureMean_ = 20.0;
  double pressureWidth_ = 1000.0;
  double humidityWidth_ = 1;
  double temperatureWidth_ = 1.0;
  int GetData();
  double rateOfFailure_ = 0.0;
};
} // namespace gramsballoon
#endif //GB_GetEnvironmentalDataDemo_H