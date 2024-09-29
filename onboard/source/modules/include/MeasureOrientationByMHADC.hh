#ifndef GRAMSBalloon_MeasureOrientationByMHADC_hh
#define GRAMSBalloon_MeasureOrientationByMHADC_hh 1
#include "EncodedSerialCommunicator.hh"
#include "anlnext/BasicModule.hh"

namespace {
class MeasureOrientationByMHADC: public anlnext::BasicModule {
  DEFINE_ANL_MODULE(MeasureOrientationByMHADC, 1.0);
  ENABLE_PARALLEL_RUN();

public:
  MeasureOrientationByMHADC() = default;
  virtual ~MeasureOrientationByMHADC() = default;

protected:
  MeasureOrientationByMHADC(const MeasureOrientationByMHADC &r) = default;

public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_pre_initialize() override;
  anlnext::ANLStatus mod_begin_run() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_end_run() override;
  anlnext::ANLStatus mod_finalyze() override;

private:
  EncodedSerialCommunicator *esc_ = nullptr;
  
};
} // namespace
#endif //GRAMSBalloon_MeasureOrientationByMHADC_hh