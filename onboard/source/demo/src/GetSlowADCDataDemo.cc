#include "GetSlowADCData.hh"
using namespace anlnext;
namespace gramsballoon {
ANLStatus GetSlowADCData::mod_define() {
  GBBasicDemoModule::mod_define();
  define_parameter("chip_select", &mod_class::chipSelect_);
  define_parameter("spi_manager_name", &mod_class::SPIManagerName_);
  define_parameter("Va", &mod_class::va_);
  define_parameter("channels", &mod_class::channels_);
  define_parameter("num_trials", &mod_class::numTrials_);
  define_parameter("chatter", &mod_class::chatter_);
  define_parameter("channels_mean", &mod_class::voltageMean_);
  define_parameter("channels_width", &mod_class::voltageWidth_);
  define_parameter("slow_ADC_daq_error", &mod_class::slowADCDaqError_);
  define_parameter("spi_manager_not_found", &mod_class::SPIManagerNotFound_);
  return AS_OK;
}
ANLStatus GetSlowADCData::mod_pre_initialize() {
  const std::string send_telem_md = "SendTelemetry";
  if (exist_module(send_telem_md)) {
    get_module_NC(send_telem_md, &sendTelemetry_);
  }
  if (chipSelect_ < 0 || chipSelect_ >= 27) {
    std::cerr << "Chip select must be non-negative and smaller than 27: CS=" << chipSelect_ << std::endl;
    if (sendTelemetry_) {
      sendTelemetry_->getErrorManager()->setError(ErrorType::OTHER_ERRORS);
    }
  }
  if (SPIManagerNotFound_) {
    std::cerr << "SPI manager does not exist. Module name = " << SPIManagerName_ << std::endl;
    if (sendTelemetry_) {
      sendTelemetry_->getErrorManager()->setError(ErrorType::MODULE_ACCESS_ERROR);
    }
  }
  if (voltageMean_.size() != voltageWidth_.size()) {
    std::cerr << "The number of channels for mean and width must be the same." << std::endl;
    if (sendTelemetry_) {
      sendTelemetry_->getErrorManager()->setError(ErrorType::OTHER_ERRORS);
    }
    return AS_ERROR;
  }
  if (voltageMean_.size() != channels_.size()) {
    std::cerr << "The number of channels for mean and width must be the same as the number of channels." << std::endl;
    if (sendTelemetry_) {
      sendTelemetry_->getErrorManager()->setError(ErrorType::OTHER_ERRORS);
    }
    return AS_ERROR;
  }
  if (numTrials_ < 2) {
    numTrials_ = 2;
  }
  std::sort(channels_.begin(), channels_.end());
  channels_.erase(std::unique(channels_.begin(), channels_.end()), channels_.end());
  return AS_OK;
}
ANLStatus GetSlowADCData::mod_analyze() {
  for (int channel: channels_) {
    uint16_t adc = 0;
    double voltage = 0.0;
    const int status = GetData(channel, adc, voltage);
    if (status) {
      adcList_[channel] = adc;
      voltageList_[channel] = voltage;
    }
    else {
      std::cerr << "Error in GetSlowADCData::mod_analyze. Failed to get data." << std::endl;
      if (sendTelemetry_) {
        sendTelemetry_->getErrorManager()->setError(ErrorType::SLOW_ADC_DATA_AQUISITION_ERROR);
      }
    }
  }
  if (chatter_ >= 1) {
    std::cout << "channel adc" << std::endl;
    for (auto m: adcList_) {
      std::cout << m.first << " " << m.second << std::endl;
    }
    std::cout << "channel voltage" << std::endl;
    for (auto m: voltageList_) {
      std::cout << m.first << " " << m.second << std::endl;
    }
  }
  return AS_OK;
}

double GetSlowADCData::Conversion(uint16_t adc) {
  const double ret = va_ / 4096.0 * (adc - 0.5);
  if (ret > 0.0)
    return ret;
  else
    return 0.0;
}
uint16_t GetSlowADCData::InverseConversion(double voltage) {
  const double ret = 4096.0 / va_ * voltage + 0.5;
  if (ret > 0.0)
    return static_cast<uint16_t>(ret);
  else
    return 0;
}
int GetSlowADCData::GetData(int channel, uint16_t &adc, double &voltage) {
  voltage = SampleFromUniformDistribution() * voltageWidth_[channel] + voltageMean_[channel];
  adc = InverseConversion(voltage);
  return 1;
}
} // namespace gramsballoon