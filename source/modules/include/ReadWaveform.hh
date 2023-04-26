/**
 * Regulate high voltage via Analog Discovery.
 *
 * @author Tsubasa Tamba, Masato Kimura
 * @date 2023-03-22
 */


#ifndef ReadWaveform_H
#define ReadWaveform_H 1

#include<fstream>
#include <anlnext/BasicModule.hh>
#include "AnalogDiscoveryManager.hh"
#include "DAQIO.hh"
#include "SendTelemetry.hh"
#include "TelemetryDefinition.hh"

class SendTelemetry;

namespace gramsballoon {

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

  const std::vector<std::vector<int16_t>>& EventData() const { return eventData_; }
  void setOndemand(bool v) { ondemand_ = v; }
  DAQIO* getDAQIO() { return daqio_.get(); }

  uint32_t EventCount() { return daqio_->EventCount(); }
  int TrigDevice() { return daqio_->TrigDevice(); }
  int TrigChannel() { return daqio_->TrigChannel(); }
  int TrigMode() { return daqio_->TrigMode(); }
  int TrigSrc() { return daqio_->TrigSrc(); }
  int TrigSlope() { return daqio_->TrigSlope(); }
  double TrigLevel() { return daqio_->TrigLevel(); }
  double TrigPosition() { return daqio_->TrigPosition(); }

  void setStartReading(bool v) { startReading_ = v; }
  void setTrigDevice(int v) { trigDevice_ = v; triggerChanged_ = true; }
  void setTrigChannel(int v) { trigChannel_ = v; triggerChanged_ = true; }
  void setTrigMode(int v) { trigMode_ = v; triggerChanged_ = true; }
  void setADCOffset(int index, double v) {
    if (index<static_cast<int>(adcOffsetList_.size())) adcOffsetList_[index] = v;
    analogInSettingChanged_ = true;
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
  std::string outputFilenameBase_ = "";
  int numEventsPerFile_ = 100;
  std::shared_ptr<std::ofstream> ofs_;
  int fileID_ = 0;
  bool ondemand_ = false;
  SendTelemetry* sendTelemetry_;
  bool startReading_ = false;
  uint32_t eventID_ = 0;
};

} /* namespace gramsballoon */

#endif /* ReadWaveform_H */
