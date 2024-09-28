#include "PushToMySQL.hh"
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
  mysqlIO_.AddColumn("chamber", "time");
  for (int i = 0; i < 6; i++) {
    mysqlIO_.AddColumn("chamber", (boost::format("rtd%i") % i).str());
  }
  mysqlIO_.AddColumn("chamber", "cp");
  mysqlIO_.AddColumn("chamber", "jp");
  mysqlIO_.AddTable("ground");
  mysqlIO_.AddColumn("ground", "time");
  mysqlIO_.AddColumn("ground", "compressT1");
  mysqlIO_.AddColumn("ground", "compressT2");
  mysqlIO_.AddColumn("ground", "compressT3");
  mysqlIO_.AddColumn("ground", "RP");
  return AS_OK;
}
ANLStatus PushToMySQL::mod_analyze() {
  if (chatter_ > 0) {
    std::cout << "PushToMySQL::mod_analyze" << std::endl;
  }
  if (!interpreter_) {
    return AS_OK;
  }
  TelemetryDefinition *telemdef = interpreter_->Telemdef();
  if (!telemdef) {
    return AS_OK;
  }
  std::vector<uint16_t> &chamber_temperature = telemdef->ChamberTemperature();
  uint16_t valve_temperature = telemdef->ValveTemperature();
  uint16_t outer_temperature = telemdef->OuterTemperature();
  int n = chamber_temperature.size();
  //TODO: This conversion should be reconsidered.
  for (int i = 0; i < n; i++) {
    mysqlIO_.SetItem("chamber", (boost::format("rtd%i") % i).str(), std::to_string(static_cast<float>(chamber_temperature[i] / 10)));
  }
  mysqlIO_.SetItem("chamber", "rtd3", std::to_string(static_cast<float>(valve_temperature) / 10));
  mysqlIO_.SetItem("chamber", "rtd4", std::to_string(static_cast<float>(outer_temperature) / 10));
  mysqlIO_.SetItem("chamber", "cp", std::to_string(telemdef->TPCHVSetting() * 1e6));
  mysqlIO_.SetItem("chamber", "jp", std::to_string(telemdef->PMTHVSetting() * 1e6));
  const uint16_t chamber_temperature2 = telemdef->TPCHVCurrentMeasure();
  mysqlIO_.SetItem("chamber", "rtd5", std::to_string(static_cast<float>(chamber_temperature2) / 10));
  // FIXME: How to insert NULL
  mysqlIO_.SetItem("chamber", "time", "2024:01:01:00:00:00");
  mysqlIO_.SetItem("ground", "compressT1", std::to_string(telemdef->EnvTemperature(0)));
  mysqlIO_.SetItem("ground", "compressT2", std::to_string(telemdef->EnvTemperature(1)));
  mysqlIO_.SetItem("ground", "compressT3", std::to_string(telemdef->EnvTemperature(2)));
  mysqlIO_.SetItem("ground", "RP", std::to_string(telemdef->EnvPressure(0)));
  // FIXME: How to insert NULL
  mysqlIO_.SetItem("ground", "time", "2024:01:01:00:00:00");
  mysqlIO_.Insert("chamber");
  mysqlIO_.Insert("ground");
  return AS_OK;
}
ANLStatus PushToMySQL::mod_finalize() {
  return AS_OK;
}
} // namespace gramsballoon::pgrams