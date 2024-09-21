#ifndef GRAMSBalloon_PressureGaugeManager_hh
#define GRAMSBalloon_PressureGaugeManager_hh 1
#include "EncodedSerialCommunicator.hh"

namespace gramsballoon::pgrams {
class PressureGaugeManager: public EncodedSerialCommunicator {
  DEFINE_ANL_MODULE(PressureGaugeManager, 1.0);
  ENABLE_PARALLEL_RUN();

public:
  PressureGaugeManager() = default;
  virtual ~PressureGaugeManager() = default;

protected:
  PressureGaugeManager(const PressureGaugeManager &r) = default;

public:
  int SendComAndGetData(const std::string &command, std::string &data, int sleepfor_msec = 500) override;
};
} // namespace gramsballoon::pgrams

#endif //GRAMSBalloon_PressureGaugeManager_hh