/**
 * Regulate high voltage via Analog Discovery.
 *
 * @author Tsubasa Tamba, Shota Arai
 * @date 2023-03-25
 */

#ifndef SendTelemetry_H
#define SendTelemetry_H 1

#include "ControlHighVoltage.hh"
#include "ErrorManager.hh"
#include "GetCompressorData.hh"
#include "GetEnvironmentalData.hh"
#include "GetPressure.hh"
#include "GetRaspiStatus.hh"
#include "GetSlowADCData.hh"
#include "MeasureAcceleration.hh"
#include "MeasureTemperatureWithRTDSensor.hh"
#include "ReadWaveform.hh"
#include "ReceiveCommand.hh"
#include "RunIDManager.hh"
#include "SerialCommunication.hh"
#include "TelemetryDefinition.hh"
#include <anlnext/BasicModule.hh>
#include <thread>

namespace gramsballoon {

class ReadWaveform;
class MeasureTemperatureWithRTDSensor;
class ControlHighVoltage;
class GetRaspiStatus;
class GetEnvironmentalData;
class MeasureAcceleration;
class GetSlowADCData;
class ReceiveCommand;
class RunIDManager;

class SendTelemetry: public anlnext::BasicModule {
  DEFINE_ANL_MODULE(SendTelemetry, 2.0);
  ENABLE_PARALLEL_RUN();

public:
  SendTelemetry();
  virtual ~SendTelemetry();

protected:
  SendTelemetry(const SendTelemetry &r) = default;

public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_finalize() override;

  void inputInfo();
  void inputDetectorInfo();
  void inputHKVesselInfo();
  void inputSoftwareInfo();
  void inputStatusInfo();
  void writeTelemetryToFile(bool failed);

  void setTelemetryType(int v) { singleton_self()->telemetryType_ = v; }
  int TelemetryType() { return singleton_self()->telemetryType_; }

  void setEventID(int v) { telemdef_->setEventID(v); };
  void setEventHeader(const std::vector<int16_t> &v) { telemdef_->setEventHeader(v); }
  void setEventData(const std::vector<std::vector<int16_t>> &v) { telemdef_->setEventData(v); }
  int EventID() { return telemdef_->EventID(); }
  const std::vector<int16_t> &EventHeader() const { return telemdef_->EventHeader(); }
  const std::vector<std::vector<int16_t>> &EventData() const { return telemdef_->EventData(); }
  ErrorManager *getErrorManager() { return (singleton_self()->errorManager_).get(); }
  int WfDivisionCounter() { return singleton_self()->wfDivisionCounter_; }

private:
  std::shared_ptr<TelemetryDefinition> telemdef_ = nullptr;
  int telemetryType_ = 1;
  std::shared_ptr<ErrorManager> errorManager_ = nullptr;
  std::map<int, std::pair<int, int>> fileIDmp_;
  bool saveTelemetry_ = true;
  std::string binaryFilenameBase_ = "";
  int numTelemPerFile_ = 1000;
  static constexpr int sleepms_ = 500;
  int wfDivisionCounter_ = 0;
  int chatter_ = 0;

  // access to other classes
  ReadWaveform *readWaveform_ = nullptr;
  std::vector<std::string> measureTemperatureModuleNames_;
  std::vector<MeasureTemperatureWithRTDSensor *> measureTemperatureVec_;
  std::string TPCHVControllerModuleName_ = "";
  ControlHighVoltage *TPCHVController_ = nullptr;
  std::string PMTHVControllerModuleName_ = "";
  ControlHighVoltage *PMTHVController_ = nullptr;
  GetRaspiStatus *getRaspiStatus_ = nullptr;
  std::vector<std::string> getEnvironmentalDataModuleNames_;
  std::vector<GetEnvironmentalData *> getEnvironmentalDataVec_;
  pgrams::GetCompressorData *getCompressorData_ = nullptr;
  std::string getPressureChamberModuleName_;
  std::string getPressureJacketModuleName_;
  pgrams::GetPressure *getPressureChamber_;
  pgrams::GetPressure *getPressureJacket_;
  MeasureAcceleration *measureAcceleration_ = nullptr;
  GetSlowADCData *getSlowADCData_ = nullptr;
  ReceiveCommand *receiveCommand_ = nullptr;
  RunIDManager *runIDManager_ = nullptr;

  // communication
  std::shared_ptr<SerialCommunication> sc_;
  std::string serialPath_;
  speed_t baudrate_ = B9600;
  mode_t openMode_ = O_RDWR;
};

} /* namespace gramsballoon */

#endif /* SendTelemetry_H */
