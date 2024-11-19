#ifndef InterpretTelemetry_H
#define InterpretTelemetry_H 1

#include <anlnext/BasicModule.hh>
#include <thread>
#include <chrono>
#include "TelemetryDefinition.hh"
#include "ReceiveTelemetry.hh"
#include "ErrorManager.hh"
#ifdef USE_ROOT
#include "PlotWaveform.hh"
#endif // USE_ROOT
#ifdef USE_HSQUICKLOOK
#include "PushToMongoDB.hh"
#endif // USE_HSQUICKLOOK
namespace gramsballoon {

class ReceiveTelemetry;
class PlotWaveform;
#ifdef USE_HSQUICKLOOK
class PushToMongoDB;
#endif // USE_HSQUICKLOOK

class InterpretTelemetry : public anlnext::BasicModule {
  DEFINE_ANL_MODULE(InterpretTelemetry, 1.0);

public:
  InterpretTelemetry();
  virtual ~InterpretTelemetry();

protected:
  InterpretTelemetry(const InterpretTelemetry &r) = default;
  
public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_finalize() override;

  void writeTelemetryToFile(bool failed);
  void updateRunIDFile();

  TelemetryDefinition* Telemdef() { return telemdef_.get(); }
  std::shared_ptr<const ErrorManager> getErrorManager() const { return errorManager_; }
  int CurrentTelemetryType() { return currentTelemetryType_; }
  
private:
  std::shared_ptr<TelemetryDefinition> telemdef_;
  ReceiveTelemetry* receiver_ = nullptr;
  std::shared_ptr<ErrorManager> errorManager_ = nullptr;
#ifdef USE_ROOT
  PlotWaveform* plotter_ = nullptr;
#endif // USE_ROOT
#ifdef USE_HSQUICKLOOK
  PushToMongoDB* pusher_ = nullptr;
#endif // USE_HSQUICKLOOK
  int currentTelemetryType_ = 0;
  std::map<int, std::pair<int, int>> fileIDmp_;
  bool saveTelemetry_ = true;
  std::string binaryFilenameBase_ = "";
  int numTelemPerFile_ = 100;
  std::string timeStampStr_ = "";
  int chatter_ = 0;
  std::string runIDFilename_;
  int currentRunID_ = -1;
  std::string receiverModuleName_;
};

} // namespace gramsballoon
#endif // InterpretTelemetry_H
