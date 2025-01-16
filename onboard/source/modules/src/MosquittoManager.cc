#include "MosquittoManager.hh"
using namespace anlnext;
namespace gramsballoon::pgrams {
ANLStatus MosquittoManager::mod_define() {
  define_parameter("host", &mod_class::host_);
  define_parameter("port", &mod_class::port_);
  define_parameter("keep_alive", &mod_class::keepAlive_);
  define_parameter("threaded_set", &mod_class::threadedSet_);
  define_parameter("user", &mod_class::user_);
  define_parameter("password", &mod_class::passwd_);
  define_parameter("chatter", &mod_class::chatter_);
  return AS_OK;
}
ANLStatus MosquittoManager::mod_pre_initialize() {
  if (host_.empty()) {
    std::cerr << "Error in MosquittoManager::mod_pre_initialize: host is empty." << std::endl;
    return AS_ERROR;
  }
  if (port_ < 0) {
    std::cerr << "Error in MosquittoManager::mod_pre_initialize: port is negative." << std::endl;
    return AS_ERROR;
  }
  if (keepAlive_ < 0) {
    std::cerr << "Error in MosquittoManager::mod_pre_initialize: keep_alive is negative." << std::endl;
    return AS_ERROR;
  }
  if (user_.empty() && !passwd_.empty()) {
    std::cerr << "Error in MosquittoManager::mod_pre_initialize: passwd is set but user is empty." << std::endl;
    return AS_ERROR;
  }
  mosquittoIO_ = std::make_shared<MosquittoIO<std::vector<uint8_t>>>("MosquittoManager", host_, port_, keepAlive_, threadedSet_);
  mosquittoIO_->setVerbose(chatter_);
  return AS_OK;
}
ANLStatus MosquittoManager::mod_initialize() {
  if (!mosquittoIO_) {
    return AS_ERROR;
  }
  if (!user_.empty()) {
    mosquittoIO_->username_pw_set(user_.c_str(), passwd_.c_str());
  }
  HandleError(mosquittoIO_->Connect());
  if (exist_module("SendTelemetry")) {
    get_module_NC("SendTelemetry", &sendTelemetry_);
  }
  return AS_OK;
}
ANLStatus MosquittoManager::mod_begin_run() {
  if (!mosquittoIO_) {
    return AS_ERROR;
  }
  for (int i = 0; i < 5; i++) {
    HandleError(mosquittoIO_->loop());
  }
  return AS_OK;
}
ANLStatus MosquittoManager::mod_analyze() {
  if (!mosquittoIO_) {
    return AS_OK;
  }
  return HandleError(mosquittoIO_->loop());
}
ANLStatus MosquittoManager::mod_end_run() {
  if (!mosquittoIO_) {
    return AS_ERROR;
  }
  return HandleError(mosquittoIO_->loop());
}
ANLStatus MosquittoManager::mod_finalize() {
  if (!mosquittoIO_) {
    return AS_ERROR;
  }
  return HandleError(mosquittoIO_->Disconnect());
}
ANLStatus MosquittoManager::HandleError(int error_code) {
  if (error_code != 0) {
    std::cerr << "Error in ReceiveCommand::mod_initialize: Connecting MQTT failed. Error Message: " << mosqpp::strerror(error_code) << std::endl;
    if (sendTelemetry_) {
      sendTelemetry_->getErrorManager()->setError(ErrorType::RECEIVE_COMMAND_SERIAL_COMMUNICATION_ERROR);
    }
    return AS_ERROR;
  }
  return AS_OK;
}
} // namespace gramsballoon::pgrams