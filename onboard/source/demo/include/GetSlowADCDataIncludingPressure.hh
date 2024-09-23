#ifndef GRAMSBalloon_GetSlowADCDataIncludingPressure_hh
#define GRAMSBalloon_GetSlowADCDataIncludingPressure_hh 1
#include "GetPressure.hh"
#include "GetSlowADCDataDemo.hh"
namespace gramsballoon::pgrams {
class GetSlowADCDataIncludingPressure: public GetSlowADCData {
  DEFINE_ANL_MODULE(GetSlowADCDataIncludingPressure, 1.0);
  ENABLE_PARALLEL_RUN();

public:
  GetSlowADCDataIncludingPressure() = default;
  virtual ~GetSlowADCDataIncludingPressure() = default;

protected:
  GetSlowADCDataIncludingPressure(const GetSlowADCDataIncludingPressure &r) = default;

public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;

private:
  std::string getPressureName_ = "GetPressure";
  GetPressure *getPressure_ = nullptr;
  int channel_ = 2;
};
} // namespace gramsballoon::pgrams
#endif //GRAMSBalloon_GetSlowADCDataIncludingPressure_hh