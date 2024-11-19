#include "PushToMySQL.hh"
#include <algorithm>
using namespace anlnext;
namespace gramsballoon::pgrams {
ANLStatus PushToMySQL::mod_define() {
  define_parameter("chatter", &mod_class::chatter_);
  define_parameter("host", &mod_class::host_);
  define_parameter("port", &mod_class::port_);
  define_parameter("user", &mod_class::user_);
  define_parameter("password", &mod_class::password_);
  define_parameter("database", &mod_class::database_);
  define_parameter("check_exist", &mod_class::checkExist_);
  return AS_OK;
}
ANLStatus PushToMySQL::mod_initialize() {
  if (exist_module("InterpretTelemetry")) {
    get_module_NC("InterpretTelemetry", &interpreter_);
  }
  mysqlIO_.SetCheckExist(checkExist_);
  mysqlIO_.Initialize(host_, port_, user_, password_, database_);
  mysqlIO_.AddTable("chamber");
  mysqlIO_.AddColumn("chamber", "id");
  mysqlIO_.AddColumn("chamber", "time");
  for (int i = 0; i < 6; i++) {
    mysqlIO_.AddColumn("chamber", (boost::format("rtd%i") % i).str());
  }
  mysqlIO_.AddColumn("chamber", "cp_PR3");
  mysqlIO_.AddColumn("chamber", "jp_PR3");
  mysqlIO_.AddColumn("chamber", "cp_PR1");
  mysqlIO_.AddColumn("chamber", "cp_PR2");
  mysqlIO_.AddColumn("chamber", "cp_PR4");
  mysqlIO_.AddColumn("chamber", "cp_PR5");
  mysqlIO_.AddColumn("chamber", "jp_PR1");
  mysqlIO_.AddColumn("chamber", "jp_PR2");
  mysqlIO_.AddColumn("chamber", "jp_PR4");
  mysqlIO_.AddColumn("chamber", "jp_PR5");
  mysqlIO_.AddTable("ground");
  mysqlIO_.AddColumn("ground", "id");
  mysqlIO_.AddColumn("ground", "time");
  mysqlIO_.AddColumn("ground", "compressT1");
  mysqlIO_.AddColumn("ground", "compressT2");
  mysqlIO_.AddColumn("ground", "compressT3");
  mysqlIO_.AddColumn("ground", "RP");
  mysqlIO_.AddTable("error");
  mysqlIO_.AddColumn("error", "id");
  mysqlIO_.AddColumn("error", "time");
  mysqlIO_.AddColumn("error", "PRESS_SERIAL_COMMUNICATION_ERROR");
  mysqlIO_.AddColumn("error", "RTD_SERIAL_COMMUNICATION_ERROR");
  mysqlIO_.AddColumn("error", "COMP_SERIAL_COMMUNICATION_ERROR");
  mysqlIO_.AddColumn("error", "OTHER_ERRORS");
  mysqlIO_.AddColumn("error", "RTD_DATA_ACQUISITION_ERROR_1");
  mysqlIO_.AddColumn("error", "RTD_DATA_ACQUISITION_ERROR_2");
  mysqlIO_.AddColumn("error", "RTD_DATA_ACQUISITION_ERROR_3");
  mysqlIO_.AddColumn("error", "RTD_DATA_ACQUISITION_ERROR_4");
  mysqlIO_.AddColumn("error", "RTD_DATA_ACQUISITION_ERROR_5");
  mysqlIO_.AddColumn("error", "RTD_DATA_ACQUISITION_ERROR_6");
  mysqlIO_.AddColumn("error", "PRESS_DATA_ACQUISITION_ERROR_JP");
  mysqlIO_.AddColumn("error", "PRESS_DATA_ACQUISITION_ERROR_CP");
  mysqlIO_.PrintTableInfo("chamber");
  mysqlIO_.PrintTableInfo("ground");
  mysqlIO_.PrintTableInfo("error");
  return AS_OK;
}
ANLStatus PushToMySQL::mod_analyze() {
  if (chatter_ > 0) {
    std::cout << "PushToMySQL::mod_analyze" << std::endl;
  }
  if (!interpreter_) {
    return AS_OK;
  }
  if (interpreter_->CurrentTelemetryType() != 1) {
    return AS_OK;
  }
  TelemetryDefinition *telemdef = interpreter_->Telemdef();
  if (!telemdef) {
    return AS_OK;
  }
  const std::vector<uint16_t> &chamber_temperature = telemdef->ChamberTemperature();
  std::cout << "chamber_temperature: ";
  const int n = chamber_temperature.size();
  for (int i = 0; i < n; i++) {
    std::cout << static_cast<float>(chamber_temperature[i]) / 10 << " ";
  }
  std::cout << std::endl;
  //TODO: This conversion should be reconsidered.
  for (int i = 0; i < n; i++) {
    mysqlIO_.SetItem("chamber", (boost::format("rtd%i") % i).str(), std::to_string(static_cast<float>(chamber_temperature[i] / 10)));
  }
  const std::vector<float> &chamber_pressure = telemdef->ChamberPressureNEU();
  const int n_champress = chamber_pressure.size();
  std::cout << "chamber pressure: ";
  for (int i = 0; i < n_champress; i++) {
    mysqlIO_.SetItem("chamber", (boost::format("cp_PR%i") % (i + 1)).str(), std::to_string(chamber_pressure[i]));
    std::cout << chamber_pressure[i] << " ";
  }
  std::cout << std::endl;
  const std::vector<float> &jacket_pressure = telemdef->JacketPressureNEU();
  const int n_jackpress = jacket_pressure.size();
  std::cout << "jacket pressure: ";
  for (int i = 0; i < n_jackpress; i++) {
    mysqlIO_.SetItem("chamber", (boost::format("jp_PR%i") % (i + 1)).str(), std::to_string(jacket_pressure[i]));
    std::cout << jacket_pressure[i] << " ";
  }
  std::cout << std::endl;
  //// FIXME: How to insert NULL
  //mysqlIO_.SetItem("chamber", "time", "2024:01:01:00:00:00");
  const std::vector<int16_t> &compressor_temperature = telemdef->CompressorTemperature();
  const int n_comptemp = std::min(static_cast<int>(compressor_temperature.size()), 3);
  std::cout << "compressor temperature: ";
  for (int i = 0; i < n_comptemp; i++) {
    mysqlIO_.SetItem("ground", (boost::format("compressT%i") % (i + 1)).str(), std::to_string(compressor_temperature[i]));
    std::cout << compressor_temperature[i] << " ";
  }
  std::cout << std::endl;
  mysqlIO_.SetItem("ground", "RP", std::to_string(telemdef->CompressorPressure()[0]));
  //// FIXME: How to insert NULL
  //mysqlIO_.SetItem("ground", "time", "2024:01:01:00:00:00");

  mysqlIO_.SetItem("error", "PRESS_SERIAL_COMMUNICATION_ERROR", std::to_string(interpreter_->getErrorManager()->ReceiveCommandSerialCommunicationError()));
  mysqlIO_.SetItem("error", "RTD_SERIAL_COMMUNICATION_ERROR", std::to_string(interpreter_->getErrorManager()->RtdSerialCommunicationError()));
  mysqlIO_.SetItem("error", "COMP_SERIAL_COMMUNICATION_ERROR", std::to_string(interpreter_->getErrorManager()->CompSerialCommunicationError()));
  mysqlIO_.SetItem("error", "OTHER_ERRORS", std::to_string(interpreter_->getErrorManager()->OtherErrors()));
  mysqlIO_.SetItem("error", "RTD_DATA_ACQUISITION_ERROR_1", std::to_string(interpreter_->getErrorManager()->RtdDataAquisitionError1()));
  mysqlIO_.SetItem("error", "RTD_DATA_ACQUISITION_ERROR_2", std::to_string(interpreter_->getErrorManager()->RtdDataAquisitionError2()));
  mysqlIO_.SetItem("error", "RTD_DATA_AQUISITION_ERROR_3", std::to_string(interpreter_->getErrorManager()->RtdDataAquisitionError3()));
  mysqlIO_.SetItem("error", "RTD_DATA_ACQUISITION_ERROR_4", std::to_string(interpreter_->getErrorManager()->RtdDataAquisitionError4()));
  mysqlIO_.SetItem("error", "RTD_DATA_ACQUISITION_ERROR_5", std::to_string(interpreter_->getErrorManager()->RtdDataAquisitionError5()));
  mysqlIO_.SetItem("error", "RTD_DATA_ACQUISITION_ERROR_6", std::to_string(interpreter_->getErrorManager()->RtdDataAquisitionError6()));
  mysqlIO_.SetItem("error", "PRESS_DATA_ACQUISITION_ERROR_JP", std::to_string(interpreter_->getErrorManager()->PressDataAquisitionErrorJp()));
  mysqlIO_.SetItem("error", "PRESS_DATA_ACQUISITION_ERROR_CP", std::to_string(interpreter_->getErrorManager()->PressDataAquisitionErrorCp()));

  mysqlIO_.Insert("chamber");
  mysqlIO_.Insert("ground");
  mysqlIO_.Insert("error");
  return AS_OK;
}
ANLStatus PushToMySQL::mod_finalize() {
  return AS_OK;
}
} // namespace gramsballoon::pgrams