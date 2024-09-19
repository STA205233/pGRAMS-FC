#ifndef GRAMSBalloon_GetMHADCData_hh
#define GRAMSBalloon_GetMHADCData_hh 1
#include "EncodedSerialCommunication.hh"
#include "anlnext/BasicModule.hh"
#include <regex>

namespace gramsballoon::pgrams {
class GetMHADCData: public anlnext::BasicModule {
  DEFINE_ANL_MODULE(GetMHADCData, 1.0);
  ENABLE_PARALLEL_RUN();

public:
  GetMHADCData() = default;
  virtual ~GetMHADCData() = default;

protected:
  GetMHADCData(const GetMHADCData &r) = default;

public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_finalize() override;
  const std::vector<int> &AdcData() const {
    return adcData_;
  }
  int NumCH() const {
    return numCh_;
  }

private:
  int numCh_ = 32;
  std::vector<int> adcData_;
  std::string filename_ = "/dev/ttyACM0";
  std::shared_ptr<EncodedSerialCommunication> esc_ = nullptr;
  int timeout_ = 1;
  speed_t baudrate_ = B9600;
  mode_t mode_ = O_RDWR | O_NONBLOCK;
};
} // namespace gramsballoon::pgrams
#endif //GRAMSBalloon_GetMHADCData_hh