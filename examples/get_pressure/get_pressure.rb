#!/usr/bin/env ruby

require 'anlnext'
require 'GRAMSBalloon'
MHz = 1000000
class MyApp < ANL::ANLApp
  def setup()
    chain GRAMSBalloon::EncodedSerialCommunicator
    with_parameters(filename: "/dev/ttyUSB2", baudrate:9600)
    chain GRAMSBalloon::GetPressure
    with_parameters(channel: 1)
  end
end

a = MyApp.new
a.run(7, 1)
