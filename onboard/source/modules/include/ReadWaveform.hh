/**
 * Regulate high voltage via Analog Discovery.
 *
 * @author Tsubasa Tamba, Masato Kimura
 * @date 2023-03-22
 */


#ifndef ReadWaveform_H
#define ReadWaveform_H 1

#ifdef GB_DEMO_MODE
#include "ReadWaveformDemo.hh"
#else
#include<fstream>
#include <anlnext/BasicModule.hh>
#include "AnalogDiscoveryManager.hh"
#include "DAQIO.hh"
#include "SendTelemetry.hh"
#include "TelemetryDefinition.hh"
#include "RunIDManager.hh"

namespace gramsballoon {

class AnalogDiscoveryManager;
class SendTelemetry;
class RunIDManager;

class ReadWaveform : public anlnext::BasicModule
{
  DEFINE_ANL_MODULE(ReadWaveform, 1.0);
  ENABLE_PARALLEL_RUN();

public:
  ReadWaveform();
  virtual ~ReadWaveform();
protected:
  ReadWaveform(const ReadWaveform& r) = default;

public:  
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_finalize() override;

  void setupAnalogIn();
  void createNewOutputFile();
  void closeOutputFile();
  void writeData();

  const std::vector<std::vector<int16_t>>& EventData() const { return singleton_self()->eventData_; }
  void setOndemand(bool v) { singleton_self()->ondemand_ = v; }
  bool getOndemand() { return singleton_self()->ondemand_; }
  DAQIO* getDAQIO() { return (singleton_self()->daqio_).get(); }

  bool StartReading() { return singleton_self()->startReading_; }
  uint32_t EventID() { return singleton_self()->eventID_; }
  uint32_t EventCount() { return daqio_->EventCount(); }
  int TrigDevice() { return daqio_->TrigDevice(); }
  int TrigChannel() { return daqio_->TrigChannel(); }
  int TrigMode() { return daqio_->TrigMode(); }
  int TrigSrc() { return daqio_->TrigSrc(); }
  int TrigSlope() { return daqio_->TrigSlope(); }
  double TrigLevel() { return daqio_->TrigLevel(); }
  double TrigPosition() { return daqio_->TrigPosition(); }
  const std::vector<double>& Offset() const { return daqio_->Offset(); }
  const std::vector<double>& Range() const { return daqio_->Range(); }
  double SampleFrequency() { return daqio_->SampleFrequency(); }
  double TimeWindow() { return daqio_->TimeWindow(); }

  void setStartReading(bool v) { singleton_self()->startReading_ = v; }
  void setTrigDevice(int v) { singleton_self()->trigDevice_ = v; singleton_self()->triggerChanged_ = true; }
  void setTrigChannel(int v) { singleton_self()->trigChannel_ = v; singleton_self()->triggerChanged_ = true; }
  void setTrigMode(int v) { singleton_self()->trigMode_ = v; singleton_self()->triggerChanged_ = true; }
  void setTrigLevel(double v) { singleton_self()->trigLevel_ = v; singleton_self()->triggerChanged_ = true; }
  void setTrigPosition(double v) { singleton_self()->trigPosition_ = v; singleton_self()->triggerChanged_ = true; }
  void setADCOffset(int index, double v) {
    if (index<static_cast<int>((singleton_self()->adcOffsetList_).size())) singleton_self()->adcOffsetList_[index] = v;
    singleton_self()->analogInSettingChanged_ = true;
  }

private:
  std::string ADManagerName_ = "";
  AnalogDiscoveryManager* ADManager_ = nullptr;
  std::shared_ptr<DAQIO> daqio_ = nullptr;
  bool triggerChanged_ = false;
  bool analogInSettingChanged_ = false;
  int trigDevice_ = 0;
  int trigChannel_ = 0;
  int trigMode_ = 2;
  double trigLevel_ = 0.0; // volt
  double trigPosition_ = 0.0; // us
  double timeWindow_ = 10.0; // us
  double sampleFreq_ = 2.0; //MHz
  std::vector<double> adcRangeList_; // volt
  std::vector<double> adcOffsetList_;

  std::vector<int16_t> eventHeader_;
  std::vector<std::vector<int16_t>> eventData_;
  std::vector<int16_t> recentEventHeader_;
  std::vector<std::vector<int16_t>> recentEventData_;
  uint32_t recentEventID_ = 0;
  std::string outputFilenameBase_ = "";
  int numEventsPerFile_ = 100;
  std::shared_ptr<std::ofstream> ofs_;
  int fileID_ = 0;
  bool ondemand_ = false;
  SendTelemetry* sendTelemetry_;
  bool startReading_ = false;
  uint32_t eventID_ = 0;
  int nonDetectionCounter_ = 0;
  int maxNonDetectionCount_ = 5;
  RunIDManager* runIDManager_ = nullptr;
  int chatter_ = 0;
};

} /* namespace gramsballoon */
#endif /* GB_DEMO_MODE */
#endif /* ReadWaveform_H */
