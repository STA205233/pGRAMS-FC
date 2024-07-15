#ifndef GB_ReadWaveformDemo_hh
#define GB_ReadWaveformDemo_hh 1
#include "GBBasicDemoModule.hh"
#include "RunIDManager.hh"
#include "SendTelemetry.hh"
#include "TelemetryDefinition.hh"
#include "anlnext/BasicModule.hh"
namespace gramsballoon {
class ReadWaveform: public GBBasicDemoModule {
  DEFINE_ANL_MODULE(ReadWaveform, 1.0);
  ENABLE_PARALLEL_RUN();

public:
  ReadWaveform() = default;
  virtual ~ReadWaveform() = default;

protected:
  ReadWaveform(const ReadWaveform &r) = default;

public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_finalize() override;

  void createNewOutputFile();
  void closeOutputFile();
  void writeData();

  const std::vector<std::vector<int16_t>> &EventData() const { return singleton_self()->eventData_; }
  void setOndemand(bool v) { singleton_self()->ondemand_ = v; }
  bool getOndemand() { return singleton_self()->ondemand_; }

  bool StartReading() const { return singleton_self()->startReading_; }
  uint32_t EventID() const { return singleton_self()->eventID_; }
  uint32_t EventCount() const { return singleton_self()->eventCount_; }
  int TrigDevice() const { return singleton_self()->trigDevice_; }
  int TrigChannel() const { return singleton_self()->trigChannel_; }
  int TrigMode() const { return singleton_self()->trigMode_; }
  int TrigSrc() const { return singleton_self()->trigSrc_; }
  int TrigSlope() const { return singleton_self()->trigSlope_; }
  double TrigLevel() const { return singleton_self()->trigLevel_; }
  double TrigPosition() const { return singleton_self()->trigPosition_; }
  const std::vector<double> &Offset() const { return singleton_self()->offset_; }
  const std::vector<double> &Range() const { return singleton_self()->range_; }
  double SampleFrequency() const { return singleton_self()->sampleFrequency_; }
  double TimeWindow() const { return singleton_self()->timeWindow_; }

  void setStartReading(bool v) { singleton_self()->startReading_ = v; }
  void setTrigDevice(int v) {
    singleton_self()->trigDevice_ = v;
    singleton_self()->triggerChanged_ = true;
  }
  void setTrigChannel(int v) {
    singleton_self()->trigChannel_ = v;
    singleton_self()->triggerChanged_ = true;
  }
  void setTrigMode(int v) {
    singleton_self()->trigMode_ = v;
    singleton_self()->triggerChanged_ = true;
  }
  void setTrigLevel(double v) {
    singleton_self()->trigLevel_ = v;
    singleton_self()->triggerChanged_ = true;
  }
  void setTrigPosition(double v) {
    singleton_self()->trigPosition_ = v;
    singleton_self()->triggerChanged_ = true;
  }
  void setADCOffset(int index, double v) {
    if (index < static_cast<int>((singleton_self()->adcOffsetList_).size())) singleton_self()->adcOffsetList_[index] = v;
    singleton_self()->analogInSettingChanged_ = true;
  }

private:
  bool triggerChanged_ = false;
  bool analogInSettingChanged_ = false;
  int trigDevice_ = 0;
  int trigChannel_ = 0;
  int trigMode_ = 2;
  double trigLevel_ = 0.0; // volt
  double trigPosition_ = 0.0; // us
  double timeWindow_ = 10.0; // us
  double sampleFreq_ = 2.0; //MHz
  std::vector<double> adcRangeList_ = {1.0, 1.0, 1.0, 1.0}; // volt
  std::vector<double> adcOffsetList_ = {0., 0., 0., 0.};

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
  SendTelemetry *sendTelemetry_;
  bool startReading_ = false;
  uint32_t eventID_ = 0;
  int nonDetectionCounter_ = 0;
  int maxNonDetectionCount_ = 5;
  RunIDManager *runIDManager_ = nullptr;
  int chatter_ = 0;
  uint32_t eventCount_ = 0;
  int trigSrc_ = 0;
  int trigSlope_ = 0;
  std::vector<double> offset_ = {0., 0., 0., 0.};
  std::vector<double> range_ = {5.0, 5.0, 5.0, 5.0};
  double sampleFrequency_ = 2.0;
  std::string ADManagerName_ = "";
  void GenerateFileHeader(std::vector<int16_t> &header, int16_t num_event);
  void GenerateFileFooter(std::vector<int16_t> &footer);
  void GenerateEventHeader();
  void GenerateEventFooter();
  void GenerateFakeEvent();
  double nonDetectionRate_ = 0.0;
};
} // namespace gramsballoon

#endif //GB_ReadWaveformDemo_hh