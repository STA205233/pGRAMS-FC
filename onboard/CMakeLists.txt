cmake_minimum_required(VERSION 3.6)
#CMAKE_MINIMUM_REQUIRED_VERSION(VERSION 3.14)

### Initial definition of cmake variables
set(CMAKE_INSTALL_PREFIX $ENV{HOME} CACHE PATH "install prefix")
# set(CMAKE_BUILD_TYPE Release CACHE STRING "build type")
set(CMAKE_BUILD_TYPE Debug CACHE STRING "build type")
set(CMAKE_CXX_FLAGS_DEBUG "-g -W -Wall" CACHE STRING "CXX_FLAGS for debug")
set(CMAKE_C_FLAGS_DEBUG "-g -W -Wall" CACHE STRING "C_FLAGS for debug")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -W -Wall" CACHE STRING "CXX_FLAGS for release")
set(CMAKE_C_FLAGS_RELEASE "-O3 -W -Wall" CACHE STRING "C_FLAGS for release")
set(CMAKE_MACOSX_RPATH 1)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

### Definition of project
project(GRAMSBalloon CXX C)
set(PROJECT_VERSION 1.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)
set(USE_RUBY ON)
set(MY_LIBRARY GRAMSBalloon)
#set(RASPI ON)

find_package(Ruby 3.0 REQUIRED)

### options
## library options
option(GB_USE_PIGPIO "enable PIGPIO" ON)
option(GB_USE_WAVEFORMS "enable waveforms" ON)
option(GB_USE_ICM20948 "enable ICM20948" ON)
option(GB_USE_RASPISYS "enable raspi system control" ON)
option(GB_USE_HSQUICKLOOK "enable hsquicklook" OFF)
option(GB_APT_RUBY "installed ruby via apt" ON)
option(GB_USE_ROOT "enable root" OFF)
option(GB_USE_MYSQL "enable mysql" OFF)
option(GB_USE_MOSQUITTO "enable mosquitto" ON)

## install options
option(GB_INSTALL_HEADERS "install all header files" ON)
option(GB_INSTALL_CMAKE_FILES "install all cmake files" ON)
option(GB_DEMO_MODE "demo mode" OFF)
set(INSTALL_HEADERS ${GB_INSTALL_HEADERS})
set(INSTALL_CMAKE_FILES ${GB_INSTALL_CMAKE_FILES})
## shortcut options
option(GB_MAC "shortcut" OFF)

if (GB_MAC)
  message("-- GB_MAC is set")
  set(GB_USE_PIGPIO OFF)
  set(GB_USE_WAVEFORMS OFF)
  set(GB_USE_ICM20948 OFF)
  set(GB_USE_RASPISYS OFF)
  set(GB_USE_HSQUICKLOOK ON)
  set(GB_APT_RUBY OFF)
  set(GB_USE_ROOT ON)
endif(GB_MAC)

if (GB_DEMO_MODE)
  message("-- GB_DEMO_MODE is set")
  set(GB_USE_PIGPIO OFF)
  set(GB_USE_WAVEFORMS OFF)
  set(GB_USE_ICM20948 OFF)
  set(GB_USE_RASPISYS OFF)
  set(GB_APT_RUBY OFF)
  add_compile_definitions("GB_DEMO_MODE")
endif(GB_DEMO_MODE)

if (GB_APT_RUBY)
  set(RUBY_CONFIG_FILE_DIR /usr/include/arm-linux-gnueabihf/ruby-2.7.0)
endif(GB_APT_RUBY)

if (GB_USE_ROOT)
  add_compile_definitions("USE_ROOT")
endif(GB_USE_ROOT)

if (GB_USE_HSQUICKLOOK)
  add_compile_definitions("USE_HSQUICKLOOK")
endif(GB_USE_HSQUICKLOOK)


### External libraries
### BOOST ###
find_package(Boost 1.60.0 REQUIRED CONFIG)
set(BOOST_INC_DIR ${Boost_INCLUDE_DIRS})
set(BOOST_LIB_DIR ${Boost_LIBRARY_DIRS})
set(BOOST_LIB ${Boost_LIBRARIES})
message("-- BOOST_INC_DIR: ${BOOST_INC_DIR}")
message("-- BOOST_LIB_DIR: ${BOOST_LIB_DIR}")
message("-- BOOST_LIB: ${BOOST_LIB}")

#ANL
if(NOT DEFINED ANLNEXT_INSTALL)
  if(DEFINED ENV{ANLNEXT_INSTALL})
    set(ANLNEXT_INSTALL $ENV{ANLNEXT_INSTALL})
  else()
    set(ANLNEXT_INSTALL $ENV{HOME})
  endif()
endif(NOT DEFINED ANLNEXT_INSTALL)
set(ANLNEXT_INC_DIR ${ANLNEXT_INSTALL}/include)
set(ANLNEXT_LIB_DIR ${ANLNEXT_INSTALL}/lib)
set(ANLNEXT_LIB ANLNext)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ANLNEXT_LIB_DIR}/anlnext)
message("-- ANLNEXT_INSTALL = ${ANLNEXT_INSTALL}")

