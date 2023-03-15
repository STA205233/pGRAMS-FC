#ifndef AnalogDiscoveryIO_H
#define AnalogDiscoveryIO_H 1

#ifdef WIN32
#include "../../inc/dwf.h"
#elif __APPLE__
#include "/Library/Frameworks/dwf.framework/Headers/dwf.h"
#else
#include <digilent/waveforms/dwf.h>
#endif

#include <vector>

#define ANALOG_OUT_STOP 0
#define ANALOG_OUT_START 1
#define ANALOG_OUT_APPLY 2

/**
 * A class to handle AnalogDiscovery
 *
 * @author Tsubasa Tamba, Shota Arai
 * @date 2023-03-15
 */
class AnalogDiscoveryIO
{
public:
  AnalogDiscoveryIO();
  int initialize();
  void setup(int device_id, int channel, double init_value=0.0);
  void setVoltage(int device_id, int channel, double voltage, double sleep);
  void finalize();

  
private:
  int numDevices_ = 0;
  std::vector<HDWF> handlerList_;
  char szError_[512] = {0};
};



#endif /* AnalogDiscoveryIO_H */
