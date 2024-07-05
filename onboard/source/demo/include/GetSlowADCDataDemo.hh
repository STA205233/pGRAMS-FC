#ifndef GB_GetSlowADCDataDemo_hh
#define GB_GetSlowADCDataDemo_hh 1
#include "GBBasicDemoModule.hh"
#include "SendTelemetry.hh"
namespace gramsballoon {
class SPIManager;
class SendTelemetry;

class GetSlowADCData: public GBBasicDemoModule {
  DEFINE_ANL_MODULE(GetSlowADCData, 1.0);
  ENABLE_PARALLEL_RUN();

public:
  GetSlowADCData() = default;
  virtual ~GetSlowADCData() = default;

protected:
  GetSlowADCData(const GetSlowADCData &r) = default;

public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_pre_initialize() override;
  anlnext::ANLStatus mod_analyze() override;

  uint16_t getADC(int channel) { return singleton_self()->adcList_[channel]; }
  double getVoltage(int channel) { return singleton_self()->voltageList_[channel]; }

private:
  int chipSelect_ = 8;
  std::string SPIManagerName_ = "SPIManager";
  SendTelemetry *sendTelemetry_ = nullptr;
  double va_ = 5.0;
  std::vector<int> channels_;
  std::map<int, uint16_t> adcList_;
  std::map<int, double> voltageList_;
  int numTrials_ = 2;
  int chatter_ = 0;
  double Conversion(uint16_t adc);
  uint16_t InverseConversion(double voltage);
  std::vector<double> voltageMean_;
  std::vector<double> voltageWidth_;
  bool slowADCDaqError_ = false;
  bool SPIManagerNotFound_ = false;
  int GetData(int channel, uint16_t &adc, double &voltage);
};
} // namespace gramsballoon
#endif //GB_GetSlowADCDataDemo_hh