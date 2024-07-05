#ifndef GB_DEMO_MODE
#include "AnalogDiscoveryManager.hh"

using namespace anlnext;

namespace gramsballoon {

AnalogDiscoveryManager::AnalogDiscoveryManager()
{
  ADIO_ = std::make_shared<AnalogDiscoveryIO>();
}

AnalogDiscoveryManager::~AnalogDiscoveryManager() = default;

ANLStatus AnalogDiscoveryManager::mod_define()
{
  define_parameter("chatter", &mod_class::chatter_);

  return AS_OK;
}

ANLStatus AnalogDiscoveryManager::mod_initialize()
{
  const std::string send_telem_md = "SendTelemetry";
  if (exist_module(send_telem_md)) {
    get_module_NC(send_telem_md, &sendTelemetry_);
  }

  const int status = ADIO_->initialize();
  if (status!=0) {
    std::cerr << "Analog Discovery initialize failed." << std::endl;
    std::cerr << "status = " << status << std::endl;
    if (sendTelemetry_) {
      sendTelemetry_->getErrorManager()->setError(ErrorType::ANALOG_DISCOVERY_INITIALIZE_ERROR);
    }
  }
  if (ADIO_->NumDevices()==0) {
    std::cerr << "Analog Discovery not connected." << std::endl;
    if (sendTelemetry_) {
      sendTelemetry_->getErrorManager()->setError(ErrorType::ANALOG_DISCOVERY_NOT_CONNECTED);
    }
  }
  if (ADIO_->NumDevices()==1) {
    std::cerr << "Only one analog Discovery is connected." << std::endl;
    if (sendTelemetry_) {
      sendTelemetry_->getErrorManager()->setError(ErrorType::ONLY_ONE_ANALOG_DISCOVERY_CONNECTED);
    }
  }

  return AS_OK;
}

ANLStatus AnalogDiscoveryManager::mod_analyze()
{
  return AS_OK;
}

ANLStatus AnalogDiscoveryManager::mod_finalize()
{
  ADIO_->finalize();

  return AS_OK;
}

} /* namespace gramsballoon */
#endif /* GB_DEMO_MODE */
