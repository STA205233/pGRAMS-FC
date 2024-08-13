#ifndef GB_DumpSerial_hh
#define GB_DumpSerial_hh 1
#include "ReceiveTelemetry.hh"
#include "RunIDManager.hh"
#include "anlnext/BasicModule.hh"
#include <string>

namespace gramsballoon {
class DumpSerial: public anlnext::BasicModule {
  DEFINE_ANL_MODULE(DumpSerial, 1.0);
  ENABLE_PARALLEL_RUN();

public:
  DumpSerial() = default;
  virtual ~DumpSerial() = default;

protected:
  DumpSerial(const DumpSerial &r) = default;

public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;

private:
  const ReceiveTelemetry *receiveTelemetry_ = nullptr;
  std::string binaryFilenameBase_ = "Telemetry";
  std::string timeStampStr_ = "";
  int currentFileID_ = 0;
  int chatter_ = 0;
  int numEventsPerFile_ = 1000;
  int eventIndex_ = 0;
  std::string runIDFilename_ = "/Users/grams/settings/run_id/run_id.txt";
  std::string CreateFilename();
};
} // namespace gramsballoon

#endif //GB_DumpSerial_hh