#ifndef GB_DEMO_MODE
#include "MAX31865IO.hh"
#include <vector>
#include <iomanip>
#include <bitset>

namespace gramsballoon {

MAX31865IO::MAX31865IO()
{
}

void MAX31865IO::setInterface(SPIInterface* intf)
{
  intf_ = intf;
}

int MAX31865IO::readReg(uint8_t reg, char *value, unsigned int length)
{
  const int pi = intf_->GPIOHandler();
  const int cs = intf_->ChipSelect();
  const unsigned int spihandler = intf_->SPIHandler();
  gpio_write(pi, cs, PI_LOW);
  reg &= ~MAX31865_WRITE_MSK;
  int rslt = MAX31865_OK;
  const int rslt_write = spi_write(pi, spihandler, reinterpret_cast<char *>(&reg), 1);
  if (rslt_write != 1) {
    if (rslt_write<0) {
      std::cerr << "read_reg failed in writing address (error code: " << pigpio_error(rslt_write) << ")" << std::endl;
    }
    else {
      std::cerr << "read_reg failed in writing data: rslt_write = " << rslt_write << std::endl;
    }
    rslt = rslt_write;
    gpio_write(pi, cs, PI_HIGH);
    return rslt;
  } 
  const int rslt_read = spi_read(pi, spihandler, value, length);

  if (rslt_read != static_cast<int>(length)) {
    if (rslt_read<0) {
      std::cerr << "read_reg failed in reading data (error code: " << pigpio_error(rslt_read) << ")" << std::endl;
    }
    else {
      std::cerr << "read_reg failed in writing data: rslt_read = " << rslt_read << std::endl;
    }
    rslt = rslt_read;
    gpio_write(pi, cs, PI_HIGH);
    return rslt;
  }
    
  gpio_write(pi, cs, PI_HIGH);
  return rslt_read;
}

int MAX31865IO::writeReg(uint8_t reg, char *value, unsigned int length)
{
  const int pi = intf_->GPIOHandler();
  const int cs = intf_->ChipSelect();
  const unsigned int spihandler = intf_->SPIHandler();

  gpio_write(pi, cs, PI_LOW);
  reg |= MAX31865_WRITE_MSK;
  int rslt = MAX31865_OK;
  int rslt_write = spi_write(pi, spihandler, reinterpret_cast<char *>(&reg), 1);
  if (rslt_write != 1) {
    if (rslt_write<0) {
      std::cerr << "write_reg failed in writing address (error code: " << pigpio_error(rslt_write) << ")" << std::endl;
    }
    else {
      std::cerr << "write_reg failed in writing data: rslt_write = " << rslt_write << std::endl;
    }
    rslt = rslt_write;
    gpio_write(pi, cs, PI_HIGH);
    return rslt;
  }
  
  rslt_write = spi_write(pi, spihandler, value, length);
  if (rslt_write != static_cast<int>(length)) {
    if (rslt_write < 0) {
      std::cerr << "write_reg failed in writing data (error code: " << pigpio_error(rslt_write) << ")" << std::endl;
    }
    else {
      std::cerr << "write_reg failed in writing data: rslt_write = " << rslt_write << std::endl;
    }
    rslt = rslt_write;
    gpio_write(pi, cs, PI_HIGH);
    return rslt;
  }
    
  gpio_write(pi, cs, PI_HIGH);
  return rslt_write;
}

int MAX31865IO::setConfigure(char bits)
{
  int _bias = getBias();
    
  if ((bits & MAX31865_CONF_CONVERSION_MSK) == MAX31865_CONF_CONVERSION_AUTO) {
    bits = (bits & (~MAX31865_CONF_FILTER_SEL_MSK)) | (getFilter() & MAX31865_CONF_FILTER_SEL_MSK);
  }
  int dbg = writeReg(MAX31865_CONF_WRITE_REG, &bits, 1);
  if (dbg != 1) {
    std::cerr << "set_configure failed due to write_reg failure. write_reg_bytes: " << dbg << std::endl;
  }
  
  if ((_bias != getBias()) && (_bias == MAX31865_CONF_BIAS_OFF)) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
  
  return MAX31865_OK;
}

int MAX31865IO::setConfigureSingle(char val, char mask)
{
  char bits = (currentBits_ & ~mask) | (val & mask);
  int status = setConfigure(bits);
  if ((currentBits_ & mask) != (val & mask)) {
      return MAX31865_BAD;
  }
  return status;
}

int MAX31865IO::updateConfigure()
{
  int rslt = readReg(MAX31865_CONF_READ_REG, &currentBits_, 1);
  if (rslt != 1) {
    std::cerr << "get_configure failed to read. read bytes: " << rslt << std::endl;
    return MAX31865_BAD;
  }
  return MAX31865_OK;
}

int16_t MAX31865IO::getData()
{
  int16_t data = 0;
  char temp[2];
  readReg(MAX31865_DATA_REG_MSB, &temp[0], 1);
  readReg(MAX31865_DATA_REG_LSB, &temp[1], 1);

  if ((temp[1] & MAX31865_DATA_FAULT_MSK) == 0) {
    data |= temp[0] << 7;
    data |= temp[1] >> 1;
  }
  else {
    data = 0;
    std::cout << "MAX31865IO::getData() deteted fault." << std::endl;
    getFaultStatus();
    faultStatusClear();
    return MAX31865_BAD;
  }
  temperatureADC_ = data;
  temperature_ = Tconversion(data);
  return MAX31865_OK;
}

int MAX31865IO::faultStatusClear()
{
  if (updateConfigure() < 0) {
    std::cerr << "fault_status_clear may change configure due to get_configure failure." << std::endl;
    return MAX31865_WARNING;
  }
  char bits = getConfigure();
  bits &= ~MAX31865_CONF_1SHOT_MSK;
  bits &= ~MAX31865_CONF_FD_CYCLE_MSK;
  bits |= MAX31865_CONF_F_STATUS_CLR_MSK;

  int rslt_write = writeReg(MAX31865_CONF_WRITE_REG, &bits, 1);
  updateConfigure();
  std::this_thread::sleep_for(std::chrono::microseconds(100));
  if (rslt_write != 1) {
    std::cerr << "fault_status_clear may not be executed. rslt_write = " << rslt_write << std::endl;
    return MAX31865_BAD;
  }
  return MAX31865_OK;
}

int MAX31865IO::getFaultStatus()
{
  char bits = 0b0;
  int rslt_read = readReg(MAX31865_FAULT_STATUS_REG, &bits, 1);
  if (rslt_read != 1) {
    std::cerr << "get_fault_status failed due to read_reg failure. rslt_read = " << rslt_read << std::endl;
    return MAX31865_BAD;
  }
  std::cout << "MAX31865 Fault status : " << std::bitset<8>(bits) << std::endl;
  return MAX31865_OK;
}

double MAX31865IO::Tconversion(int adc)
{
  double res = (static_cast<double>(adc) * static_cast<double>(rRef_) / 400.0) / 32.0 - 256.0;
  return res;
}

int MAX31865IO::setBias(char val)
{
  return setConfigureSingle(val, MAX31865_CONF_BIAS_MSK);
}

int MAX31865IO::setConversion(char val)
{
  return setConfigureSingle(val, MAX31865_CONF_CONVERSION_MSK);
}

int MAX31865IO::setWires(char val)
{
  return setConfigureSingle(val, MAX31865_CONF_WIRE_MSK);
}

int MAX31865IO::setFaultDetection(char val)
{
  return setConfigureSingle(val, (MAX31865_CONF_FD_CYCLE_MSK | MAX31865_CONF_F_STATUS_CLR_MSK));
}

int MAX31865IO::setFilter(char val)
{
  return setConfigureSingle(val, MAX31865_CONF_FILTER_SEL_MSK);
}

char MAX31865IO::getBias()
{
  updateConfigure();
  return (currentBits_ & MAX31865_CONF_BIAS_MSK);
}

char MAX31865IO::getConversion()
{
  updateConfigure();
  return (currentBits_ & MAX31865_CONF_CONVERSION_MSK);
}

char MAX31865IO::getWires()
{
  updateConfigure();
  return (currentBits_ & MAX31865_CONF_WIRE_MSK);
}

char MAX31865IO::getFaultDetection()
{
  updateConfigure();
  return (currentBits_ & (MAX31865_CONF_FD_CYCLE_MSK | MAX31865_CONF_F_STATUS_CLR_MSK));
}

char MAX31865IO::getFilter()
{
  updateConfigure();
  return (currentBits_ & MAX31865_CONF_FILTER_SEL_MSK);
}

} /* namespace gramsballoon */
#endif /* GB_DEMO_MODE */
