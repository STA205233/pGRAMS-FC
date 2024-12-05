/**
 * Receive commands from ground.
 *
 * @author Tsubasa Tamba, Shota Arai
 * @date 2023-03-01
 * @note 2024-11-28 Shota Arai: Modified for pGRAMS telemetry and command system.
 */


#ifndef ReceiveCommand_H
#define ReceiveCommand_H 1

#include <anlnext/BasicModule.hh>
#include <queue>
#include <sys/select.h>
#include <sys/time.h>
#include "mosquittopp.h"
#include "CommandDefinition.hh"
#include "SerialCommunication.hh"
#include "ShutdownSystem.hh"
#include "SendTelemetry.hh"
#include "ReadWaveform.hh"
#include "ControlHighVoltage.hh"
#include "RunIDManager.hh"

    namespace gramsballoon {

class ShutdownSystem;
class SendTelemetry;
class ReadWaveform;
class ControlHighVoltage;
class RunIDManager;

class ReceiveCommand : public anlnext::BasicModule
{
  DEFINE_ANL_MODULE(ReceiveCommand, 1.0);
  ENABLE_PARALLEL_RUN();

public:
  ReceiveCommand();
  virtual ~ReceiveCommand();
protected:
  ReceiveCommand(const ReceiveCommand& r) = default;

public:  
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_finalize() override;

  bool applyCommand();
  void writeCommandToFile(bool failed);

  uint16_t CommandCode() { return (singleton_self()->comdef_)->Code(); }
  uint32_t CommandIndex() { return singleton_self()->commandIndex_; }
  uint16_t CommandRejectCount() { return singleton_self()->commandRejectCount_; }

private:
  static constexpr int QOS = 1;
  std::vector<uint8_t> buffer_;
  std::vector<uint8_t> command_;
  std::shared_ptr<CommandDefinition> comdef_ = nullptr;
  uint32_t commandIndex_ = 0;
  uint16_t commandRejectCount_ = 0;
  std::map<int, std::pair<int, int>> fileIDmp_;
  bool saveCommand_ = true;
  std::string binaryFilenameBase_ = "";
  int numCommandPerFile_ = 100;
  int chatter_ = 0;

  // access to other classes
  SendTelemetry* sendTelemetry_ = nullptr;
  ShutdownSystem* shutdownSystem_ = nullptr;
  ReadWaveform* readWaveform_ = nullptr;
  ControlHighVoltage* TPCHVController_ = nullptr;
  std::string TPCHVControllerModuleName_ = "";
  ControlHighVoltage* PMTHVController_ = nullptr;
  std::string PMTHVControllerModuleName_ = "";
  RunIDManager* runIDManager_ = nullptr;

  //communication
  std::shared_ptr<mosqpp::mosquittopp> mosq_ = nullptr;
  std::string host_ = "localhost";
  int port_ = 1883;
  std::string passwd_ = "password";
  std::string topic_ = "command";
  int keepAlive_ = 60;
  int timeoutSec_ = 2;
  constexpr static int bufferSize_ = 200;
  constexpr static int serialReadingTimems_ = 250;
};

} /* namespace gramsballoon */

#endif /* ReceiveCommand_H */
