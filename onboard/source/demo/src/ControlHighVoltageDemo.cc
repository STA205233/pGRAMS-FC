#include "ControlHighVoltage.hh"
using namespace anlnext;
namespace gramsballoon {
ANLStatus ControlHighVoltage::mod_define() {
  GBBasicDemoModule::mod_define();
  define_parameter("device_id", &mod_class::deviceID_);
  define_parameter("channel", &mod_class::channel_);
  define_parameter("sleep", &mod_class::sleep_);
  define_parameter("voltages", &mod_class::voltages_);
  define_parameter("upper_limit_voltage", &mod_class::upperLimitVoltage_);
  define_parameter("invalid_channel_error", &mod_class::invalidChannelError_);
  define_parameter("chatter", &mod_class::chatter_);
  return AS_OK;
}
ANLStatus ControlHighVoltage::mod_analyze() {
  if (invalidChannelError_) {
    setInvalidChannelError();
    return AS_ERROR;
  }
  if (voltageIndex_ < static_cast<int>(voltages_.size())) {
    exec_ = true;
    nextVoltage_ = voltages_[voltageIndex_];
    voltageIndex_++;
    return AS_OK;
  }

  if ((currentVoltage_ != nextVoltage_) && exec_) {
    const std::string msg = "Setting voltage to " + std::to_string(nextVoltage_) + " mV";
    PrintInfo(msg);
    currentVoltage_ = nextVoltage_;
    exec_ = false;
  }
  return AS_OK;
}

bool ControlHighVoltage::setNextVoltage(double v) {
  singleton_self()->exec_ = false;
  if (v < 0.0) {
    return false;
  }
  if (v > singleton_self()->upperLimitVoltage_) {
    return false;
  }
  singleton_self()->nextVoltage_ = v;
  return true;
}
void ControlHighVoltage::setInvalidChannelError() {
  if (sendTelemetry_) {
    const std::string name = module_id();
    if (name == "ControlHighVoltage_TPC") {
      sendTelemetry_->getErrorManager()->setError(ErrorType::TPC_HV_INVALID_CHANNEL);
    }
    else if (name == "ControlHighVoltage_PMT") {
      sendTelemetry_->getErrorManager()->setError(ErrorType::PMT_HV_INVALID_CHANNEL);
    }
    else {
      sendTelemetry_->getErrorManager()->setError(ErrorType::OTHER_ERRORS);
    }
  }
}
} // namespace gramsballoon
