#ifndef GRAMSBalloon_GetArduinoData_hh
#define GRAMSBalloon_GetArduinoData_hh 1
#include "anlnext/BasicModule.hh"
#include <array>
#include <regex>
#include <string>
namespace gramsballoon::pgrams {
class GetArduinoData: public anlnext::BasicModule {
  DEFINE_ANL_MODULE(GetArduinoData, 1.0);
  ENABLE_PARALLEL_RUN();

public:
  GetArduinoData() = default;
  virtual ~GetArduinoData() = default;

protected:
  GetArduinoData(const GetArduinoData &r) = default;

private:
  static constexpr int NUM_CH = 32;
  std::array<int, NUM_CH> adcData_;
  std::string filename_ = "/dev/ttyACM0";
  std::regex regex_;

public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_finalize() override;
  std::array<int, NUM_CH> &AdcData() {
    return singleton_self()->adcData_;
  }
};
} // namespace gramsballoon::pgrams
#endif //GRAMSBalloon_GetArduinoData_hh