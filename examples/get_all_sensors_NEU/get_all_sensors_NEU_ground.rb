#!/usr/bin/env ruby

require 'anlnext'
require 'GRAMSBalloon'
require 'HSQuickLook'

class MyApp < ANL::ANLApp
  def setup()
    chain HSQuickLook::MongoDBClient
    with_parameters(database: "grams")
    chain GRAMSBalloon::ReceiveTelemetry
    with_parameters(serial_path: @serial_path, open_mode: 2, chatter: 1)
    chain GRAMSBalloon::InterpretTelemetry
    with_parameters(save_telemetry: true, num_telem_per_file: 1000, chatter: 0, binary_filename_base: Dir.home + "/data/telemetry/telemetry")
    chain GRAMSBalloon::PushToMongoDB
    chain GRAMSBalloon::PushToMySQL
    with_parameters(host: ENV["PGRAMS_MYSQL_HOST"], user: ENV["PGRAMS_MYSQL_USER"], password: ENV["PGRAMS_MYSQL_PASSWD"], database: "pgrams", check_exist: true)
  end
  attr_accessor :serial_path
end


a = MyApp.new
a.serial_path = "telemetryPTY1"

a.run(:all, 1)
