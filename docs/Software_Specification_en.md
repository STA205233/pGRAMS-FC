
# pGRAMSFC Software Specification

<link rel="stylesheet", href="./style.css">

## Aims

## big picture

## ANL Modules

### AnalogDiscoveryManager

#### Functions

- Establishes a connection to Analog discovery; other modules attempting to connect to Analog discovery must reference this module.

#### Input Parameters

- None.

#### Specifications

- <b>mod_pre_initialize</b><br>
  Initialize Analog Discovery with <tt>ADIO->initialize()</tt>. Discover connected devices and set handler IDs; other modules that handle Analog Discovery can access this module to obtain a list of handlers.
- <b>mod_finalize</b><br>
  Performs the termination operation of Analog Discovery.

#### Core class

- AnalogDiscoveryIO.cc

### ControlHighVoltage

#### Functions

- Accesses the DAC side of Analog Discovery and outputs an arbitrary voltage. The output voltage controls the HV.
- Since the HV power supply must be handled carefully, the voltage change requires a two-step authentication. In other words, after the target voltage is set, it is necessary to issue a new permission for execution.
- It is envisioned to have two modules, one for TPC and one for PMT.

#### Input Parameters

- <modpar>device_id</modpar> (default: 0)<br>
  Target Analog Discovery device ID.
- <modpar>channel</modpar> (default: 0)<br>
  The channel of the target Analog Discovery, which together with the previous item uniquely determines the DAC pin to output.
- <modpar>sleep</modpar> (default: 500)<br>
  Sleep time after outputting voltage. The unit is ${\rm ms}$.
- <modpar>voltages</modpar> (default: [])<br> <br>
  Enter when you want the DAC output to follow the transition from the beginning without relying on commands. The units are $V$. For example, to raise the output from 0V to 5V by 1V, enter [0.0, 1.0, 2.0, 3.0, 4.0, 5.0].

#### Specifications

- <b>mod_initialize</b><br>
  Access <tt>AnalogDiscoveryManager</tt> module and initialize AnalogOut.
- <b>mod_analyze</b><br>
  If the current output voltage (<tt>currentVoltage_</tt>) and the next output voltage (<tt>nextVoltage_</tt>) are different, <tt>nextVoltage_</tt> is newly output by the DAC. However, if the member variable <tt>exec_</tt> is false, this operation is not performed. Since <tt>exec_</tt> is set back to false after each operation, it must be rewritten to true each time after checking whether the setting is as intended.

#### Core class

- None.

### EncodedSerialCommunicator

#### Functions

- Perform serial communication

#### Input Parameters

- <modpar>filename</modpar> (default: "/dev/ttyACM0")
  Serial file path.
- <modpar>timeout_sec</modpar> (default: 1) <br>
  Time out time in sec. If arduino sends nothing for <modpar>timeout_sec</modpar> + <modpar>timeout_usec</modpar>, this module skip the process.
- <modpar>timeout_usec</modpar> (default: 0) <br>
  See <modpar>timeout_sec</modpar>.
- <modpar>baudrate</modpar> (default: B9600)
  Baudrate of serial communication.
