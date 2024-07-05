/**
 * SPI Manager
 *
 * @author Tsubasa Tamba, Shota Arai
 * @date 2023-03-01
 */


#ifndef SPIManager_H
#define SPIManager_H 1
#ifndef GB_DEMO_MODE

#include <anlnext/BasicModule.hh>
#include "SPIInterface.hh"
#include "SendTelemetry.hh"

namespace gramsballoon {

class SendTelemetry;

class SPIManager : public anlnext::BasicModule
{
  DEFINE_ANL_MODULE(SPIManager, 1.0);
  ENABLE_PARALLEL_RUN();

public:
  SPIManager();
  virtual ~SPIManager();
protected:
  SPIManager(const SPIManager& r) = default;

public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_finalize() override;

  SPIInterface* Interface() { return (singleton_self()->interface_).get(); }
  void addChipSelect(int v);

private:
  std::shared_ptr<SPIInterface> interface_ = nullptr;
  int channel_ = 0;
  int baudrate_ = 100000;
  int spiFlags_ = (1<<5) + (1<<6);
  const std::string portNumber_ = "8888";
  std::vector<int> chipSelectArray_;
  SendTelemetry* sendTelemetry_ = nullptr;
  int chatter_ = 0;
};

} /* namespace gramsballoon */

#endif /* GB_DEMO_MODE */
#endif /* SPIManager_H */
