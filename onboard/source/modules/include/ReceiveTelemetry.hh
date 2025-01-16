#ifndef ReceiveTelemetry_H
#define ReceiveTelemetry_H 1

#include "MosquittoManager.hh"
#include <anlnext/BasicModule.hh>
#include <chrono>
#include <deque>
#include <thread>

namespace gramsballoon {
namespace pgrams {
class MosquittoManager;
} // namespace pgrams

class ReceiveTelemetry: public anlnext::BasicModule {
  DEFINE_ANL_MODULE(ReceiveTelemetry, 1.0);

public:
  ReceiveTelemetry() = default;
  virtual ~ReceiveTelemetry() = default;

protected:
  ReceiveTelemetry(const ReceiveTelemetry &r) = default;

public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_finalize() override;

  const std::vector<uint8_t> &Telemetry() const { return telemetry_; }
  std::vector<uint8_t> &Telemetry() { return telemetry_; }
  bool Valid() { return valid_; }
  void setValid(bool v) { valid_ = v; }
  int Chatter() { return chatter_; }

private:
  std::vector<uint8_t> telemetry_;
  int maxTelemetry_ = 32000;
  bool valid_;

  // communication
  pgrams::MosquittoManager *mosquittoManager_ = nullptr;
  pgrams::MosquittoIO<std::vector<uint8_t>> *mosq_ = nullptr;
  int qos_ = 0;
  std::string subTopic_ = "Telemetry";
  int chatter_ = 0;
};

} // namespace gramsballoon
#endif // ReceiveTelemetry_H