### PIGPIO ###
if(GB_USE_PIGPIO)
  set(PIGPIO_INSTALL_PREFIX /home/grams/software/pigpio/util)
  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PIGPIO_INSTALL_PREFIX})
  find_package(pigpio REQUIRED)
  set(PIGPIO_INC_DIR ${pigpio_INCLUDE_DIRS})
  set(PIGPIO_LIB_DIR ${pigpio_LIBRARY_DIRS})
  set(PIGPIO_LIB ${pigpio_LIBRARIES})
  message("-- PIGPIO_INC_DIR: ${PIGPIO_INC_DIR}")
  message("-- PIGPIO_LIB_DIR: ${PIGPIO_LIB_DIR}")
  message("-- PIGPIO_LIB: ${PIGPIO_LIB}")
  set(PIGPIO_LIB_DAEMON pigpiod_if2)
  message("-- PIGPIO_LIB_DAEMON: ${PIGPIO_LIB_DAEMON}")
  set(PIGPIO_LIB_RT rt)
  message("-- PIGPIO_LIB_RT: ${PIGPIO_LIB_RT}")
  set(BME68x_LIB GBBME68x)
endif(GB_USE_PIGPIO)

### Waveforms ###
if(GB_USE_WAVEFORMS)
  set(WAVEFORMS_LIB dwf)
  message("-- WAVEFORMS_LIB: ${WAVEFORMS_LIB}")
endif(GB_USE_WAVEFORMS)

### ICM20948 ###
if(GB_USE_ICM20948)
  set(ICM20948_LIB ICM20948 I2C)
  message("-- ICM20948_LIB: ${ICM20948_LIB}")
endif(GB_USE_ICM20948)

### HSQUICKLOOK ###
if(GB_USE_HSQUICKLOOK)
  find_path(HSQUICKLOOK_INC_DIR
    NAMES hsquicklook/DocumentBuilder.hh
    PATHS $ENV{HSQUICKLOOK_INSTALL}/include /usr/local/include $ENV{HOME}/include)
  find_library(HSQUICKLOOK_LIB
    NAMES HSQuickLookAnalyzer
    PATHS $ENV{HSQUICKLOOK_INSTALL}/lib /usr/local/lib $ENV{HOME}/lib)
  message("-- HSQUICKLOOK_INC_DIR: ${HSQUICKLOOK_INC_DIR}")
  message("-- HSQUICKLOOK_LIB: ${HSQUICKLOOK_LIB}")

  find_package(mongocxx REQUIRED)
  set(MONGODB_LIB mongo::mongocxx_shared)
  message("-- MONGODB_LIB: ${MONGODB_LIB}")
endif(GB_USE_HSQUICKLOOK)

### ROOT ###
if(GB_USE_ROOT)
  set(ROOTSYS $ENV{ROOTSYS})
  list(APPEND CMAKE_PREFIX_PATH ${ROOTSYS})
  find_package(ROOT REQUIRED)
  set(ROOT_INC_DIR ${ROOT_INCLUDE_DIRS})
  set(ROOT_LIB_DIR ${ROOT_LIBRARY_DIR})
  set(ROOT_LIB ${ROOT_LIBRARIES})
  list(APPEND CMAKE_INSTALL_RPATH ${ROOT_LIBRARY_DIR})
  message("-- ROOTSYS = ${ROOTSYS}")
  message("-- ROOT_INC_DIR = ${ROOT_INC_DIR}")
  message("-- ROOT_LIB_DIR = ${ROOT_LIB_DIR}")
  message("-- ROOT libraries = ${ROOT_LIB}")
endif(GB_USE_ROOT)

### MYSQL ###
if(GB_USE_MYSQL)
  find_package(mysql-concpp REQUIRED CONFIG)
  set(MYSQL_INC_DIR ${MYSQL_CONCPP_INCLUDE_DIR})
  set(MYSQL_LIB_DIR ${MYSQL_CONCPP_RUNTIME_LIBRARY_DIR})
  set(MYSQL_LIB mysqlcppconnx)
  message(STATUS "MYSQL_LIB_DIR: ${MYSQL_INC_DIR}")
  message(STATUS "MYSQL_LIB_DIR: ${MYSQL_LIB_DIR}")
  message(STATUS "MYSQL_LIB: ${MYSQL_LIB}")
endif(GB_USE_MYSQL)

if(GB_USE_MOSQUITTO)
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(Mosquitto IMPORTED_TARGET libmosquitto REQUIRED)
  set(MOSQUITTO_LIB PkgConfig::Mosquitto)
endif(GB_USE_MOSQUITTO)

### subdirectories
add_subdirectory(source)
if(USE_RUBY)
  add_subdirectory(rubyext)
endif(USE_RUBY)
