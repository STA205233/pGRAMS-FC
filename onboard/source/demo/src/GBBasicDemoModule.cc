#include "GBBasicDemoModule.hh"

using namespace anlnext;
namespace gramsballoon {
ANLStatus GBBasicDemoModule::mod_define() {
  define_parameter("random_seed", &mod_class::seed_);
  return AS_OK;
}
ANLStatus GBBasicDemoModule::mod_initialize() {
  engine_ = std::make_shared<std::mt19937>(seed_);
  distribution_ = std::make_shared<std::uniform_real_distribution<>>(min_, max_);
  gaussianDistribution_ = std::make_shared<std::normal_distribution<>>(0.0, 1.0);
  return AS_OK;
}
void GBBasicDemoModule::PrintInfo(const std::string &msg) const {
  std::cout << singleton_self()->module_id() << msg << std::endl;
}
} // namespace gramsballoon