#ifndef GB_DEMO_MODE
#include "SlowADCIO.hh"
#include <vector>
#include <iomanip>
#include <bitset>

namespace gramsballoon {

SlowADCIO::SlowADCIO()
{
}

void SlowADCIO::setInterface(SPIInterface* intf)
{
  intf_ = intf;
}

bool SlowADCIO::getData(int channel, uint16_t& adc, double& voltage, int num_trials)
{
  double sum = 0.0;
  int num = 0;
  for (int i=0; i<=num_trials; i++) {
    int rslt = 0;
    std::vector<char> buf;
    rslt = readReg(channel, buf);
    if (rslt < 0) {
      std::cerr << "Failed to get data in slowADCIO.cc" << std::endl;
      return false;
    }
    if (i==0) continue;
    uint16_t value = static_cast<uint16_t>((static_cast<uint16_t>(buf[0]) << 8) + (static_cast<uint16_t>(buf[1])));
    sum += static_cast<double>(value);
    num++;
  }
  if (num!=0) {
    adc = static_cast<uint16_t>(sum/num);
  }
  voltage = conversion(adc);

  return true;
}

int SlowADCIO::readReg(int channel, std::vector<char>& buf, int num_read/* =2 */)
{
  if (channel<0 || channel>=SLOW_ADC_CHANNEL_MAX) {
    return -1;
  }

  const int byte = num_read*2;
  const int pi = intf_->GPIOHandler();
  const int cs = intf_->ChipSelect();
  const unsigned int spihandler = intf_->SPIHandler();

  std::vector<char> address(byte, static_cast<char>(0));
  buf.resize(num_read*2);
  for (int i=0; i<num_read; i++) {
    address[i*2] = static_cast<char>(channel<<3);
  }
  gpio_write(pi, cs, PI_LOW);
  const int rslt_write = spi_xfer(pi, spihandler, &address[0], &buf[0], byte);
  if (rslt_write != num_read*2) {
    std::cout << "rslt_write = " << rslt_write << std::endl;
    std::cerr << "Error in read_reg. rslt_write: " << pigpio_error(rslt_write) << std::endl;
    return -1;
  }
  gpio_write(pi, cs, PI_HIGH);
  return rslt_write;
}

double SlowADCIO::conversion(uint16_t adc)
{
  const double ret =  va_ / 4096.0 * (adc - 0.5);
  if (ret>0.0) return ret;
  else return 0.0;
}

} /* namespace gramsballoon */
#endif /* GB_DEMO_MODE */
