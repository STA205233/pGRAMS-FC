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
  void SetADCData(int channel, uint16_t adc) { singleton_self()->adcList_[channel] = adc; }
  void SetVoltage(int channel, double voltage) { singleton_self()->voltageList_[channel] = voltage; }
  SendTelemetry *GetSendTelemetry() { return singleton_self()->sendTelemetry_; }

public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_pre_initialize() override;
  anlnext::ANLStatus mod_analyze() override;

  uint16_t getADC(int channel) { return singleton_self()->adcList_[channel]; }
  double getVoltage(int channel) { return singleton_self()->voltageList_[channel]; }
  double Conversion(uint16_t adc) const;
  uint16_t InverseConversion(double voltage) const;
  double CurrentInverseConversion(double current) const;
  double CurrentConversion(double voltage_adc) const;
  double MainVoltageInverseConversion(double voltage) const;
  double MainVoltageConversion(double voltage) const;
  double TPCHVVoltageInverseConversion(double voltage) const;
  double TPCHVVoltageConversion(double voltage_adc) const;
  double TPCHVCurrentConversion(double voltage) const;
  double TPCHVCurrentInverseConversion(double voltage) const;
  double ChamberPressureConversion(double voltage_adc) const;
  double ChamberPressureInverseConversion(double pressure) const;

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
  std::vector<double> voltageMean_;
  std::vector<double> voltageWidth_;
  bool slowADCDaqError_ = false;
  bool SPIManagerNotFound_ = false;
  int mainVoltageChennal_ = -1;
  int mainCurrentChannel_ = -1;
  int tpcHVVoltageChannel_ = -1;
  int tpcHVCurrentChannel_ = -1;
  int chamberPressureChannel_ = -1;
  int GetData(int channel, uint16_t &adc, double &voltage);
};
} // namespace gramsballoon
#endif //GB_GetSlowADCDataDemo_hh