#ifndef GB_DEMO_MODE
#include "DAQIO.hh"
#include <thread>
#include <chrono>
#include <sys/time.h>

namespace gramsballoon {

DAQIO::DAQIO()
{
  range_.resize(4);
  offset_.resize(4);
}

void DAQIO::setAnalogDiscoveryIO(AnalogDiscoveryIO* io)
{
  ADIO_ = io;
}

void DAQIO::initialize()
{
  const int num_devices = ADIO_ -> NumDevices();
  const std::vector<HDWF>& handler_list = ADIO_ -> HandlerList();
  for (int i=0; i<num_devices; i++) {
    FDwfAnalogInTriggerAutoTimeoutSet(handler_list[i], 0);
  }
}

void DAQIO::setTriggerParameters(int device, int channel, int mode, double level, double position)
{
  trigDevice_ = device;
  trigChannel_ = channel;
  trigMode_ = mode;
  trigSrc_ = mode & 0xf;
  trigSlope_ = (mode & 0xf0) >> 4;
  trigLevel_ = level; // volt
  trigPosition_ = position; //us
}

void DAQIO::setSampleParameters(double freq, double tw)
{
  sampleFreq_ = freq; // MHz
  timeWindow_ = tw; // us
  numSample_ = static_cast<int>(freq*tw);
  if (numSample_>AD2_MAXBIN) {
    numSample_ = AD2_MAXBIN;
  }
}

int DAQIO::setupTrigger()
{
  const int num_devices = ADIO_ -> NumDevices();
  const std::vector<HDWF>& handler_list = ADIO_ -> HandlerList();

  if (trigSrc_==static_cast<int>(TriggerSrc::RANDOM_TRIGGER)) {
    FDwfDeviceTriggerSet(handler_list[trigDevice_], 0, trigsrcAnalogIn);
    for(int i=0; i<num_devices; i++) {
      if(i==trigDevice_) {
        FDwfAnalogInTriggerSourceSet(handler_list[i], trigsrcNone); // fire the trigger from the master device
      }
      else {
        FDwfAnalogInTriggerSourceSet(handler_list[i], trigsrcExternal1); // slave device is triggerd by T1 port
      }
    }
  }
  else if (trigSrc_==static_cast<int>(TriggerSrc::PERIODIC_TRIGGER)) {
    for(int i=0; i<num_devices; i++) {
      FDwfAnalogInTriggerSourceSet(handler_list[i], trigsrcPC);
      FDwfDeviceTriggerSet(handler_list[i], 0, trigsrcPC);
    }
  }
  else if (trigSrc_==static_cast<int>(TriggerSrc::SELF_TRIGGER)) {
    for(int i=0; i<num_devices; i++) {
      if (i==trigDevice_) {
        FDwfAnalogInTriggerSourceSet(handler_list[i], trigsrcDetectorAnalogIn);
        FDwfAnalogInTriggerTypeSet(handler_list[i], trigtypeEdge);
        FDwfDeviceTriggerSet(handler_list[i], 0, trigsrcAnalogIn);
        for (int j=0; j<2; j++) {
          if (j==trigChannel_) {
            FDwfAnalogInTriggerChannelSet(handler_list[i], j);
            FDwfAnalogInTriggerLevelSet(handler_list[i], trigLevel_); // Volt
            if (trigSlope_==static_cast<int>(TriggerSlope::RISE)) {
              FDwfAnalogInTriggerConditionSet(handler_list[i], DwfTriggerSlopeRise);
            }
            else if (trigSlope_==static_cast<int>(TriggerSlope::FALL)) {
              FDwfAnalogInTriggerConditionSet(handler_list[i], DwfTriggerSlopeFall);
            }
            else if (trigSlope_==static_cast<int>(TriggerSlope::EITHER)){
              FDwfAnalogInTriggerConditionSet(handler_list[i], DwfTriggerSlopeEither);
            }
            else {
              std::cerr << "Trigger slope not set appropriately...: trigger slope = " << trigSlope_ << std::endl;
              return -1;
            }
          }
        }
      }
      else {
        FDwfAnalogInTriggerSourceSet(handler_list[i], trigsrcExternal1);
      }
    }
  }
  else if (trigSrc_==static_cast<int>(TriggerSrc::EXTERNAL_TRIGGER)) {
    for(int i=0; i<num_devices; i++){
      FDwfAnalogInTriggerSourceSet(handler_list[i], trigsrcExternal1);
    }
  }
  else {
    std::cerr << "Trigger source not set appropriately...: trigger src = " << trigSrc_ << std::endl;
    return -1;
  }

  for (int i=0; i<num_devices; i++){  
    for(int j=0; j<2; j++){
      const double us = 1.0E-6;
      FDwfAnalogInTriggerPositionSet(handler_list[i], (timeWindow_*0.5-trigPosition_)*us);
    }
  }

  for (int i=0; i<num_devices; i++){  
    const bool reconfigure = false;
    const bool data_aquisition = true;  
    FDwfAnalogInConfigure(handler_list[i], reconfigure, data_aquisition);
  }
	
  std::this_thread::sleep_for(std::chrono::seconds(2));

  return 0;
}

DAQResult DAQIO::getData(int event_id, std::vector<int16_t>& header, std::vector<std::vector<int16_t>>& data)
{
  const int num_devices = ADIO_ -> NumDevices();
  const int num_channels = num_devices * 2;
  const std::vector<HDWF>& handler_list = ADIO_ -> HandlerList();
  STS status = 0;
  int count = 0;
  bool data_acquired = false;
  timeval event_time;

  const int sz_header = 5;
  header.clear();
  data.clear();
  header.resize(sz_header, 0);
  header[0] = static_cast<int16_t>(event_id);
  data.resize(num_channels, std::vector<int16_t>(AD2_MAXBIN));

  int counter = 0;
  while (true) {
    counter++;
    FDwfAnalogInStatus(handler_list[trigDevice_], true, &status);
    std::this_thread::sleep_for(std::chrono::microseconds(100));
    #if 0
    if (status==DwfStateDone) {
      std::cout << "device 0 detected: counter = " << counter << std::endl;
    }
    if (status1==DwfStateDone) {
      std::cout << "device 1 detected: counter = " << counter << std::endl;
    }
    #endif
    if (status==DwfStateDone) {
      data_acquired = true;
      eventCount_++;
      break;
    }
    if (trigSrc_==static_cast<int>(TriggerSrc::PERIODIC_TRIGGER)) {
      std::this_thread::sleep_for(std::chrono::milliseconds(PERIODIC_TRIGGER_MS));
      for (int i=0; i<num_devices; i++) {
        FDwfDeviceTriggerPC(handler_list[i]);
      }
    }
    count++;
    if (count>=getDataMaxTrial_) {
      std::cout << "Data acquisition tried " << count << " times, but could not detect any event." << std::endl;
      std::cout << "Maybe trigger level is too high: trigger level = " << trigLevel_ << std::endl;
      break;
    }
  }

  gettimeofday(&event_time, NULL);
  header[1] = static_cast<int16_t>((event_time.tv_sec>>16)&(0xffff));
  header[2] = static_cast<int16_t>((event_time.tv_sec)&(0xffff));
  header[3] = static_cast<int16_t>((event_time.tv_usec>>16)&(0xffff));
  header[4] = static_cast<int16_t>((event_time.tv_usec)&(0xffff));
  
  if (!data_acquired) {
    std::cout << "DAQ did not detect any events." << std::endl;
    return DAQResult::NON_DETECTION;
  }
  
  std::vector<bool> detected(num_devices, true);
  for (int i=0; i<num_devices; i++) {
    if (i!=trigDevice_) {
      int num = 0;
      while (true) {
	      FDwfAnalogInStatus(handler_list[i], true, &status);
	      std::this_thread::sleep_for(std::chrono::microseconds(100));
	      if (status==DwfStateDone) {
	        //std::cout << "break!!! num = " << num << std::endl;
	        break;
	      }
	      num++;
	      if (num==1000) break;
      }
    }
    if (status!=DwfStateDone) {
      detected[i] = false;
      std::cout << "device " << i << " does not detect an event..." << std::endl;
    }
  }
  
  for (int i=0; i<num_devices; i++) {
    if (!detected[i]) continue;
    for (int j=0; j<2; j++) {
      const int k = i*2 + j;
      FDwfAnalogInStatusData16(handler_list[i], j, &data[k][0], 0, numSample_);
    }
  }

  return DAQResult::TRIGGERED;
}

void DAQIO::generateFileHeader(std::vector<int16_t>& header, int16_t num_event)
{
  const int sz_header = 32;
  header.resize(sz_header);
  timeval time_now;
  gettimeofday(&time_now, NULL);

  header[0] = static_cast<int16_t>(trigDevice_);
  header[1] = static_cast<int16_t>(trigChannel_);
  header[2] = static_cast<int16_t>(trigSrc_);
  header[3] = static_cast<int16_t>(trigSlope_);
  header[4] = static_cast<int16_t>(trigLevel_*1E3);
  header[5] = static_cast<int16_t>(trigPosition_*1E3);
  header[6] = static_cast<int16_t>(sampleFreq_*1E2);
  header[7] = static_cast<int16_t>(timeWindow_*1E3);
  header[8] = static_cast<int16_t>(numSample_);
  header[9] = static_cast<int16_t>(num_event);

  header[10] = static_cast<int16_t>((time_now.tv_sec>>16)&(0xffff));
  header[11] = static_cast<int16_t>((time_now.tv_sec)&(0xffff));
  header[12] = static_cast<int16_t>((time_now.tv_usec>>16)&(0xffff));
  header[13] = static_cast<int16_t>((time_now.tv_usec)&(0xffff));

  getRange();
  int index = 14;
  for (int i=0; i<4; i++) {
    const double scale = 1.0E3;
    header[index] = static_cast<int16_t>(((static_cast<int>(range_[i]*scale))>>16) & (0xffff));
    header[index+1] = static_cast<int16_t>((static_cast<int>(range_[i]*scale)) & (0xffff));
    index += 2;
  }
  
  getOffset();
  for (int i=0; i<4; i++) {
    const double scale = 1.0E3;
    header[index] = static_cast<int16_t>(((static_cast<int>(offset_[i]*scale))>>16) & (0xffff));
    header[index+1] = static_cast<int16_t>((static_cast<int>(offset_[i]*scale)) & (0xffff));
    index += 2;
  }
}

void DAQIO::generateFileFooter(std::vector<int16_t>& footer)
{
  const int sz_footer = 16;
  footer.resize(sz_footer);
  timeval time_now;
  gettimeofday(&time_now, NULL);
  footer[0] = static_cast<int16_t>((time_now.tv_sec>>16)&(0xffff));
  footer[1] = static_cast<int16_t>((time_now.tv_sec)&(0xffff));
  footer[2] = static_cast<int16_t>((time_now.tv_usec>>16)&(0xffff));
  footer[3] = static_cast<int16_t>((time_now.tv_usec)&(0xffff));
}

void DAQIO::getOffset()
{
  const int num_devices = ADIO_ -> NumDevices();
  const std::vector<HDWF>& handler_list = ADIO_ -> HandlerList();
  offset_.resize(4, 0.0);
  
  for (int i=0; i<2; i++) {
    for (int j=0; j<2; j++) {
      const int k = i*2+j;
      if (i<num_devices) {
        double offset = 0.0;
        FDwfAnalogInChannelOffsetGet(handler_list[i], j, &offset);
        offset_[k] = offset;
      }
    }
  }
  
}

void DAQIO::getRange()
{
  const int num_devices = ADIO_ -> NumDevices();
  const std::vector<HDWF>& handler_list = ADIO_ -> HandlerList();
  range_.resize(4, 0.0);
  
  for (int i=0; i<2; i++) {
    for (int j=0; j<2; j++) {
      const int k = i*2+j;
      if (i<num_devices) {
        double range = 0.0;
        FDwfAnalogInChannelRangeGet(handler_list[i], j, &range);
        range_[k] = range;
      }
    }
  }
}

} /* namespace gramsballoon */
#endif /* GB_DEMO_MODE */
