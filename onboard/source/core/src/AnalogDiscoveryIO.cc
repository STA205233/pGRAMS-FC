#ifndef GB_DEMO_MODE
#include "AnalogDiscoveryIO.hh"
#include <thread>
#include <chrono>

namespace gramsballoon {

AnalogDiscoveryIO::AnalogDiscoveryIO()
{
}

int AnalogDiscoveryIO::initialize()
{
  if (!FDwfEnum(enumfilterAll, &numDevices_)) {
    FDwfGetLastErrorMsg(szError_);
    std::cerr << "FDwfEnum failed: " << szError_ << std::endl;
    return -1;
  }

  std::cout << "number of Analog Discovery: " << numDevices_ << std::endl;

  handlerList_.resize(numDevices_);
  deviceName_.resize(numDevices_, std::vector<char>(32));
  deviceSerialName_.resize(numDevices_, std::vector<char>(32));

  for (int i = 0; i < numDevices_; i++) {
    FDwfEnumDeviceName(i, &deviceName_[i][0]);
    FDwfEnumSN(i, &deviceSerialName_[i][0]);
    if (!FDwfDeviceOpen(i, &handlerList_[i])) {
      FDwfGetLastErrorMsg(szError_);
      std::cerr << "Device open failed: device id = " << i << ",\n"
                << szError_ << std::endl;
      return -1;
    }
  }

  return 0;
}

void AnalogDiscoveryIO::setupAnalogOut(int device_id, int channel, double init_value /*=0.0*/)
{
  if (device_id<0 || device_id>=NumDevices()) {
    std::cerr << "Device ID " << device_id << " not connected"<< std::endl;
    return;
  }
  if (channel<0 || channel>=2) {
    std::cerr << "Channel " << channel << " is inappropriate." << std::endl;
    return;
  }
  FDwfAnalogOutNodeEnableSet(handlerList_[device_id], channel, AnalogOutNodeCarrier, true);
  FDwfAnalogOutNodeFunctionSet(handlerList_[device_id], channel, AnalogOutNodeCarrier, funcDC);
  FDwfAnalogOutNodeOffsetSet(handlerList_[device_id], channel, AnalogOutNodeCarrier, init_value);
  FDwfAnalogOutConfigure(handlerList_[device_id], channel, ANALOG_OUT_START);
}

void AnalogDiscoveryIO::setupAnalogIn(int device_id, int channel, double freq, int buf_size, double range, double offset)
{
  if (device_id<0 || device_id>=NumDevices()) {
    std::cerr << "Device ID " << device_id << " not connected"<< std::endl;
    return;
  }
  if (channel<0 || channel>=2) {
    std::cerr << "Channel " << channel << " is inappropriate." << std::endl;
    return;
  }

  FDwfAnalogInFrequencySet(handlerList_[device_id], freq);
  FDwfAnalogInBufferSizeSet(handlerList_[device_id], buf_size);
  FDwfAnalogInChannelEnableSet(handlerList_[device_id], channel, true);
  FDwfAnalogInChannelRangeSet(handlerList_[device_id], channel, range);
  FDwfAnalogInChannelOffsetSet(handlerList_[device_id], channel, offset);

  const bool reconfigure = false;
  const bool data_aquisition = true;
  FDwfAnalogInConfigure(device_id + 1, reconfigure, data_aquisition);
  # if 0
  double range_now = 15.0;
  FDwfAnalogInChannelRangeGet(device_id + 1, channel, &range_now);
  std::cout << "range: " << range_now << std::endl;
  double rgVoltsStep[32] = {0};
  int _pnSteps;
  FDwfAnalogInChannelRangeSteps(handlerList_[device_id], rgVoltsStep, &_pnSteps);
  std::cout << "RangeSteps: ";
  for (int i = 0; i < _pnSteps; i++)
  {
    std::cout << rgVoltsStep[i] << " ";
  }
  std::cout << std::endl;
  double pvoltsMin, pvoltsMax, pnSteps;
  FDwfAnalogInChannelRangeInfo(1, &pvoltsMin, &pvoltsMax, &pnSteps);
  #endif
  
}
  

void AnalogDiscoveryIO::setVoltage(int device_id, int channel, double voltage, int sleep)
{
  if (device_id<0 || device_id>=NumDevices()) {
    std::cerr << "Device ID " << device_id << " not connected"<< std::endl;
    return;
  }
  if (channel<0 || channel>=2) {
    std::cerr << "Channel " << channel << " is inappropriate." << std::endl;
    return;
  }

  FDwfAnalogOutNodeOffsetSet(handlerList_[device_id], channel, AnalogOutNodeCarrier, voltage);
  FDwfAnalogOutConfigure(handlerList_[device_id], channel, ANALOG_OUT_APPLY);
  std::this_thread::sleep_for(std::chrono::milliseconds(sleep));
}

void AnalogDiscoveryIO::finalize()
{
  FDwfDeviceCloseAll();
}

} /* namespace gramsballoon */
#endif /* GB_DEMO_MODE */
