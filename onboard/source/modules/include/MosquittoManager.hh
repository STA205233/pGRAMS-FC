#ifndef GB_MosquittoManager_hh
#define GB_MosquittoManager_hh 1
#include "MosquittoIO.hh"
#include "anlnext/BasicModule.hh"

namespace gramsballoon::pgrams {
class MosquittoManager: public anlnext::BasicModule {
  DEFINE_ANL_MODULE(MosquittoManager, 1.0);
  // TODO: For now, we will not enable parallel run on this module.

public:
  MosquittoManager() = default;
  virtual ~MosquittoManager() = default;

protected:
  MosquittoManager(const MosquittoManager &r) = default;

public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_pre_initialize() override;
  anlnext::ANLStatus mod_begin_run() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_end_run() override;
  anlnext::ANLStatus mod_finalize() override;

private:
  std::shared_ptr<MosquittoIO> mosquittoIO_ = nullptr;
  std::string host_ = "localhost";
  int port_ = 1883;
  std::string topicPub_ = "command";
  std::string topicSub_ = "telemetry";
  int qos_ = 0;
  int chatter_ = 0;
};
} // namespace gramsballoon::pgrams
#endif // GB_MosquittoManager_hh