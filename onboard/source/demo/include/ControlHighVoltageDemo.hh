/**
 * Control high voltage via Analog Discovery. Demo version.
 *
 * @author Shota Arai
 * @date 2024-07-02
 */
#ifndef GB_ControlHighVoltageDemo_hh
#define GB_ControlHighVoltageDemo_hh 1
#include "GBBasicDemoModule.hh"
#include "SendTelemetry.hh"
#include "anlnext/BasicModule.hh"
namespace gramsballoon {
class SendTelemetry;
class ControlHighVoltage: public GBBasicDemoModule {
  DEFINE_ANL_MODULE(ControlHighVoltage, 1.0);
  ENABLE_PARALLEL_RUN();

public:
  ControlHighVoltage() = default;
  virtual ~ControlHighVoltage() = default;

protected:
  ControlHighVoltage(const ControlHighVoltage &r) = default;

public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_analyze() override;

  void setInvalidChannelError();

  bool setNextVoltage(double v);
  void setExec(bool v) { singleton_self()->exec_ = v; }
  double CurrentVoltage() { return singleton_self()->currentVoltage_; }
  double NextVoltage() { return singleton_self()->nextVoltage_; }
  double UpperLimitVoltage() { return singleton_self()->upperLimitVoltage_; }
  void setUpperLimitVoltage(double v) { singleton_self()->upperLimitVoltage_ = v; }

private:
  int sleep_ = 500; // ms
  int deviceID_ = 0;
  int channel_ = 0;
  bool invalidChannelError_ = false;
  double currentVoltage_ = 0.0; // volt
  double nextVoltage_ = 0.0; // volt
  bool exec_ = false;
  std::vector<double> voltages_;
  int voltageIndex_ = 0;
  SendTelemetry *sendTelemetry_ = nullptr;
  double upperLimitVoltage_ = 0.0;
  int chatter_ = 0;
};
} // namespace gramsballoon

#endif //GB_ControlHighVoltageDemo_hh