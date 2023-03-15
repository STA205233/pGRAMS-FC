#include "AnalogDiscoveryManager.hh"

using namespace anlnext;

AnalogDiscoveryManager::AnalogDiscoveryManager()
{
  ADIO_ = std::make_unique<AnalogDiscoveryIO>();
}

AnalogDiscoveryManager::~AnalogDiscoveryManager() = default;

ANLStatus AnalogDiscoveryManager::mod_define()
{
  return AS_OK;
}

ANLStatus AnalogDiscoveryManager::mod_initialize()
{
  const int status = ADIO_->initialize();
  if (status!=0) {
    std::cerr << "Analog Discovery initialize failed." << std::endl;
    std::cerr << "status = " << status << std::endl;
    return AS_QUIT_ERROR;
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

