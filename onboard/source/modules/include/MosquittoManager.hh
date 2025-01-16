#ifndef GB_MosquittoManager_hh
#define GB_MosquittoManager_hh 1
#include "MosquittoIO.hh"
#include "SendTelemetry.hh"
#include "anlnext/BasicModule.hh"
#include <string>

namespace gramsballoon::pgrams {
class MosquittoManager: public anlnext::BasicModule {
  DEFINE_ANL_MODULE(MosquittoManager, 1.0);
  ENABLE_PARALLEL_RUN();

public:
  MosquittoManager() = default;
  virtual ~MosquittoManager() = default;

protected:
  MosquittoManager(const MosquittoManager &r) = default;

public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_pre_initialize() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_begin_run() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_end_run() override;
  anlnext::ANLStatus mod_finalize() override;

  void Publish(std::vector<uint8_t> &message, const std::string &topic, int qos) {
    singleton_self()->mosquittoIO_->Publish(message, topic, qos);
  }
  MosquittoIO<std::vector<uint8_t>> *getMosquittoIO() { return singleton_self()->mosquittoIO_.get(); }

private:
  std::shared_ptr<MosquittoIO<std::vector<uint8_t>>> mosquittoIO_ = nullptr;
  std::string host_ = "localhost";
  std::string user_ = "";
  std::string passwd_ = "";
  SendTelemetry *sendTelemetry_ = nullptr;
  int port_ = 1883;
  int keepAlive_ = 60;
  int chatter_ = 0;
  bool threadedSet_ = true;
  anlnext::ANLStatus HandleError(int error_code);
};
} // namespace gramsballoon::pgrams
#endif // GB_MosquittoManager_hh