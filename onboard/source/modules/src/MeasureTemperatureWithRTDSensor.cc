#ifndef GB_DEMO_MODE
#include <thread>
#include <chrono>
#include "MeasureTemperatureWithRTDSensor.hh"
#include "pigpiod_if2.h"
#include "bme68x.h"

using namespace anlnext;

namespace gramsballoon {

MeasureTemperatureWithRTDSensor::MeasureTemperatureWithRTDSensor()
{
  max31865io_ = std::make_shared<MAX31865IO>();
  interface_ = std::make_shared<SPIInterface>();
}

MeasureTemperatureWithRTDSensor::~MeasureTemperatureWithRTDSensor() = default;

ANLStatus MeasureTemperatureWithRTDSensor::mod_define()
{
  define_parameter("chip_select", &mod_class::chipSelect_);
  define_parameter("SPI_manager_name", &mod_class::SPIManagerName_);
  define_parameter("chatter", &mod_class::chatter_);

  return AS_OK;
}

ANLStatus MeasureTemperatureWithRTDSensor::mod_pre_initialize()
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

ANLStatus MeasureTemperatureWithRTDSensor::mod_initialize()
{
  const unsigned int spihandler = SPIManager_ -> Interface() -> SPIHandler();
  const int pi = SPIManager_ -> Interface() -> GPIOHandler();

  interface_ -> setSPIHandler(spihandler);
  interface_ -> setChipSelect(chipSelect_);
  interface_ -> setGPIOHandler(pi);

  max31865io_->setInterface(interface_.get());

  max31865io_->faultStatusClear();
  max31865io_->setConfigureSingle(MAX31865_CONF_BIAS_ON, MAX31865_CONF_BIAS_MSK);
  max31865io_->setConfigureSingle(MAX31865_CONF_CONVERSION_AUTO, MAX31865_CONF_CONVERSION_MSK);
  max31865io_->setConfigureSingle(MAX31865_CONF_MANUAL_FD_DISABLE, MAX31865_CONF_FD_CYCLE_MSK);
  max31865io_->setConfigureSingle(MAX31865_CONF_3WIRE, MAX31865_CONF_WIRE_MSK);
  max31865io_->setConfigureSingle(MAX31865_CONF_50HZ, MAX31865_CONF_FILTER_SEL_MSK);

  return AS_OK;
}

ANLStatus MeasureTemperatureWithRTDSensor::mod_analyze()
{
  int status = max31865io_->getData();
  if (status!=MAX31865_OK) {
    std::cerr << "Failed to get data in " << module_id() << "::mod_analyze(), status = " << status << std::endl;
    setDataAquisitionError();
  }
  else if (TemperatureADC()==0) {
    std::cerr << "Error in " << module_id() << "::mod_analyze(), RTD ADC is equal to zero." << std::endl;
    setDataAquisitionError();
  }

  if (chatter_>=1) {
    const double temperature = max31865io_->Temperature();
    std::cout << "temperature ADC: " << TemperatureADC() << std::endl;
    std::cout << " temperature : " << temperature << std::endl;
  }
 
  return AS_OK;
}

ANLStatus MeasureTemperatureWithRTDSensor::mod_finalize()
{
  return AS_OK;
}

void MeasureTemperatureWithRTDSensor::setDataAquisitionError()
{
  if (sendTelemetry_==nullptr) return;

  const std::string name = module_id();
  if (name=="MeasureTemperatureWithRTDSensor_1") {
    sendTelemetry_->getErrorManager()->setError(ErrorType::RTD_DATA_AQUISITION_ERROR_1);
  }
  else if (name=="MeasureTemperatureWithRTDSensor_2") {
    sendTelemetry_->getErrorManager()->setError(ErrorType::RTD_DATA_AQUISITION_ERROR_2);
  }
  else if (name=="MeasureTemperatureWithRTDSensor_3") {
    sendTelemetry_->getErrorManager()->setError(ErrorType::RTD_DATA_AQUISITION_ERROR_3);
  }
  else if (name=="MeasureTemperatureWithRTDSensor_4") {
    sendTelemetry_->getErrorManager()->setError(ErrorType::RTD_DATA_AQUISITION_ERROR_4);
  }
  else if (name=="MeasureTemperatureWithRTDSensor_5") {
    sendTelemetry_->getErrorManager()->setError(ErrorType::RTD_DATA_AQUISITION_ERROR_5);
  }
  else {
    sendTelemetry_->getErrorManager()->setError(ErrorType::OTHER_ERRORS);
  }
}

} /* namespace gramsballoon */
#endif /* GB_DEMO_MODE */
