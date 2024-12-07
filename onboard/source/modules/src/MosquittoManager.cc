#ifndef GB_MosquittoManager_hh
#define GB_MosquittoManager_hh 1
#include "anlnext/BasicModule.hh"

namespace gramsballoon::pgrams {
class MosquittoManager : public anlnext::BasicModule {
  DEFINE_ANL_MODULE(MosquittoManager, 1.0);
  ENABLE_PARALLEL_RUN();
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
  
};
} // namespace gramsballoon::pgrams
#endif // GB_MosquittoManager_hh