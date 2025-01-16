#!/usr/bin/env ruby

require 'anlnext'
require 'GRAMSBalloon'
# require 'HSQuickLook'

class MyApp < ANL::ANLApp
  def setup()
    chain GRAMSBalloon::MosquittoManager
    with_parameters(host: "localhost", port: 1883, chatter: 3, keep_alive: 60, threaded_set: true, device_id: "ground")
    chain GRAMSBalloon::ReceiveTelemetry
    with_parameters(topic: "telemetry", chatter: 100)
    chain GRAMSBalloon::InterpretTelemetry
    with_parameters(save_telemetry: true, num_telem_per_file: 1000, chatter: 0, binary_filename_base: Dir.home + "/data/telemetry/telemetry")
    chain GRAMSBalloon::PushToMySQL
    with_parameters(host: ENV["PGRAMS_MYSQL_HOST"], user: ENV["PGRAMS_MYSQL_USER"], password: ENV["PGRAMS_MYSQL_PASSWD"], database: "pgrams", check_exist: true)
    #chain GRAMSBalloon::Sleep
    #with_parameters(sleep_sec: 1)  
  end
end


a = MyApp.new

a.run(:all, 1)
