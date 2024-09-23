include_directories(
  include
  ../core/include
  ../BME68x-Sensor-API/include
  ../ICM20948-linux-i2c/src
  ../ICM20948-linux-i2c/I2C/src
  ${ANLG4_INC_DIR}
  ${ANLNEXT_INC_DIR}
  ${ROOT_INC_DIR}
  ${G4_INC_DIR}
  ${BOOST_INC_DIR}
  ${PIGPIO_INC_DIR}
  ${HSQUICKLOOK_INC_DIR}
  )

link_directories(
  /usr/local/lib
  ${ANLNEXT_LIB_DIR}
  ${ROOT_LIB_DIR}
  ${G4_LIB_DIR}
  )

set(GB_MODULES
  src/SimpleLoop.cc
  src/ReceiveTelemetry.cc
  src/RunIDManager.cc
  src/ReadTelemetry.cc
  src/DumpSerial.cc
  src/GetArduinoData.cc
  src/EncodedSerialCommunicator.cc
  src/GetMHADCData.cc
  src/GetCompressorData.cc
  src/GetPressure.cc
  src/PressureGaugeManager.cc
  )

if(GB_DEMO_MODE)
  set(GB_DEMO_MODULES
    ../demo/src/GBBasicDemoModule.cc
    ../demo/src/MeasureTemperatureWithRTDSensorDemo.cc
    ../demo/src/ReadWaveformDemo.cc
    ../demo/src/MeasureAccelerationDemo.cc
    ../demo/src/GetRaspiStatusDemo.cc
    ../demo/src/ShutdownSystemDemo.cc
    ../demo/src/ControlHighVoltageDemo.cc
    ../demo/src/GetEnvironmentalDataDemo.cc
    ../demo/src/GetSlowADCDataDemo.cc
    ../demo/src/GetSlowADCDataIncludingPressure.cc
    ../demo/src/MeasureTemperatureWithRTDSensorByArduino.cc
    ../demo/src/MeasureTemperatureWithRTDSensorByMHADC.cc
  )
  list(APPEND GB_MODULES 
    src/ReceiveCommand.cc
    src/SendTelemetry.cc
  )
   include_directories(../demo/include)
endif(GB_DEMO_MODE) 
  
if(GB_USE_PIGPIO)
  set(GB_PIGPIO_MODULES
    src/SPIManager.cc
    src/GetEnvironmentalData.cc
    src/MeasureTemperatureWithRTDSensor.cc
    src/GetSlowADCData.cc
    )
endif(GB_USE_PIGPIO)

if(GB_USE_WAVEFORMS)
  set(GB_WAVEFORMS_MODULES
    src/AnalogDiscoveryManager.cc
    src/ControlHighVoltage.cc
    src/ReadWaveform.cc
    src/ReceiveCommand.cc
    src/SendTelemetry.cc
    )
endif(GB_USE_WAVEFORMS)

if(GB_USE_ICM20948)
  set(GB_ICM20948_MODULES
    src/MeasureAcceleration.cc
    )
endif(GB_USE_ICM20948)

if(GB_USE_RASPISYS)
  set(GB_RASPISYS_MODULES
    src/GetRaspiStatus.cc
    src/ShutdownSystem.cc
    )
endif(GB_USE_RASPISYS)

if(GB_USE_HSQUICKLOOK)
  set(GB_HSQUICKLOOK_MODULES
    src/PushToMongoDB.cc
  )
endif(GB_USE_HSQUICKLOOK)

if(GB_USE_ROOT)
  set(GB_ROOT_MODULES
    src/PlotWaveform.cc
    src/InterpretTelemetry.cc
  )
endif(GB_USE_ROOT)

add_library(GBModules SHARED
  ${GB_MODULES}
  ${GB_PIGPIO_MODULES}
  ${GB_WAVEFORMS_MODULES}
  ${GB_ICM20948_MODULES}
  ${GB_RASPISYS_MODULES}
  ${GB_HSQUICKLOOK_MODULES}
  ${GB_ROOT_MODULES}
  ${GB_DEMO_MODULES}
  )

target_link_libraries(GBModules
  GBCore
  ${BME68x_LIB}
  ${ANLG4_LIB}
  ${ANLNEXT_LIB}
  ${ROOT_LIB}
  ${G4_LIB}
  ${PIGPIO_LIB_DAEMON}
  ${PIGPIO_LIB_RT}
  ${WAVEFORMS_LIB}
  ${ICM20948_LIB}
  ${MONGODB_LIB}
  ${HSQUICKLOOK_LIB}
  )

install(TARGETS GBModules LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

if(INSTALL_HEADERS)
  install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_PREFIX}/include/GRAMSBalloon
    FILES_MATCHING PATTERN "*.hh")
  if (GB_DEMO_MODE)
  install(DIRECTORY ../demo/include/
    DESTINATION ${CMAKE_INSTALL_PREFIX}/include/GRAMSBalloon
    FILES_MATCHING PATTERN "*.hh")
  endif(GB_DEMO_MODE)
endif(INSTALL_HEADERS)
