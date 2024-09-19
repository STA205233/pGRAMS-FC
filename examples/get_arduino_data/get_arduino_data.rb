#!/usr/bin/env ruby

require 'anlnext'
require 'GRAMSBalloon'
MHz = 1000000
class MyApp < ANL::ANLApp
  def setup()
    chain GRAMSBalloon::GetArduinoData
    with_parameters()
  end
end

a = MyApp.new
a.run(1, 1)
