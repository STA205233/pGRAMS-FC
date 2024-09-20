%module GRAMSBalloon
%{
// include headers of my modules
#include <anlnext/BasicModule.hh>
#include <string>
#include "SimpleLoop.hh"
#ifdef USE_PIGPIO
#include "SPIManager.hh"
#endif
#ifdef USE_PIGPIO
#include "GetEnvironmentalData.hh"
#endif
#ifndef GB_MAC
#include "ReceiveCommand.hh"
#endif
#ifdef USE_WAVEFORMS
#include "AnalogDiscoveryManager.hh"
#endif
#ifdef USE_WAVEFORMS
#include "ControlHighVoltage.hh"
#endif
#ifdef USE_PIGPIO
#include "MeasureTemperatureWithRTDSensor.hh"
#endif
#ifdef USE_WAVEFORMS
#include "ReadWaveform.hh"
#endif
#ifndef GB_MAC
#include "SendTelemetry.hh"
#endif
#ifdef USE_RASPISYS
#include "GetRaspiStatus.hh"
#endif
#ifdef USE_ICM20948
#include "MeasureAcceleration.hh"
#endif
#include "ReceiveTelemetry.hh"
#ifdef USE_RASPISYS
#include "ShutdownSystem.hh"
#endif
#ifdef USE_ROOT
#include "InterpretTelemetry.hh"
#endif
#ifdef USE_HSQUICKLOOK
#include "PushToMongoDB.hh"
#endif
#ifdef USE_PIGPIO
#include "GetSlowADCData.hh"
#endif
#include "RunIDManager.hh"
#include "ReadTelemetry.hh"
#include "DumpSerial.hh"
#include "GetArduinoData.hh"
#include "GetMHADCData.hh"
#include "EncodedSerialCommunicator.hh"
#include "GetCompressorData.hh"
#ifdef USE_ROOT
#include "PlotWaveform.hh"
#endif
#ifdef GB_DEMO_MODE
#include "GBBasicDemoModule.hh"
#include "ShutdownSystemDemo.hh"
#include "GetEnvironmentalDataDemo.hh"
#include "MeasureAccelerationDemo.hh"
#include "MeasureTemperatureWithRTDSensorDemo.hh"
#include "MeasureTemperatureWithRTDSensorByArduino.hh"
#include "GetRaspiStatusDemo.hh"
#include "ControlHighVoltageDemo.hh"
#include "ReadWaveformDemo.hh"
#include "GetSlowADCDataDemo.hh"
#endif
%}

%include "std_vector.i"
%import(module="anlnext/ANL") "anlnext/ruby/ANL.i"

// interface to my modules

namespace gramsballoon {

class SimpleLoop : public anlnext::BasicModule
{
public:
  SimpleLoop();
};

#ifdef USE_PIGPIO
class SPIManager : public anlnext::BasicModule
{
public:
  SPIManager();
};
#endif

#ifdef USE_PIGPIO
class GetEnvironmentalData : public anlnext::BasicModule
{
public:
  GetEnvironmentalData();
};
#endif

#if defined GB_MAC || defined GB_DEMO_MODE
class ReceiveCommand : public anlnext::BasicModule
{
public:
  ReceiveCommand();
};
#endif

#ifdef USE_WAVEFORMS
class AnalogDiscoveryManager : public anlnext::BasicModule
{
public:
  AnalogDiscoveryManager();
};
#endif

#ifdef USE_WAVEFORMS
class ControlHighVoltage : public anlnext::BasicModule
{
public:
  ControlHighVoltage();
};
#endif

#ifdef USE_PIGPIO
class MeasureTemperatureWithRTDSensor : public anlnext::BasicModule
{
public:
  MeasureTemperatureWithRTDSensor();
};
#endif

#ifdef USE_WAVEFORMS
class ReadWaveform : public anlnext::BasicModule
{
public:
  ReadWaveform();
};
#endif

#if defined GB_MAC || defined GB_DEMO_MODE
class SendTelemetry : public anlnext::BasicModule
{
public:
  SendTelemetry();
};
#endif

#ifdef USE_RASPISYS
class GetRaspiStatus : public anlnext::BasicModule
{
public:
  GetRaspiStatus();
};
#endif

#ifdef USE_ICM20948
class MeasureAcceleration :  public anlnext::BasicModule
{
public:
  MeasureAcceleration();
};
#endif

class ReceiveTelemetry :  public anlnext::BasicModule
{
public:
  ReceiveTelemetry();
};

class DumpSerial: public anlnext::BasicModule {
public:
    DumpSerial();
};

namespace pgrams {
class GetArduinoData: public anlnext::BasicModule {
public:
  GetArduinoData();
};

class GetMHADCData: public anlnext::BasicModule {
public:
  GetMHADCData();
};

class EncodedSerialCommunicator: public anlnext::BasicModule {
public:
  EncodedSerialCommunicator();
};

class GetCompressorData: public anlnext::BasicModule {
public:
  GetCompressorData();
};
} // namespace pgrams

#ifdef USE_RASPISYS
class ShutdownSystem : public anlnext::BasicModule
{
public:
  ShutdownSystem();
};
#endif

#ifdef USE_ROOT
class InterpretTelemetry : public anlnext::BasicModule
{
public:
  InterpretTelemetry();
};
#endif

#ifdef USE_HSQUICKLOOK
class PushToMongoDB : public anlnext::BasicModule
{
public:
  PushToMongoDB();
};
#endif

#ifdef USE_PIGPIO
class GetSlowADCData : public anlnext::BasicModule
{
public:
  GetSlowADCData();
};
#endif

class RunIDManager : public anlnext::BasicModule
{
public:
  RunIDManager();
};

class ReadTelemetry : public anlnext::BasicModule
{
public:
  ReadTelemetry();
};

#ifdef USE_ROOT
class PlotWaveform : public anlnext::BasicModule
{
public:
  PlotWaveform();
};
#endif
#ifdef GB_DEMO_MODE
class GBBasicDemoModule : public anlnext::BasicModule
{
public:
  GBBasicDemoModule();
protected:
  double SampleFromUniformDistribution();
  void PrintInfo(const std::string &msg) const;
};

class ShutdownSystem : public anlnext::BasicModule
{
public:
  ShutdownSystem();
};

class GetEnvironmentalData : public GBBasicDemoModule
{
public:
  GetEnvironmentalData();
};

class MeasureAcceleration : public GBBasicDemoModule
{
public:
  MeasureAcceleration();
};

class MeasureTemperatureWithRTDSensor : public GBBasicDemoModule
{
public:
  MeasureTemperatureWithRTDSensor();
};
namespace pgrams{
class MeasureTemperatureWithRTDSensorByArduino: public GBBasicDemoModule {
public:
  MeasureTemperatureWithRTDSensorByArduino();
};
} // namespace pgrams
class GetRaspiStatus : public GBBasicDemoModule
{
public:
  GetRaspiStatus();
};

class ControlHighVoltage : public GBBasicDemoModule
{
public:
  ControlHighVoltage();
};

class ReadWaveform : public GBBasicDemoModule
{
public:
  ReadWaveform();
};

class GetSlowADCData : public GBBasicDemoModule
{
public:
  GetSlowADCData();
};
#endif
} // namespace GRAMSBalloon
