#ifndef ErrorManager_H
#define ErrorManager_H 1


#include <iostream>
#include <vector>
#include <string>
#include "magic_enum.hpp"

/**
 * A class to handle software error
 *
 * @author Tsubasa Tamba,
 * @date 2023-04-27
 */

namespace gramsballoon {

enum class ErrorType {
  RECEIVE_COMMAND_SERIAL_COMMUNICATION_ERROR = 0,
  RECEIVE_COMMAND_SELECT_ERROR = 1,
  RECEIVE_COMMAND_SREAD_ERROR = 2,
  INVALID_COMMAND = 3,
  PIGPIO_START_ERROR = 4,
  PRESS_SERIAL_COMMUNICATION_ERROR = 4,
  SPI_OPEN_ERROR = 5,
  RTD_SERIAL_COMMUNICATION_ERROR = 5,
  RTD_DATA_AQUISITION_ERROR_1 = 6,
  RTD_DATA_AQUISITION_ERROR_2 = 7,
  RTD_DATA_AQUISITION_ERROR_3 = 8,
  RTD_DATA_AQUISITION_ERROR_4 = 9,
  RTD_DATA_AQUISITION_ERROR_5 = 10,
  RTD_DATA_AQUISITION_ERROR_6 = 11,
  ENV_DATA_AQUISITION_ERROR_1 = 11,
  ENV_DATA_AQUISITION_ERROR_2 = 12,
  ENV_DATA_AQUISITION_ERROR_3 = 13,
  ENV_DATA_AQUISITION_ERROR_4 = 14,
  ENV_DATA_AQUISITION_ERROR_5 = 15,
  PRESS_DATA_AQUISITION_ERROR_JP = 12,
  PRESS_DATA_AQUISITION_ERROR_CP = 13,
  COMP_TEMP_DATA_ERROR = 14,
  COMP_PRESS_DATA_ERROR = 15,
  ACCEL_DEVICE_NOT_FOUND = 16,
  ACCEL_DATA_AQUISITION_ERROR = 17,
  SLOW_ADC_DATA_AQUISITION_ERROR = 18,
  COMP_SERIAL_COMMUNICATION_ERROR = 18,
  GET_SD_CAPACITY_ERROR = 19,
  ANALOG_DISCOVERY_INITIALIZE_ERROR = 20,
  ANALOG_DISCOVERY_NOT_CONNECTED = 21,
  ONLY_ONE_ANALOG_DISCOVERY_CONNECTED = 22,
  TPC_HV_INVALID_CHANNEL = 23,
  TPC_HV_INVALID_VOLTAGE = 24,
  PMT_HV_INVALID_CHANNEL = 25,
  PMT_HV_INVALID_VOLTAGE = 26,
  TRIGGER_SETUP_ERROR = 27,
  TOO_FEW_EVENTS_DETECTED = 28,
  SEND_TELEMETRY_SERIAL_COMMUNICATION_ERROR = 29,
  SEND_TELEMETRY_SWRITE_ERROR = 30,
  SHUTDOWN_REJECTED = 31,
  REBOOT_REJECTED = 32,
  SOFTWARE_STOP_REJECTED = 33,
  MODULE_ACCESS_ERROR = 62,
  OTHER_ERRORS = 63
};

class ErrorManager {
public:
  ErrorManager();
  void resetError();
  void setError(ErrorType v);
  static int strToBit(const std::string &s);
  static std::string bitToStr(int v);

