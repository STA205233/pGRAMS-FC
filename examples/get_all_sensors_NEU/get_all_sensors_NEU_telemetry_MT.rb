#!/usr/bin/env ruby
require 'anlnext'
require 'GRAMSBalloon'

class MyApp < ANL::ANLApp
    def setup()
        chain GRAMSBalloon::ReceiveCommand
        with_parameters(serial_path: "/dev/ttyS0", chatter: 0, binary_filename_base: "/home/grams/data/command/command") do |m|  
          m.set_singleton(1)
        end
        # chain GRAMSBalloon::EncodedSerialCommunicator, "MHADCManager"
        # with_parameters(filename: "/dev/ttyAMA0", baudrate:9600, timeout_sec: 0, timeout_usec: 100)
        # chain GRAMSBalloon::GetMHADCData
        # with_parameters(num_ch: 32, sleep_for_msec: 10, MHADCManager_name: "MHADCManager", chatter: 0)
        chain GRAMSBalloon::Sleep
        with_parameters(sleep_sec: 1) do |m|
          m.set_singleton(0)
        end
        chain GRAMSBalloon::GetArduinoData
        with_parameters(num_ch: 6, sleep_for_msec: 10, timeout_sec:0, timeout_usec:10000, chatter: 0) do |m|
          m.set_singleton(0)
        end
        
        measure_temperature_modules = []
        for i in 0..5 do
          # chain GRAMSBalloon::MeasureTemperatureWithRTDSensorByMHADC, "MeasureTemperatureWithRTDSensorByMHADC_#{i}"
          chain GRAMSBalloon::MeasureTemperatureWithRTDSensorByArduino, "MeasureTemperatureWithRTDSensorByArduino_#{i}"
          with_parameters(channel: i, chatter: 0) do |m|
              m.set_singleton(0)
          end
          # measure_temperature_modules << "MeasureTemperatureWithRTDSensorByMHADC_#{i}"
          measure_temperature_modules << "MeasureTemperatureWithRTDSensorByArduino_#{i}"
        end
        
        chain GRAMSBalloon::EncodedSerialCommunicator, "CompressorManager"
        with_parameters(filename: "/dev/ttyUSB2", baudrate: 13, timeout_usec: 10000, timeout_sec: 0) do |m|
          m.set_singleton(0)
        end
        chain GRAMSBalloon::GetCompressorData
        with_parameters(EncodedSerialCommunicator_name: "CompressorManager", sleep_for_msec: 10) do |m|
          m.set_singleton(0)
        end
        chain GRAMSBalloon::PressureGaugeManager, "PressureCommunicator_1"
        with_parameters(filename: "/dev/ttyUSB0", baudrate: 4098,  timeout_usec: 10000, timeout_sec: 0) do |m|
          m.set_singleton(0)
        end
        chain GRAMSBalloon::GetPressure, "GetPressure_1"
        with_parameters(EncodedSerialCommunicator_name:"PressureCommunicator_1", sleep_for_msec: 500, channel: 2, chatter: 1, type: "jp") do |m|
          m.set_singleton(0)
        end
        #chain GRAMSBalloon::PressureGaugeManager, "PressureCommunicator_2"
        #with_parameters(filename: "/dev/ttyUSB1", baudrate: 4098, timeout_usec: 0, timeout_sec: 1) do |m|
        #  m.set_singleton(0)
        #end
        #chain GRAMSBalloon::GetPressure, "GetPressure_2"
        #with_parameters(EncodedSerialCommunicator_name:"PressureCommunicator_2", sleep_for_msec: 10, channel: 1, chatter: 1) do |m|
          #m.set_singleton(0)
        #end
        chain GRAMSBalloon::SendTelemetry
        with_parameters(
          # serial_path: "./telemetryPTY0",
          serial_path: "/dev/ttyS0",
          MeasureTemperature_module_names: measure_temperature_modules,
          GetPressure_jacket_module_name: "GetPressure_1",
          GetPressure_chamber_module_name: "GetPressure_2",
          binary_filename_base:ENV["HOME"] + "/data/telemetry_test/telemetry",
          chatter: 0,
          sleep_for_msec: 10
        ) do |m|
            m.set_singleton(0)
        end

        chain GRAMSBalloon::RunIDManager
        with_parameters(
          filename: ENV["HOME"] + "/settings/run_id/run_id.txt"
        ) do |m|
            m.set_singleton(0)
        end

    end
end

a = MyApp.new


a.num_parallels = 2
a.run(1000000000, 1)
exit_status = 1
puts "exit_status: #{exit_status}"
exit exit_status
