#ifndef GB_DEMO_MODE
#ifndef MAX31865IO_H
#define MAX31865IO_H 1


#include <pigpiod_if2.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include "SPIInterface.hh"

/**
 * A class to handle MAX31865 to measure temperature
 *
 * @author Tsubasa Tamba, Shota Arai
 * @date 2023-03-17
 */



#include <pigpiod_if2.h>
#include <iostream>
#include <vector>

#define MAX31865_CONF_READ_REG 0x00
#define MAX31865_CONF_WRITE_REG 0x80

#define MAX31865_CONF_BIAS_ON 0b10000000
#define MAX31865_CONF_BIAS_OFF 0
#define MAX31865_CONF_BIAS_MSK 0b10000000

#define MAX31865_CONF_CONVERSION_AUTO 0b01000000
#define MAX31865_CONF_CONVERSION_OFF 0b00000000
#define MAX31865_CONF_CONVERSION_MSK 0b01000000

#define MAX31865_CONF_3WIRE 0b00010000
#define MAX31865_CONF_24WIRE 0
#define MAX31865_CONF_WIRE_MSK 0b00010000

#define MAX31865_CONF_AUTO_FD 0b00000100
#define MAX31865_CONF_MANUAL_FD_START 0b00001000
#define MAX31865_CONF_MANUAL_FD_STOP 0b000001100
#define MAX31865_CONF_MANUAL_FD_DISABLE 0b00000000
#define MAX31865_CONF_FD_CYCLE_MSK 0b00001110
#define MAX31865_CONF_F_STATUS_CLR_MSK 0b00000010

#define MAX31865_CONF_60HZ 0
#define MAX31865_CONF_50HZ 1
#define MAX31865_CONF_FILTER_SEL_MSK 0b1

#define MAX31865_CONF_1SHOT_MSK 0b00100000

#define MAX31865_DATA_REG_MSB 0x01
#define MAX31865_DATA_REG_LSB 0x02
#define MAX31865_DATA_FAULT_MSK 1

// If data is written, MSB bit of the address must be set to 1, otherwise set to 0.
#define MAX31865_WRITE_MSK 0b10000000

#define MAX31865_FAULT_STATUS_REG 0x07

#define MAX31865_OK 0
#define MAX31865_BAD -1
#define MAX31865_WARNING 10

namespace gramsballoon {

class MAX31865IO
{
public:
  MAX31865IO();
  ~MAX31865IO() = default;

  void setInterface(SPIInterface* intf);
  int readReg(uint8_t reg, char *value, unsigned int length);
  int writeReg(uint8_t reg, char *value, unsigned int length);
  int setConfigure(char bits);
  int setConfigureSingle(char val, char mask);
  int updateConfigure();
  char getConfigure() { return currentBits_; }

  int16_t getData();
  int faultStatusClear();
  int getFaultStatus();
  double Tconversion(int adc);  

  int setBias(char val);
  int setConversion(char val);
  int setWires(char val);
  int setFaultDetection(char val);
  int setFilter(char val);
  char getBias();
  char getConversion();
  char getWires();
  char getFaultDetection();
  char getFilter();

  double Temperature() { return temperature_; }
  int16_t TemperatureADC() { return temperatureADC_; }

private:
  int pi;
  const int rRef_ = 430;
  unsigned int handle;
  int cs;
  char currentBits_;
  SPIInterface* intf_;
  int16_t temperatureADC_ = 0;
  double temperature_ = 0.0;
};

} /* namespace gramsballoon */

#endif /* GB_DEMO_MODE */
#endif 


