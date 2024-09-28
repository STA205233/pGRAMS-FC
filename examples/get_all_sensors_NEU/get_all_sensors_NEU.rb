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
    chain GRAMSBalloon::MeasureTemperatureWithRTDSensorByMHADC, "MeasureTemperatureWithRTDSensorByMHADC_1"
    with_parameters(channel: 0)
    chain GRAMSBalloon::MeasureTemperatureWithRTDSensorByMHADC, "MeasureTemperatureWithRTDSensorByMHADC_2"
    with_parameters(channel: 1)
    chain GRAMSBalloon::MeasureTemperatureWithRTDSensorByMHADC, "MeasureTemperatureWithRTDSensorByMHADC_3"
    with_parameters(channel: 2)
    chain GRAMSBalloon::MeasureTemperatureWithRTDSensorByMHADC, "MeasureTemperatureWithRTDSensorByMHADC_4"
    with_parameters(channel: 3)
    chain GRAMSBalloon::MeasureTemperatureWithRTDSensorByMHADC, "MeasureTemperatureWithRTDSensorByMHADC_5"
    with_parameters(channel: 4)
    chain GRAMSBalloon::MeasureTemperatureWithRTDSensorByMHADC, "MeasureTemperatureWithRTDSensorByMHADC_6"
    with_parameters(channel: 5)
    chain GRAMSBalloon::EncodedSerialCommunicator, "CompresorManager"
    with_parameters(filename: "/dev/ttyUSB0", baudrate:9600)
    chain GRAMSBalloon::GetCompressorData
    with_parameters(EncodedSerialCommunicator_name: "CompresorManager")
    chain GRAMSBalloon::PressureGaugeManager, "PressureCommunicator_1"
    with_parameters(filename: "/dev/ttyUSB1", baudrate:115200)
    chain GRAMSBalloon::GetPressure, "GetPressure_1"
    with_parameters(EncodedSerialCommunicator_name:"PressureCommunicator_1")
    chain GRAMSBalloon::PressureGaugeManager, "PressureCommunicator_2"
    with_parameters(filename: "/dev/ttyUSB2", baudrate:115200)
    chain GRAMSBalloon::GetPressure, "GetPressure_2"
    with_parameters(EncodedSerialCommunicator_name:"PressureCommunicator_2")
  end
end

a = MyApp.new
a.run(7, 1)
