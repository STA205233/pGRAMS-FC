/**
 * Get environmental data with BME680 sensor.
 *
 * @author Tsubasa Tamba, Shota Arai
 * @date 2023-03-01
 */


#ifndef GetEnvironmentalData_H
#define GetEnvironmentalData_H 1
#ifdef GB_DEMO_MODE
#include "GetEnvironmentalDataDemo.hh"
#else /* GB_DEMO_MODE */
#include <anlnext/BasicModule.hh>
#include "SPIInterface.hh"
#include "BME680IO.hh"
#include "SPIManager.hh"
#include "SendTelemetry.hh"
#include <chrono>

namespace gramsballoon {

class SPIManager;
class SendTelemetry;

class GetEnvironmentalData : public anlnext::BasicModule
{
  DEFINE_ANL_MODULE(GetEnvironmentalData, 1.0);
  ENABLE_PARALLEL_RUN();

public:
  GetEnvironmentalData();
  virtual ~GetEnvironmentalData();
protected:
  GetEnvironmentalData(const GetEnvironmentalData& r) = default;

public:  
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_pre_initialize() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_finalize() override;

  void setDataAquisitionError();

  BME680IO* GetBME680IO() { return (singleton_self()->bme680io_).get(); }
  double Temperature() { return singleton_self()->temperature_; }
  double Humidity() { return singleton_self()->humidity_; }
  double Pressure() { return singleton_self()->pressure_; }

private:
  int chipSelect_ = 8;
  std::string SPIManagerName_ = "SPIManager";
  SPIManager* SPIManager_ = nullptr;
  SendTelemetry* sendTelemetry_ = nullptr;
  std::shared_ptr<BME680IO> bme680io_ = nullptr;
  std::shared_ptr<SPIInterface> interface_ = nullptr;
  double pressure_;
  double humidity_;
  double temperature_;
  int chatter_ = 0;
};

} /* namespace gramsballoon */
#endif /* GB_DEMO_MODE */
#endif /* GetEnvironmentalData_H */
