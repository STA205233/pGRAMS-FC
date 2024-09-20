#!/usr/bin/env ruby

require 'anlnext'
require 'GRAMSBalloon'
MHz = 1000000
class MyApp < ANL::ANLApp
  def setup()
    chain GRAMSBalloon::MHADCManager
    with_parameters(filename: "/dev/ttyAMA0", baudrate:9600)
    chain GRAMSBalloon::GetMHADCData
    with_parameters(num_ch: 32)
  end
end

a = MyApp.new
a.run(7, 1)
