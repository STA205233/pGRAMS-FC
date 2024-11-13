#ifndef GRAMSBalloon_Sleep_hh
#define GRAMSBalloon_Sleep_hh 1
#include "anlnext/BasicModule.hh"

namespace gramsballoon::pgrams {
class Sleep: public anlnext::BasicModule {
  DEFINE_ANL_MODULE(Sleep, 1.0);
  ENABLE_PARALLEL_RUN();

public:
  Sleep() = default;
  virtual ~Sleep() = default;

protected:
  Sleep(const Sleep &r) = default;

public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_analyze() override;
  
  
  
  

private:
  int sleep_sec = 0;
};
} // namespace gramsballoon::pgrams
#endif //GRAMSBalloon_Sleep_hh