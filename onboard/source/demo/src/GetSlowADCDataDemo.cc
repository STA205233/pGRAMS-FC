#include "GetSlowADCDataDemo.hh"
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
  define_parameter("main_voltage_channel", &mod_class::mainVoltageChennal_);
  define_parameter("main_current_channel", &mod_class::mainCurrentChannel_);
  define_parameter("chamber_pressure_channel", &mod_class::chamberPressureChannel_);
  define_parameter("tpc_hv_voltage_channel", &mod_class::tpcHVVoltageChannel_);
  define_parameter("tpc_hv_current_channel", &mod_class::tpcHVCurrentChannel_);
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
  if (channel == mainVoltageChennal_) {
    const double voltage_main = SampleFromUniformDistribution() * voltageWidth_[channel] + voltageMean_[channel];
    voltage = MainVoltageInverseConversion(voltage_main);
  }
  else if (channel == mainCurrentChannel_) {
    const double current = SampleFromUniformDistribution() * voltageWidth_[channel] + voltageMean_[channel];
    voltage = CurrentInverseConversion(current);
  }
  else if (channel == tpcHVVoltageChannel_) {
    const double voltage_tpc = SampleFromUniformDistribution() * voltageWidth_[channel] + voltageMean_[channel];
    voltage = TPCHVVoltageInverseConversion(voltage_tpc);
  }
  else if (channel == tpcHVCurrentChannel_) {
    const double current_tpc = SampleFromUniformDistribution() * voltageWidth_[channel] + voltageMean_[channel];
    voltage = TPCHVCurrentInverseConversion(current_tpc);
  }
  else if (channel == chamberPressureChannel_) {
    const double chamber_pressure = SampleFromUniformDistribution() * voltageWidth_[channel] + voltageMean_[channel];
    voltage = ChamberPressureInverseConversion(chamber_pressure);
  }
  else {
    voltage = SampleFromUniformDistribution() * voltageWidth_[channel] + voltageMean_[channel];
  }
  adc = InverseConversion(voltage);

  return 1;
}
double GetSlowADCData::CurrentInverseConversion(double current) const {
  const double voltage = current / 1.25 + 1;
  return voltage;
}
double GetSlowADCData::CurrentConversion(double voltage) const {
  const double ret = (voltage - 1) * 1.25;
  if (ret > 0.0)
    return ret;
  else
    return 0.0;
}
double GetSlowADCData::MainVoltageInverseConversion(double voltage) const {
  const double vol_adc = voltage / 24. * 3.34;
  return vol_adc;
}
double GetSlowADCData::MainVoltageConversion(double voltage_main) const {
  const double ret = voltage_main / 3.34 * 24.;
  return ret;
}
double GetSlowADCData::TPCHVVoltageInverseConversion(double voltage) const {
  const double vol_adc = voltage / 4;
  return vol_adc;
}
double GetSlowADCData::TPCHVVoltageConversion(double voltage_adc) const {
  const double ret = voltage_adc * 4;
  return ret;
}
double GetSlowADCData::TPCHVCurrentConversion(double voltage_adc) const {
  const double current = voltage_adc / 4 * 200;
  return current;
}
double GetSlowADCData::TPCHVCurrentInverseConversion(double current) const {
  const double voltage_adc = current * 4 / 200;
  return voltage_adc;
}
double GetSlowADCData::ChamberPressureConversion(double voltage_adc) const {
  constexpr double rShunt = 100;
  const double pressure = (voltage_adc / rShunt * 1000 - 4) * 2 / 16;
  return pressure;
}
double GetSlowADCData::ChamberPressureInverseConversion(double pressure) const {
  constexpr double rShunt = 100;
  const double voltage_adc = (pressure * 8 + 4) / 1000 * rShunt;
  return voltage_adc;
}
} // namespace gramsballoon