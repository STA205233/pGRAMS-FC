#ifndef GB_DEMO_MODE
#ifndef SlowADCIO_H
#define SlowADCIO_H 1

#include <iostream>
#include <vector>
#include <pigpiod_if2.h>
#include "SPIInterface.hh"

/**
 * A class to handle ADC128S102
 *
 * @author Tsubasa Tamba, Shota Arai
 * @date 2023-05-01
 */
#define SLOW_ADC_CHANNEL_MAX 8

namespace gramsballoon {

class SlowADCIO
{
public:
  SlowADCIO();
  ~SlowADCIO() = default;

  void setInterface(SPIInterface* intf);
  bool getData(int channel, uint16_t& adc, double& voltage, int num_trials);
  int readReg(int channel, std::vector<char>& buf, int num_read=1);
  double conversion(uint16_t adc);

  void setVa(double v) { va_ = v; }
  

private:
  SPIInterface* intf_;
  double va_ = 5.0;
};

} /* namespace gramsballoon */

#endif /* GB_DEMO_MODE */
#endif 


