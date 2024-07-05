#include "GetEnvironmentalData.hh"

using namespace anlnext;
namespace gramsballoon {
ANLStatus GetEnvironmentalData::mod_define() {
  GBBasicDemoModule::mod_define();
  define_parameter("chip_select", &mod_class::chipSelect_);
  define_parameter("SPI_manager_name", &mod_class::SPIManagerName_);
  define_parameter("SPI_manager_not_found", &mod_class::SPIManagerNotFound_);
  define_parameter("chatter", &mod_class::chatter_);
  define_parameter("pressure_mean", &mod_class::pressureMean_);
  define_parameter("humidity_mean", &mod_class::humidityMean_);
  define_parameter("temperature_mean", &mod_class::temperatureMean_);
  define_parameter("pressure_width", &mod_class::pressureWidth_);
  define_parameter("humidity_width", &mod_class::humidityWidth_);
  define_parameter("temperature_width", &mod_class::temperatureWidth_);
  define_parameter("rate_of_failure", &mod_class::rateOfFailure_);
  return AS_OK;
}
ANLStatus GetEnvironmentalData::mod_pre_initialize() {
  const std::string send_telem_md = "SendTelemetry";
  if (exist_module(send_telem_md)) {
    get_module_NC(send_telem_md, &sendTelemetry_);
  }
  if (SPIManagerNotFound_) {
    std::cerr << "SPI manager does not exist. Module name = " << SPIManagerName_ << std::endl;
    if (sendTelemetry_) {
      sendTelemetry_->getErrorManager()->setError(ErrorType::MODULE_ACCESS_ERROR);
    }
  }
  return AS_OK;
}
ANLStatus GetEnvironmentalData::mod_analyze() {
  if (chatter_ >= 1) {
    std::cout << "Chip select: " << chipSelect_ << std::endl;
  }
  const int status = GetData();
  if (status != 0) {
    std::cerr << "Failed to get environmental data." << std::endl;
    setDataAquisitionError();
  }
  if (chatter_ >= 1) {
  }
  pressure_ = SampleFromUniformDistribution() * pressureWidth_ + pressureMean_;
  humidity_ = SampleFromUniformDistribution() * humidityWidth_ + humidityMean_;
  temperature_ = SampleFromUniformDistribution() * temperatureWidth_ + temperatureMean_;

  return AS_OK;
}
void GetEnvironmentalData::setDataAquisitionError() {
  if (sendTelemetry_ == nullptr) return;

  const std::string name = module_id();
  if (name == "GetEnvironmentalData_1") {
    sendTelemetry_->getErrorManager()->setError(ErrorType::ENV_DATA_AQUISITION_ERROR_1);
  }
  else if (name == "GetEnvironmentalData_2") {
    sendTelemetry_->getErrorManager()->setError(ErrorType::ENV_DATA_AQUISITION_ERROR_2);
  }
  else if (name == "GetEnvironmentalData_3") {
    sendTelemetry_->getErrorManager()->setError(ErrorType::ENV_DATA_AQUISITION_ERROR_3);
  }
  else if (name == "GetEnvironmentalData_4") {
    sendTelemetry_->getErrorManager()->setError(ErrorType::ENV_DATA_AQUISITION_ERROR_4);
  }
  else if (name == "GetEnvironmentalData_5") {
    sendTelemetry_->getErrorManager()->setError(ErrorType::ENV_DATA_AQUISITION_ERROR_5);
  }
  else {
    sendTelemetry_->getErrorManager()->setError(ErrorType::OTHER_ERRORS);
  }
}
int GetEnvironmentalData::GetData() {
  if (SampleFromUniformDistribution() < rateOfFailure_ - 0.5) {
    return 1;
  }
  else {
    temperature_ = SampleFromUniformDistribution() * temperatureWidth_ + temperatureMean_;
    humidity_ = SampleFromUniformDistribution() * humidityWidth_ + humidityMean_;
    pressure_ = SampleFromUniformDistribution() * pressureWidth_ + pressureMean_;
    return 0;
  }
}
} // namespace gramsballoon