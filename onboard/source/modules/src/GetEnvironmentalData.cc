#ifndef GB_DEMO_MODE
#include <thread>
#include <chrono>
#include "GetEnvironmentalData.hh"
#include "pigpiod_if2.h"
#include "bme68x.h"

using namespace anlnext;

namespace gramsballoon {

GetEnvironmentalData::GetEnvironmentalData()
{
  bme680io_ = std::make_shared<BME680IO>();
  interface_ = std::make_shared<SPIInterface>();
}

GetEnvironmentalData::~GetEnvironmentalData() = default;

ANLStatus GetEnvironmentalData::mod_define()
{
  define_parameter("chip_select", &mod_class::chipSelect_);
  define_parameter("SPI_manager_name", &mod_class::SPIManagerName_);
  define_parameter("chatter", &mod_class::chatter_);

  return AS_OK;
}

ANLStatus GetEnvironmentalData::mod_pre_initialize()
{
  const std::string send_telem_md = "SendTelemetry";
  if (exist_module(send_telem_md)) {
    get_module_NC(send_telem_md, &sendTelemetry_);
  }

  if (chipSelect_<0 || chipSelect_>=27) {
    std::cerr << "Chip select must be non-negative and smaller than 27: CS=" << chipSelect_ << std::endl;
    if (sendTelemetry_) {
      sendTelemetry_->getErrorManager()->setError(ErrorType::OTHER_ERRORS);
    }
  }
  if (exist_module(SPIManagerName_)) {
    get_module_NC(SPIManagerName_, &SPIManager_);
  }
  else {
    std::cerr << "SPI manager does not exist. Module name = " << SPIManagerName_ << std::endl;
    if (sendTelemetry_) {
      sendTelemetry_->getErrorManager()->setError(ErrorType::MODULE_ACCESS_ERROR);
    }
  }
  SPIManager_->addChipSelect(chipSelect_);
  
  return AS_OK;
}

ANLStatus GetEnvironmentalData::mod_initialize()
{
  const unsigned int spihandler = SPIManager_ -> Interface() -> SPIHandler();
  const int pi = SPIManager_ -> Interface() -> GPIOHandler();
  
  interface_ -> setSPIHandler(spihandler);
  interface_ -> setChipSelect(chipSelect_);
  interface_ -> setGPIOHandler(pi);

  bme680io_ -> setup(interface_.get());
  
  return AS_OK;
}

ANLStatus GetEnvironmentalData::mod_analyze()
{
  const int status = bme680io_ -> getData();
  if (status!=0) {
    std::cerr << "Failed to get environmental data." << std::endl;
    setDataAquisitionError();
  }
  if (chatter_>=1) {
    bme680io_ -> printData();
  }
  pressure_ = bme680io_ -> SensorData() -> pressure;
  humidity_ = bme680io_ -> SensorData() -> humidity;
  temperature_ = bme680io_ -> SensorData() -> temperature;

  return AS_OK;
}

ANLStatus GetEnvironmentalData::mod_finalize()
{  
  return AS_OK;
}

void GetEnvironmentalData::setDataAquisitionError()
{
  if (sendTelemetry_==nullptr) return;

  const std::string name = module_id();
  if (name=="GetEnvironmentalData_1") {
    sendTelemetry_->getErrorManager()->setError(ErrorType::ENV_DATA_AQUISITION_ERROR_1);
  }
  else if (name=="GetEnvironmentalData_2") {
    sendTelemetry_->getErrorManager()->setError(ErrorType::ENV_DATA_AQUISITION_ERROR_2);
  }
  else if (name=="GetEnvironmentalData_3") {
    sendTelemetry_->getErrorManager()->setError(ErrorType::ENV_DATA_AQUISITION_ERROR_3);
  }
  else if (name=="GetEnvironmentalData_4") {
    sendTelemetry_->getErrorManager()->setError(ErrorType::ENV_DATA_AQUISITION_ERROR_4);
  }
  else if (name=="GetEnvironmentalData_5") {
    sendTelemetry_->getErrorManager()->setError(ErrorType::ENV_DATA_AQUISITION_ERROR_5);
  }
  else {
    sendTelemetry_->getErrorManager()->setError(ErrorType::OTHER_ERRORS);
  }
}

} /* namespace gramsballoon */
#endif /* GB_DEMO_MODE */
