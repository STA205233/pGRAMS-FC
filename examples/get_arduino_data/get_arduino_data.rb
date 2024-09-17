#!/usr/bin/env ruby

require 'anlnext'
require 'GRAMSBalloon'
MHz = 1000000
class MyApp < ANL::ANLApp
  def setup()
    chain GRAMSBalloon::GetArduinoData
    with_parameters(ch:0)
  end
end

a = MyApp.new
a.run(100, 1)