  uint64_t ErrorCode() { return errorCode_; }
  void SetErrorCode(uint64_t v) { errorCode_ = v; }

private:
  uint64_t errorCode_ = 0;

public:
  bool ReceiveCommandSerialCommunicationError() const { return errorCode_ & (1LL << static_cast<int>(ErrorType::RECEIVE_COMMAND_SERIAL_COMMUNICATION_ERROR)); }
  bool ReceiveCommandSelectError() const { return errorCode_ & (1LL << static_cast<int>(ErrorType::RECEIVE_COMMAND_SELECT_ERROR)); }
  bool ReceiveCommandSreadError() const { return errorCode_ & (1LL << static_cast<int>(ErrorType::RECEIVE_COMMAND_SREAD_ERROR)); }
  bool InvalidCommand() const { return errorCode_ & (1LL << static_cast<int>(ErrorType::INVALID_COMMAND)); }
  bool CompTempDataError() const { return errorCode_ & (1LL << static_cast<int>(ErrorType::COMP_TEMP_DATA_ERROR)); }
  bool CompPressDataError() const { return errorCode_ & (1LL << static_cast<int>(ErrorType::COMP_PRESS_DATA_ERROR)); }
  bool PressSerialCommunicationError() const { return errorCode_ & (1LL << static_cast<int>(ErrorType::PRESS_SERIAL_COMMUNICATION_ERROR)); }
  bool RtdSerialCommunicationError() const { return errorCode_ & (1LL << static_cast<int>(ErrorType::RTD_SERIAL_COMMUNICATION_ERROR)); }
  bool RtdDataAquisitionError1() const { return errorCode_ & (1LL << static_cast<int>(ErrorType::RTD_DATA_AQUISITION_ERROR_1)); }
  bool RtdDataAquisitionError2() const { return errorCode_ & (1LL << static_cast<int>(ErrorType::RTD_DATA_AQUISITION_ERROR_2)); }
  bool RtdDataAquisitionError3() const { return errorCode_ & (1LL << static_cast<int>(ErrorType::RTD_DATA_AQUISITION_ERROR_3)); }
  bool RtdDataAquisitionError4() const { return errorCode_ & (1LL << static_cast<int>(ErrorType::RTD_DATA_AQUISITION_ERROR_4)); }
  bool RtdDataAquisitionError5() const { return errorCode_ & (1LL << static_cast<int>(ErrorType::RTD_DATA_AQUISITION_ERROR_5)); }
  bool RtdDataAquisitionError6() const { return errorCode_ & (1LL << static_cast<int>(ErrorType::RTD_DATA_AQUISITION_ERROR_6)); }
  bool PressDataAquisitionErrorJp() const { return errorCode_ & (1LL << static_cast<int>(ErrorType::PRESS_DATA_AQUISITION_ERROR_JP)); }
  bool PressDataAquisitionErrorCp() const { return errorCode_ & (1LL << static_cast<int>(ErrorType::PRESS_DATA_AQUISITION_ERROR_CP)); }
  bool AccelDeviceNotFound() const { return errorCode_ & (1LL << static_cast<int>(ErrorType::ACCEL_DEVICE_NOT_FOUND)); }
  bool AccelDataAquisitionError() const { return errorCode_ & (1LL << static_cast<int>(ErrorType::ACCEL_DATA_AQUISITION_ERROR)); }
  bool SlowAdcDataAquisitionError() const { return errorCode_ & (1LL << static_cast<int>(ErrorType::SLOW_ADC_DATA_AQUISITION_ERROR)); }
  bool CompSerialCommunicationError() const { return errorCode_ & (1LL << static_cast<int>(ErrorType::COMP_SERIAL_COMMUNICATION_ERROR)); }
  bool GetSdCapacityError() const { return errorCode_ & (1LL << static_cast<int>(ErrorType::GET_SD_CAPACITY_ERROR)); }
  bool AnalogDiscoveryInitializeError() const { return errorCode_ & (1LL << static_cast<int>(ErrorType::ANALOG_DISCOVERY_INITIALIZE_ERROR)); }
  bool AnalogDiscoveryNotConnected() const { return errorCode_ & (1LL << static_cast<int>(ErrorType::ANALOG_DISCOVERY_NOT_CONNECTED)); }
  bool OnlyOneAnalogDiscoveryConnected() const { return errorCode_ & (1LL << static_cast<int>(ErrorType::ONLY_ONE_ANALOG_DISCOVERY_CONNECTED)); }
  bool TpcHvInvalidChannel() const { return errorCode_ & (1LL << static_cast<int>(ErrorType::TPC_HV_INVALID_CHANNEL)); }
  bool TpcHvInvalidVoltage() const { return errorCode_ & (1LL << static_cast<int>(ErrorType::TPC_HV_INVALID_VOLTAGE)); }
  bool PmtHvInvalidChannel() const { return errorCode_ & (1LL << static_cast<int>(ErrorType::PMT_HV_INVALID_CHANNEL)); }
  bool PmtHvInvalidVoltage() const { return errorCode_ & (1LL << static_cast<int>(ErrorType::PMT_HV_INVALID_VOLTAGE)); }
  bool TriggerSetupError() const { return errorCode_ & (1LL << static_cast<int>(ErrorType::TRIGGER_SETUP_ERROR)); }
  bool TooFewEventsDetected() const { return errorCode_ & (1LL << static_cast<int>(ErrorType::TOO_FEW_EVENTS_DETECTED)); }
  bool SendTelemetrySerialCommunicationError() const { return errorCode_ & (1LL << static_cast<int>(ErrorType::SEND_TELEMETRY_SERIAL_COMMUNICATION_ERROR)); }
  bool SendTelemetrySwriteError() const { return errorCode_ & (1LL << static_cast<int>(ErrorType::SEND_TELEMETRY_SWRITE_ERROR)); }
  bool ShutdownRejected() const { return errorCode_ & (1LL << static_cast<int>(ErrorType::SHUTDOWN_REJECTED)); }
  bool RebootRejected() const { return errorCode_ & (1LL << static_cast<int>(ErrorType::REBOOT_REJECTED)); }
  bool SoftwareStopRejected() const { return errorCode_ & (1LL << static_cast<int>(ErrorType::SOFTWARE_STOP_REJECTED)); }
  bool ModuleAccessError() const { return errorCode_ & (1LL << static_cast<int>(ErrorType::MODULE_ACCESS_ERROR)); }
  bool OtherErrors() const { return errorCode_ & (1LL << static_cast<int>(ErrorType::OTHER_ERRORS)); }
};

} /* namespace gramsballoon */

#endif /* ErrorManager_H */
