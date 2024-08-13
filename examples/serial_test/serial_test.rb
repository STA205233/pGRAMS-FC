#!/usr/bin/env ruby

require 'anlnext'
require 'GRAMSBalloon'
require 'HSQuickLook'

class MyApp < ANL::ANLApp
  def setup()
    chain GRAMSBalloon::ReceiveTelemetry
    with_parameters(serial_path: "telemetryPTY1", open_mode: 2, chatter: 1)
    chain GRAMSBalloon::DumpSerial
    with_parameters(chatter: 1, binary_filename_base: "./test_")
  end
  attr_accessor :serial_path
end

def get_serial_path()
  filename = Dir.home + "/settings/serial/telemetry_port.txt"
  ret = ""
  File.open(filename, mode="rt") do |f|
    f.each_line do |line|
      ret = line.strip
    end
  end
  return ret
end

a = MyApp.new
a.serial_path = get_serial_path()

a.run(:all, 1)