- <modpar>mode</modpar> (default: O_RDWR | O_NONBLOCK)
  Serial mode. See description of that in [ReceiveCommand module](#receivecommand)

#### Specifications

None.

#### Core Class

- EncodedSerialCommunication.cc
  Perform serial communication encoded by UTF-8

#### Relationship with other ANL Modules

None.

### GetArduinoData

#### Functions

- Get ADC data from Arduino.

#### Input Parameters

- <modpar>serial_path</modpar> (default: "/dev/ttyACM0") <br>
  Path for serial communication
- <modpar>num_ch</modpar> (default: 32) <br>
  Number of channels.
- <modpar>timeout_sec</modpar> (default: 1) <br>
  Time out time in sec. If arduino sends nothing for <modpar>timeout_sec</modpar> + <modpar>timeout_usec</modpar>, this module skip the process.
- <modpar>timeout_usec</modpar> (default: 0) <br>
  See <modpar>timeout_sec</modpar>.
- <modpar>sleep_for_msec</modpar> (default: 0) <br>
  Before receiving the data, this module waits for <modpar>sleep_for_msec</modpar>.
- <modpar>baudrate</modpar> (default: 9600) <br>
  Baudrate of serial communication.
- <modpar>mode</modpar> (default: O_RDWR | O_NONBLOCK) <br>
  Serial Communication mode. See description of that in [ReceiveCommand module](#receivecommand).
  
#### Specifications

- <b>mod_initialize</b> <br>
  Initialize the serial communication.
- <b>mod_analyze</b> <br>
  Receive the data from arduino, and extract ADC values using regular expression.
  If it cannot receive a proper text and fails to extract ADC data, ADC value will be set to 0.
  
#### Core Class

- EncodedSerialCommunication.cc <br>
  Perform serial communication encoded in UTF-8.

#### Relationship with other ANL Modules

- MeasureTemperatureWithRTDSensorByArduino <br>
  Converts ADC data into temperature.

### GetEnvironmentalData

#### Functions

- Obtain environmental data (temperature, humidity, and atmospheric pressure) inside the pressure-resistant container using the environmental sensor BME680.

#### Input Parameters

- <modpar>chip_select</modpar> (default: 8)<br>
  Input the chip select of the corresponding pin.
- <modpar>SPI_manager_name</modpar> (default: "SPIManager")<br>
  Enter the name of the corresponding SPIManager module.

#### Specifications

- <b>mod_pre_initialize</b><br>
  Pass the value of <tt>chip_select</tt> to SPIManager.
- <b>mod_initialize</b><br>
  Get GPIO and SPI handlers from SPIManager.
- <b>mod_analyze</b><br>
  Accesses BME680 and measures temperature, humidity, and barometric pressure.

#### Core class

- BME680IO.cc

### GetPressure

#### Functions

- Get ADC data using EncodedSerialCommunicator.
- Extract pressure information with regular expression.

#### Input Parameters

- <modpar>channel</modpar> (default: 0) <br>
  Channel number of pressure meter.
- <modpar>EncodedSerialCommunicator_name</modpar> (default: "EncodedSerialCommunicator") <br>
  Name of EncodedSerialCommunicator. But, normally you should use PressureGaugeManager name.
- <modpar>sleep_for_msec</modpar> (default: 500) <br>
  Sleeping time before taking data.

#### Specifications

- <b>mod_analyze</b><br>
  Get Pressure data using EncodedSerialCommunicator, and extract it.

#### Core Class

#### Relationship with other ANL Modules

### GetRaspiStatus

#### Functions

- Obtains and maintains the status of the Raspberypi. Specifically, the CPU temperature and the overall capacity and remaining capacity of the SD card are acquired.

#### Input Parameters

- <modpar>temperature_filename</modpar> (default: "/sys/class/thermal/thermal_zone0/temp")<br>
  A text file containing the CPU temperature. Basically, it is not changed.
- <modpar>path</modpar> (default: "/")<br> <br>
  Path of the target to check data capacity and remaining data. Basically, do not change.

#### Specifications

- <b>mod_analyze</b><br>
  First, the CPU temperature is read into <tt>temperature_filename</tt>. In this case, the number to be read is the ADC value, which is a number equal to 1000 times the actual temperature (in °C). Next, <tt>getCapacity()</tt> is used to obtain the total disk capacity and free space.

#### Core class

- None.

### GetSlowADCData

#### Functions

- Slow Read data from the ADC (ADC128S102). Operationally, the power supply board voltage and current, chamber pressure, and HV are monitored.

#### Input Parameters

- <modpar>chip_select</modpar> (default: 8)<br>
  Input the chip select of the corresponding pin.
- <modpar>SPI_manager_name</modpar> (default: "SPIManager")<br>
  Enter the name of the corresponding SPIManager module.
- <modpar>Va</modpar> (default: 5.0)<br>
  Enter the voltage to be used for the ADC conversion in floating-point stores; the ADC conversion formula is<br>
  $V_{\rm measure}=V_{\rm A}\times (ADC-0.5)/4096$<br>
  It is represented by <br> $V_{\rm measure
- <modpar>channels</modpar> (default: [0])<br> <br>
  Input an integer array of channels to be measured; the ADC128S102 has 8 channels, so you can input values from 0 to 7.
- <modpar>num_trials</modpar> (default: 2)<br>
  When obtaining the ADC value, the data is taken multiple times and the average is calculated. The number of trials is <tt>num_trials</tt>. However, since the data obtained in the first trial is discarded, the average of num_trials-1 trials is actually taken.

#### Specifications

- <b>mod_pre_initialize</b><br>
  Pass the value of <tt>chip_select</tt> to SPIManager.
- <b>mod_initialize</b><br>
  Get GPIO and SPI handlers from SPIManager. <tt>slowADCIO</tt> to read Va configuration.
- <b>mod_analyze</b><br>
  Get ADC values for channels stored in <tt>channels</tt>.

#### Core class

- SlowADCIO.cc

### InterpretTelemetry

### MeasureAcceleration

#### Functions

- Acceleration, angular velocity, and geomagnetic field are measured using accelerometer ICM20948.

#### Input Parameters

- <modpar>device_path</modpar> (default: "/dev/i2c-1")<br>
  Path of virtual file for I2C communication. Basically, do not change.
- <modpar>calibrate_gyro</modpar> (default: false)<br>
  Input boolean whether or not calibration is performed when measuring angular velocity.

#### Specifications

- <b>mod_initialize</b><br>
  Initialization work for ICM20948 and angular velocity calibration.
- <b>mod_analyze</b><br>
  Access the ICM20948 sensor to acquire acceleration, angular velocity, and geomagnetic data.

#### Core class

- ICM20948IO.cc

### MeasureTemperatureWithRTDSensor

#### Functions

- Using the MAX31865 resistance thermometer, measure the temperature of vacuum-insulated vessels, solenoid valves, and outside air.

#### Input Parameters

- <modpar>chip_select</modpar> (default: 8)<br>
  Input the chip select of the corresponding pin.
- <modpar>SPI_manager_name</modpar> (default: "SPIManager")<br>
  Enter the name of the corresponding SPIManager module.

#### Specifications

- <b>mod_pre_initialize</b><br>
  Pass the value of <tt>chip_select</tt> to SPIManager.
- <b>mod_initialize</b><br>
  Get GPIO and SPI handlers from SPIManager. <tt>max31865io -> setConfigureSingle()</tt> to initialize the thermometer.
- <b>mod_analyze</b><br>
  Access MAX31865 and measure the temperature. In this case, both the actual temperature and the ADC value can be obtained; it is the ADC value that is used for Telemetry.

#### Core class

- MAX31865IO.cc

### MeasureTemperatureWithRTDSensorByArduino

#### Functions

- Convert Arduino ADC data into temperature.

#### Input Parameters

- <modpar>GetArduinoData_name</modpar>: (default: "GetArduinoData") <br>
  The name of GetArduinoData module.
- <modpar>channel</modpar>: (default: 0) <br>
  Channel number of ADC data in the Arduino.
- <modpar>bit</modpar>: (default: 10) <br>
  The resolution of ADC.
- <modpar>offset</modpar>: (default: 0) <br>
  Offset value of temperature.

#### Specifications

- <b>mod_analyze</b> <br>
  Get ADC data from the Arduino and convert them.

#### Core Class
  
  None.

#### Relationship with other ANL Modules

- GetArduinoData <br>
  Get Arduino data from this module.

### MeasureTemperatureWithRTDSensorByMHADC

#### Functions

- Get ADC data from EncodedSerialCommunicator module
- Convert ADC data into temperature.

#### Input Parameters

- <modpar>GetMHADCData_name</modpar> (default: "GetMHADCData") <br>
  Name of GetMHADCData module.
- <modpar>channel</modpar> (default: 0) <br>
  The number of channel.
- <modpar>bit</modpar> (default: 10) <br>
  ADC resolution.
- <modpar>offset</modpar> (default: 0) <br>
  Offset of ADC value for calibration.
  
#### Specifications

- <b>mod_analyze</b><br>
  If <modpar>GetMHADCData</modpar> exists, get data from this module, and convert it into temperature, otherwise temperature is set to 0.
  
#### Core Class

None.

#### Relationship with other ANL Modules

- EncodedSerialCommunicator <br>
  Get ADC data with this module.

### PressureGaugeManager

#### Functions

- Inherited from EncodedSerialCommunicator.
- Provide specific functions for pressure gauge.

#### Input Parameters

Same as [EncodedSerialCommunicator](#encodedserialcommunicator).
  
#### Specifications

None.

#### Core Class

Same as [EncodedSerialCommunicator](#encodedserialcommunicator).

#### Relationship with other ANL Modules

- EncodedSerialCommunicator<br>
  Base class for serial communicaton encoded in UTF-8
  
### PushToMongoDB

### PushToMySQL

#### Functions

- Push telemetry data to MySQL.

#### Input Parameters

- <modpar>host</modpar> (default: "localhost")<br>
  Ip address of the database.
- <modpar>port</modpar> (default: 33060)<br>
  Port of the database.
- <modpar>user</modpar> (default: "root")<br>
  User of the database.
- <modpar>password</modpar> (default: "test")<br>
  Password to access the database.
- <modpar>database</modpar> (default: "pgrams")<br>
  Database name.
- <modpar>check_exist</modpar> (default: true)<br>
  Flag to check the existence of the database and its column.

#### Specifications

- <b>mod_initialize</b> <br>
  This function initialize the MySQL communicataion. If <modpar>check_exist</modpar> is true, it checks the existence of tables and its columns.
- <b>mod_analyze</b><br>
  Collect telemetry data from telemetry definitions, and push them to MySQL.

#### Core Class

- MySQLIO.cc
  Responsible for communication with MySQL server.

#### Relationship with other ANL Modules

- ReceiveTelemetry <br>
  Get the telemetry data from this module.

### ReadWaveform

#### Functions

- Reads and saves waveform data by manipulating Analog Discvovery.
- When waveform data is requested by a command from the ground, it goes to write the most recently acquired waveform to the <tt>SendTelemetry</tt> module (not pass by reference, but copy the value).

#### Input Parameters

- <modpar>ad_manager_name</modpar> (default: "AnalogDiscoveryManager")<br>
  Enter the name of the AnalogDiscoveryManager module. Since it is assumed that a single module manages all Analog Discovery, there is basically no need to change it.
- <modpar>trig_device</modpar> (default: 0)<br>
  Trigger source device, 0 or 1 if two Analog Discovery devices are used.
- <modpar>trig_channel</modpar> (default: 0)<br>
  Trigger source channel, 0 or 1 if two Analog Discovery devices are used. Together with the previous section, this determines the waveform that will be the trigger source.
- <modpar>trig_mode</modpar> (default: 0x02)<br>
  Enter the trigger mode, consisting of 8 bit integers, where the upper 4 bits represent the trigger slope and the lower 4 bits represent the trigger source. 0-255 values can be entered, but there are many invalid values. Trigger source can be 0 (random trigger), 1 (periodic trigger), 2 (self trigger), or 11 (external trigger). The default value is RISE + self trigger.
  For example, if you want to set FALL + external trigger, just enter 0x1B=27.
- <modpar>trig_level</modpar> (default: 0.0)<br>
  Enter the trigger level. The unit is ${\rm V}$.
- <modpar>trig_position</modpar> (default: 0.0)<br>
  Enter the trigger position. The unit is ${\rm \mu s}$.
- <modpar>time_window</modpar> (default: 10.0)<br>
  Enter the time window (time length of the waveform to be acquired). The unit is ${\rm \mu s}$.
- <modpar>sample_frequency</modpar> (default: 2.0)<br>
  Input sample frequency. The unit is MHz. <tt>time_window</tt> $\times$ <tt>sample_frequency</tt> is the total number of samples (=size of waveform data).
- <modpar>adc_range_list</modpar> (default: [1.0, 1.0, 1.0, 1.0])<br>
  Sets the ADC range for each of the 4 channels. The unit is ${\rm V}$. Due to the specification of Analog Discovery, only $5\;{\rm V}$ and $50\;{\rm V}$ are allowed, and the default is 1.0, but the actual value is $5\;{\rm V}$ (actually about $5.5\;{\rm V}$). Basically, it is not changed.
- <modpar>adc_offset_list</modpar> (default: [0.0, 0.0, 0.0, 0.0])<br>
  Input the offset of ADC. The unit is ${\rm V}$. Basically, do not change. <br>
  The conversion formula for ADC is as follows. <br>
  ${\rm Voltage}={\rm ADC}*{\rm range}/65536+{\rm offset}$<br>
  See <https://www.mouser.com/pdfDocs/waveforms_sdk_rm.pdf> for details.
- <modpar>output_filename_base</modpar> (default: "output")<br> <br>
  Base name of the binary file where the waveform data will be stored. Binary files are generated sequentially as "output_000000.dat", "output_000001.dat"... ... and so on.
- <modpar>num_events_per_file</modpar> (default: 100)<br>
  Enter the number of events to be stored in one binary file.
- <modpar>start_reading</modpar> (default: false)<br>
  Enter a boolean to indicate whether or not to start reading the waveform from the beginning.

#### Specifications

- <b>mod_initialize</b><br>
  Establish access to the AnalogDiscoveryManger module and the sendTelemetry module. Then, set up AnalogIn of Analog Discovery. Set the sample frequency, range, and offset with <tt>adio->setupAnalogIn()</tt>, followed by initialization work with <tt>daqio->initialize()</tt>. In addition, trigger setup is done with <tt>daqio->setupTrigger()</tt>.

- <b>mod_analyze</b><br>
  Run <tt>daqio->getData()</tt> for each loop to get data. Save a <tt>numEventsPerFile</tt> binary file for each loop. If a command to change the trigger setup comes from the command side, it will be reflected. If waveform data needs to be sent by telemetry, go to <tt>SendTelemetry</tt> module to write data.

- <b>mod_finalize</b><br>
  Save unsaved data.

#### Core class

- DAQIO.cc

### ReceiveCommand

#### Functions

- Receive commands from the ground via serial communication.
- After interpreting the commands, the appropriate commands are distributed to each module.
- See below for a list of commands:<br>
  <https://docs.google.com/spreadsheets/d/149plbWC4adAmXE9alBa7HCU7ituWycGj6gzw1qqyXyk/edit#gid=2044691227>

#### Input Parameters

- <modpar>baudrate</modpar> (default: B9600)<br>
  Enter the baudrate of the communication. Basically, just enter the number after B as an integer.
- <modpar>serial_path</modpar> (default: "/dev/null")<br>
  Path to the device special file used for serial communication, e.g. "/dev/ttyAMA0" for serial communication with Raspi.
- <modpar>open_mode</modpar> (default: O_RDWR=2)<br>
  Enter the open_mode option for serial communication: READONLY=0, WRITEONLY=1, READWRITE=2, NONBLOCK=4, or 6 if you want READWRITE and NONBLOCK. When receiving commands from Raspi, it is better to specify READWRITE=2.
- <modpar>TPC_HVController_module_name</modpar> (default: "ControlHighVoltage_TPC")<br>
  Name of the ControlHighVoltage module that controls TPC. Basically, do not change.
- <modpar>PMT_HVController_module_name</modpar> (default: "ControlHighVoltage_PMT")<br>
  Name of the ControlHighVoltage module that controls PMT. Basically, do not change.

#### Specifications

- <b>mod_initialize</b><br>
  Establish access to other modules and initialize serial communication.
- <b>mod_analyze</b><br>
  Attempts to receive commands at each loop. If no signal is received via serial communication, it times out after a certain period of time (10 seconds) and moves to the next loop. If a signal comes in, it interprets the string. After confirming that the start code, end code, CRC, and command code are normal, the intended instructions are reflected to the other target modules. If the reception of a command is interrupted midway, it attempts to receive the continuation of the command in the next loop. If multiple commands are received in a sequence in one loop of processing, only the first command is valid. Therefore, when multiple commands are sent from the ground, a certain interval (1 second is sufficient) must be allowed.

#### Core class

- CommandDefinition.cc<br>.
  Contains the interpretation of commands.
- SerialCommunication.cc<br>
  Describes serial communication operations.

### ReceiveTelemetry

#### Functions

- Receive telemetry from Raspi on the ground.
- This module is designed to run on a Mac on the ground.

#### Input Parameters

- <modpar>baudrate</modpar> (default: B9600)<br>
  Enter the baudrate of the communication. Basically, enter the number after B as an integer.
- <modpar>serial_path</modpar> (default: "/dev/null")<br>
  The path to the device special file used for serial communication, e.g. "/dev/tty.usbserial-14110" when receiving on a Mac.
- <modpar>open_mode</modpar> (default: O_RDWR=6)<br>
  Enter the open_mode option for serial communication: READONLY=0, WRITEONLY=1, READWRITE=2, NONBLOCK=4. If you want READWRITE and NONBLOCK, specify 6. When receiving telemetry on a Mac, it is better to specify READWRITE=2.

#### Specifications

- <b>mod_initialize</b><br>
  Initialize the serial communication.
- <b>mod_analyze</b><br>
  Performs telemetry via serial communication. Only the start and end codes are determined; other interpretations are left to the InterpretTelemetry module. If the telemetry reception is interrupted in the middle of a loop, it attempts to receive the continuation of the telemetry in the next loop. If multiple telemetries are received in a sequence in one loop of processing, only the first telemetry is valid.

#### Core class

- SerialCommunication.cc<br>.
  The serial communication operation is described.

### SendTelemetry

#### Functions

- It gathers information from other modules and creates telemetry.
- The telemetry is then transmitted from Raspi to the ground.
- For more information on telemetry, see below. <br>
  <https://docs.google.com/spreadsheets/d/149plbWC4adAmXE9alBa7HCU7ituWycGj6gzw1qqyXyk/edit#gid=32632668>

#### Input Parameters

- <modpar>MeasureTemperature_module_names</modpar> (default: [])<br> <br>
  Insert the module name of MeasureTemperatureWithRTDSensor. There are usually multiple modules, so ["MeasureTemperatureWithRTDSensor_1", "MeasureTemperatureWithRTDSensor_2", ...]. Etc.
- <modpar>TPCHVController_module_name</modpar> (default: "ControlHighVoltage_TPC")<br>
  Module name of ControlHighVoltage for TPC. The basic default value may be left unchanged.
- <modpar>PMTHVController_module_name</modpar> (default: "ControlHighVoltage_PMT")<br>
  Module name of ControlHighVoltage for PMT. The basic default value may be left unchanged.
- <modpar>GetEnvironmentalData_module_names</modpar> (default: [])<br>
  Insert the module name of GetEnvironmentalData. There are usually multiple modules, so ["GetEnvironmentalData_1", "GetEnvironmentalData_2", ...]. and so on.
- <modpar>serial_path</modpar> (default: "/dev/null")<br>
  Path of the device special file used for serial communication, e.g. "/dev/ttyAMA0" for serial communication with Raspi.
- <modpar>baudrate</modpar> (default: B9600)<br>
  Enter the baudrate of the communication. Basically, enter the number after B as an integer.
- <modpar>open_mode</modpar> (default: O_RDWR=2)<br>
  Enter the open_mode option for serial communication: READONLY=0, WRITEONLY=1, READWRITE=2, NONBLOCK=4. If you want READWRITE and NONBLOCK, specify 6. When receiving commands from Raspi, it is better to specify READWRITE=2.

#### Specifications

- <b>mod_initialize</b><br>
  Establish access to other modules and initialize serial communication.
- <b>mod_analyze</b><br>
  Send telemetry. Usually sends HK Telemetry.

#### Core class

- TelemetryDefinition.cc<br>.
  Describes how to generate telemetry.
- SerialCommunication.cc<br>
  Describes the operation of serial communication.

### ShutdownSystem

#### Functions

- Controls shutdown and restart of Raspi.

#### Input Parameters

- None.

#### Specifications

- <b>mod_analyze</b><br>
  Determines whether to perform a shutdown or restart operation and executes it.
  Because these operations have significant impact, a two-step authentication is employed.
  For example, in the case of a shutdown, the boolean variables <tt>prepareShutdown_</tt> and <tt>shutDown_</tt> must both be true to be executed. The same is true in the case of a restart. These variables can be rewritten by sending commands from the ground. (In other words, two different commands must be sent to be executed.)

#### Core class

- None.

### SPIManager

#### Functions

- Establishes SPI communication. Other modules that use SPI communication must refer to this module.

#### Input Parameters

- <modpar>channel</modpar> (default: 0)
  <br>The internal channel for SPI communication. 0 or 1 can be specified, but since it is meaningless in operation, it is basically not necessary to set it.
- <modpar>baudrate</modpar> (default: 100000)
  <modpar>baudrate for SPI communication; basically, the default value can be used.
- <modpar>spi_flags</modpar> (default: 0b1110000)<br>
  Flags to set SPI communication options. 22 bit integer. In particular, the lower bit represents the SPI mode, which is determined by the timing (rise/fall) and polarity of the target device. For example, for environmental data acquisition, the flag must be set to 0b1110000, while for temperature measurement by an RTD sensor, the flag must be set to 0b1110001. In such a situation, two separate SPIManager modules need to be set.
  See <https://abyz.me.uk/rpi/pigpio/pdif2.html#spi_open> for details.

#### Specifications

- <b>mod_initialize</b><br>
  After accessing GPIO with <tt>pigpio_start</tt>, establish SPI communication with <tt>spi_open</tt>. Obtain a handler respectively. The <tt>GPIOHandler</tt> and <tt>SPIHandler</tt> are owned by this module, and other modules refer to them. 8 values for chip_select are owned for convenience, but each module has different values, so they are set again. Initialize all registered <tt>chip_select</tt> with <tt>set_mode</tt>, <tt>set_pull_up_down</tt>.
- <b>mod_finalize</b><br>
  Perform SPI communication termination operations with <tt>spi_close</tt> and <tt>pigpio_stop</tt>.
  