########################################################
set(TARGET_EXT_LIBRARY GRAMSBalloon)
set(EXT_LIB_INSTALL_DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/ruby)
set(SWIG_IF_FILE ${TARGET_EXT_LIBRARY}.i)
set(CLASS_LIST_FILE class_list_${TARGET_EXT_LIBRARY}.hh)
set(RUBY_EXT_INCLUDE_DIRS
  ../source/modules/include
  ../source/core/include
  ../source/demo/include
  ../source/BME68x-Sensor-API/include
  ../source/ICM20948-linux-i2c/src
  ../source/ICM20948-linux-i2c/I2C/src
  ${ANLNEXT_INC_DIR}
  ${BOOST_INC_DIR}
  ${HSQUICKLOOK_INC_DIR}
  ${ROOT_INC_DIR}
  )
set(RUBY_EXT_LIBRARY_DIRS
  /usr/local/lib
  ${ANLNEXT_LIB_DIR}
  ${ADD_LIB_DIR}
  ${CMAKE_INSTALL_PREFIX}/lib
  ${ROOT_LIB_DIR}
  )
set(RUBY_EXT_LIBRARIES
  ${ANLNEXT_LIB}
  ${BME68x_LIB}
  GBCore
  GBModules
  ${PIGPIO_LIB_DAEMON}
  ${PIGPIO_LIB_RT}
  ${WAVEFORMS_LIB}
  ${ICM20948_LIB}
  ${HSQUICKLOOK_LIB}
  ${MONGODB_LIB}
  ${ROOT_LIB}
)


########################################################
set(cxx_definitions )
if(GB_USE_PIGPIO)
  list(APPEND cxx_definitions -DUSE_PIGPIO)
endif()
if(GB_USE_WAVEFORMS)
  list(APPEND cxx_definitions -DUSE_WAVEFORMS)
endif()
if(GB_USE_ICM20948)
  list(APPEND cxx_definitions -DUSE_ICM20948)
endif()
if(GB_USE_RASPISYS)
  list(APPEND cxx_definitions -DUSE_RASPISYS)
endif()
if(GB_USE_HSQUICKLOOK)
  list(APPEND cxx_definitions -DUSE_HSQUICKLOOK)
endif()
if(GB_USE_ROOT)
  list(APPEND cxx_definitions -DUSE_ROOT)
endif()
if(GB_MAC)
  list(APPEND cxx_definitions -DGB_MAC)
endif()
if(GB_DEMO_MODE)
  list(APPEND cxx_definitions -DGB_DEMO_MODE)
endif()

set(SWIG_FLAGS ${cxx_definitions})
add_definitions(${cxx_definitions})

message("-- cxx_definitions: ${cxx_definitions}")

########################################################
include(CreateSwigRuby)
