/**
 * Basic module for demo version
 *
 * @author Shota Arai
 * @date 2024-07-02
 */
#ifndef GBBasicDemoModule_hh
#define GBBasicDemoModule_hh 1
#include <anlnext/BasicModule.hh>
#include <iostream>
#include <random>

namespace gramsballoon {
class GBBasicDemoModule: public anlnext::BasicModule {
  DEFINE_ANL_MODULE(GBBasicDemoModule, 1.0);
  ENABLE_PARALLEL_RUN();

public:
  GBBasicDemoModule() = default;
  virtual ~GBBasicDemoModule() = default;
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;

protected:
  GBBasicDemoModule(const GBBasicDemoModule &r) = default;
  double SampleFromUniformDistribution() {
    return (*singleton_self()->distribution_)(*singleton_self()->engine_) - 0.5;
  }
  double SampleFromGaussianDistribution() {
    return (*singleton_self()->gaussianDistribution_)(*singleton_self()->engine_);
  }
  void PrintInfo(const std::string &msg) const;

private:
  std::shared_ptr<std::mt19937> engine_ = nullptr;
  std::shared_ptr<std::uniform_real_distribution<>> distribution_ = nullptr;
  std::shared_ptr<std::normal_distribution<>> gaussianDistribution_ = nullptr;
  const double min_ = 0.0;
  const double max_ = 1.0;
  int seed_ = 0;
};
} // namespace gramsballoon
#endif //GBBasicDemoModule_hh