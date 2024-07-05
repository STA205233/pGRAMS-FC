#ifndef GB_DEMO_MODE
#include "SPIManager.hh"
#include "pigpiod_if2.h"
#include "bme68x.h"

using namespace anlnext;

namespace gramsballoon {

SPIManager::SPIManager()
{
  interface_ = std::make_shared<SPIInterface>();
}

SPIManager::~SPIManager() = default;

ANLStatus SPIManager::mod_define()
{
  define_parameter("channel", &mod_class::channel_);
  define_parameter("baudrate", &mod_class::baudrate_);
  define_parameter("spi_flags", &mod_class::spiFlags_);
  define_parameter("chatter", &mod_class::chatter_);

  return AS_OK;
}

ANLStatus SPIManager::mod_initialize()
{
  const std::string send_telem_md = "SendTelemetry";
  if (exist_module(send_telem_md)) {
    get_module_NC(send_telem_md, &sendTelemetry_);
  }

  int pi = pigpio_start(NULL, portNumber_.c_str());
  if (pi<0) {
    std::cerr << "pigpio daemon connection failed: pi =  " << pi << std::endl;
    if (sendTelemetry_) {
      sendTelemetry_->getErrorManager()->setError(ErrorType::PIGPIO_START_ERROR);
    }
  }

  unsigned int spi_handler = spi_open(pi, channel_, baudrate_, spiFlags_);
  if (static_cast<int>(spi_handler)<0) {
    std::cerr << "spi open failed: spi_handler = " << spi_handler << std::endl;
    if (sendTelemetry_) {
      sendTelemetry_->getErrorManager()->setError(ErrorType::SPI_OPEN_ERROR);
    }
  }

  interface_ -> setGPIOHandler(pi);
  interface_ -> setChipSelect(8);
  interface_ -> setSPIHandler(spi_handler);
  if (chatter_>=1) {
    std::cout << "spi_handler: " << spi_handler << std::endl;
  }

  const int n = chipSelectArray_.size();
  for (int i=0; i<n; i++) {
    set_mode(pi, chipSelectArray_[i], PI_OUTPUT);
    set_pull_up_down(pi, chipSelectArray_[i], PI_PUD_UP);
    gpio_write(pi, chipSelectArray_[i], PI_HIGH);
  }
   
  return AS_OK;
}

ANLStatus SPIManager::mod_analyze()
{
  return AS_OK;
}

ANLStatus SPIManager::mod_finalize()
{
  spi_close(interface_->GPIOHandler(), interface_->SPIHandler());
  pigpio_stop(interface_->GPIOHandler());
  
  return AS_OK;
}

void SPIManager::addChipSelect(int v)
{
  (singleton_self()->chipSelectArray_).push_back(v);
}

} /* namespace gramsballoon */
#endif /* GB_DEMO_MODE */
