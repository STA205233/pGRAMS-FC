/**
 *  Module for communicating with Arduino.
 *
 * @author Shota Arai
 * @date 2024-09-19
 * 
 */

#ifndef GRAMSBalloon_GetArduinoData_hh
#define GRAMSBalloon_GetArduinoData_hh 1
#include "EncodedSerialCommunication.hh"
#include "anlnext/BasicModule.hh"
#include <fstream>
#include <regex>
#include <string>
#include <vector>
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
  static constexpr int BUFFER_SIZE = 500;
  int numCh_ = 32;
  std::vector<int> adcData_;
  std::string filename_ = "/dev/ttyACM0";
  std::shared_ptr<EncodedSerialCommunication> esc_ = nullptr;
  int timeout_ = 1;

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
};
} // namespace gramsballoon::pgrams
#endif //GRAMSBalloon_GetArduinoData_hh