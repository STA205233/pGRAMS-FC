#!/usr/bin/env ruby

require 'anlnext'
require 'GRAMSBalloon'
MHz = 1000000
class MyApp < ANL::ANLApp
  def setup()
    chain GRAMSBalloon::EncodedSerialCommunicator, "MHADCManager"
    with_parameters(filename: "/dev/ttyAMA0", baudrate:9600)
    chain GRAMSBalloon::GetMHADCData
    with_parameters(num_ch: 32)
    chain GRAMSBalloon::MeasureTemperatureWithRTDSensorByMHADC
    with_parameters(channel: 0)
    chain GRAMSBalloon::EncodedSerialCommunicator, "CompresorManager"
    with_parameters(filename: "/dev/ttyUSB0", baudrate:9600)
    chain GRAMSBalloon::GetCompressorData
    with_parameters(EncodedSerialCommunicator_name: "CompresorManager")
    chain GRAMSBalloon::PressureGaugeManager, "PressureCommunicator_1"
    with_parameters(filename: "/dev/ttyUSB1", baudrate:115200)
    chain GRAMSBalloon::GetPressure
    with_parameters(EncodedSerialCommunicator_name:"PressureCommunicator_1")
  end
end

a = MyApp.new
a.run(7, 1)
