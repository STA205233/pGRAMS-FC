#!/usr/bin/env ruby
require 'anlnext'
require 'GRAMSBalloon'

class MyApp < ANL::ANLApp
    def setup()

        chain GRAMSBalloon::ReceiveCommand
        with_parameters(serial_path: "./commandPTY0", chatter: 0, binary_filename_base: ENV["HOME"] + "/data/command/command") do |m|
            m.set_singleton(1)
        end

        rtd_cs_array = [8, 7, 16, 20, 21]
        rtd_cs_array.each_with_index do |cs, i|
          chain GRAMSBalloon::MeasureTemperatureWithRTDSensor, "MeasureTemperatureWithRTDSensor_#{i+1}"
          with_parameters(chip_select: cs) do |m|
              m.set_singleton(0)
          end
        end

        env_cs_array = [26, 19, 6, 5, 22]
        env_cs_array.each_with_index do |cs, i|
          chain GRAMSBalloon::GetEnvironmentalData,"GetEnvironmentalData_#{i+1}"
          with_parameters(chip_select: cs) do |m|
              m.set_singleton(0)
          end
        end

        chain GRAMSBalloon::MeasureAcceleration
        with_parameters(calibrate_gyro: false) do |m|
            m.set_singleton(0)
        end

        chain GRAMSBalloon::GetSlowADCData
        with_parameters(chip_select: 17, Va: 5.026,
        channels: [0, 1, 2, 3, 4], num_trials: 100, chatter: 0, channels_mean:[28.0, 1.0, 1.0, 0.0, 0.0], channels_width:[0.1, 0.1, 0.01, 0.01, 0.01], main_voltage_channel:0, main_current_channel:1, chamber_pressure_channel: 2, tpc_hv_voltage_channel:3, tpc_hv_current_channel:4) do |m|
            m.set_singleton(0)
        end

        chain GRAMSBalloon::GetRaspiStatus do |m|
            m.set_singleton(0)
        end

        chain GRAMSBalloon::ControlHighVoltage, "ControlHighVoltage_TPC"
        with_parameters(device_id: 0, channel: 0, voltages: [], upper_limit_voltage: 0.1) do |m|
          m.set_singleton(0)
        end
        ### conversion: HV = DAC * 2000.0
        
        chain GRAMSBalloon::ControlHighVoltage, "ControlHighVoltage_PMT"
        with_parameters(device_id: 0, channel: 1, voltages: [], upper_limit_voltage: 3.2) do |m|
          m.set_singleton(0)
        end
        ### conversion: HV = DAC * 250.0
                
        chain GRAMSBalloon::ReadWaveform
        with_parameters(
            trig_device: 0,
            trig_channel: 0,
            trig_mode: 18,
            trig_level: -0.01,
            trig_position: 5.0,
            time_window: 160.0, # us
            sample_frequency: 50.0, #MHz
            output_filename_base: ENV["HOME"] + "/data/daq/daq_output",
            num_events_per_file: 100,
            start_reading: true,
            non_detection_rate: 0.1
        ) do |m|
          m.set_singleton(2)
        end


        chain GRAMSBalloon::SendTelemetry
        with_parameters(
          serial_path: "./telemetryPTY0",
          MeasureTemperature_module_names: ["MeasureTemperatureWithRTDSensor_1",
                                            "MeasureTemperatureWithRTDSensor_2",
                                            "MeasureTemperatureWithRTDSensor_3",
                                            "MeasureTemperatureWithRTDSensor_4",
                                            "MeasureTemperatureWithRTDSensor_5"],
          GetEnvironmentalData_module_names: ["GetEnvironmentalData_1",
                                              "GetEnvironmentalData_2",
                                              "GetEnvironmentalData_3",
                                              "GetEnvironmentalData_4",
                                              "GetEnvironmentalData_5"],
          TPCHVController_module_name: "ControlHighVoltage_TPC",
          PMTHVController_module_name: "ControlHighVoltage_PMT",
          binary_filename_base:ENV["HOME"] + "/data/telemetry/telemetry",
          chatter: 0
        ) do |m|
            m.set_singleton(0)
        end

        chain GRAMSBalloon::RunIDManager
        with_parameters(
          filename: ENV["HOME"] + "/settings/run_id/run_id.txt"
        ) do |m|
            m.set_singleton(0)
        end

        chain GRAMSBalloon::ShutdownSystem do |m|
            m.set_singleton(0)
        end
    end
end




main_modules = []
for i in 1..5 do
  main_modules << "MeasureTemperatureWithRTDSensor_#{i}"
end

for i in 1..5 do
  main_modules << "GetEnvironmentalData_#{i}"
end

main_modules << "MeasureAcceleration" << "GetSlowADCData" << "GetRaspiStatus"
main_modules << "ControlHighVoltage_TPC" << "ControlHighVoltage_PMT"
main_modules << "SendTelemetry" << "RunIDManager" << "ShutdownSystem"

daq_modules = ["ReadWaveform"]
command_modules = ["ReceiveCommand"]


a = MyApp.new


a.num_parallels = 3

a.modify do |m|
  main_modules.each do |mod|
    m.get_parallel_module(1, mod).off
    m.get_parallel_module(2, mod).off
  end
  command_modules.each do |mod|
    m.get_parallel_module(0, mod).off
    m.get_parallel_module(2, mod).off
  end
  daq_modules.each do |mod|
    m.get_parallel_module(0, mod).off
    m.get_parallel_module(1, mod).off
  end
  m.get_parallel_module(0, "GetEnvironmentalData_1").off
  m.get_parallel_module(0, "GetEnvironmentalData_2").off
  puts m.class
end

a.run(1000000000, 1)
exit_status = 1


exit_status = a.get_parallel_module(0, "ShutdownSystem").get_result_value("exit_status")


puts "exit_status: #{exit_status}"
exit exit_status

