#!/usr/bin/env ruby

require 'anlnext'
require 'GRAMSBalloon'
MHz = 1000000
class MyApp < ANL::ANLApp
  def setup()
    chain GRAMSBalloon::PressureGaugeManager
    with_parameters(filename: "/dev/ttyUSB2", baudrate:115200)
    chain GRAMSBalloon::GetPressure
    with_parameters(channel: 1, EncodedSerialCommunicator_name: "PressureGaugeManager")
  end
end

a = MyApp.new
a.run(100, 1)
