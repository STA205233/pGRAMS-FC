#!/usr/bin/env ruby

require 'anlnext'
require 'GRAMSBalloon'
MHz = 1000000
class MyApp < ANL::ANLApp
  def setup()
    chain GRAMSBalloon::GetArduinoData
    with_parameters(filename: "test.txt", num_ch: 6)
  end
end

a = MyApp.new
a.run(7, 1)
