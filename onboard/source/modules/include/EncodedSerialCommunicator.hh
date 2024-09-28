#ifndef GRAMSBalloon_EncodedSerialCommunicator_HH
#define GRAMSBalloon_EncodedSerialCommunicator_HH 1
#include "EncodedSerialCommunication.hh"
#include "anlnext/BasicModule.hh"

namespace gramsballoon::pgrams {
class EncodedSerialCommunicator: public anlnext::BasicModule {
  DEFINE_ANL_MODULE(EncodedSerialCommunicator, 1.0);
  ENABLE_PARALLEL_RUN();

public:
  EncodedSerialCommunicator() = default;
  virtual ~EncodedSerialCommunicator() = default;

protected:
  EncodedSerialCommunicator(const EncodedSerialCommunicator &r) = default;
  std::shared_ptr<EncodedSerialCommunication> GetEncodedSerialCommunication() { return esc_; }
  int GetTimeout() const { return timeout_; }

public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_finalize() override;
  virtual int SendComAndGetData(const std::string &command, std::string &data, int sleepfor_msec = 500);
  virtual int SendCommand(const std::string &command);

private:
  std::string filename_ = "/dev/ttyACM0";
  std::shared_ptr<EncodedSerialCommunication> esc_ = nullptr;
  int timeout_ = 1;
  int timeoutUsec_ = 0;
  speed_t baudrate_ = B9600;
  mode_t mode_ = O_RDWR | O_NONBLOCK;
};
} // namespace gramsballoon::pgrams
#endif //GRAMSBalloon_EncodedSerialCommunicator_HH