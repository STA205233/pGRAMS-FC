#!/usr/bin/env ruby

require 'anlnext'
require 'GRAMSBalloon'
MHz = 1000000
class MyApp < ANL::ANLApp
  def setup()
    chain GRAMSBalloon::EncodedSerialCommunicator
    with_parameters(filename: "/dev/ttyUSB0", baudrate:9600)
    chain GRAMSBalloon::GetCompressorData
  end
end

a = MyApp.new
a.run(7, 1)
