#include "TelemetryDefinition.hh"
#include "BinaryFileManipulater.hh"
#include <chrono>
#include <fstream>
#include <thread>

namespace gramsballoon {

TelemetryDefinition::TelemetryDefinition() {
  chamberTemperature_.resize(3);
  chamberPressureNEU_.resize(5);
  jacketPressureNEU_.resize(5);
  compressorTemperature_.resize(4);
  compressorPressure_.resize(2);
  envTemperature_.resize(5);
  envHumidity_.resize(5);
  envPressure_.resize(5);
  acceleration_.resize(3);
  gyro_.resize(3);
  magnet_.resize(3);
  ADCOffset_.resize(4);
  ADCRange_.resize(4);
}

void TelemetryDefinition::generateTelemetry(int wf_division_id /*=0*/) {
  clear();
  startCode_ = 0xEB905B6A;
  addValue<uint32_t>(startCode_);
  addValue<uint16_t>(static_cast<uint16_t>(telemetryType_));
  gettimeofday(&timeNow_, NULL);
  addValue<int32_t>(static_cast<int32_t>(timeNow_.tv_sec));
  addValue<int32_t>(static_cast<int32_t>(timeNow_.tv_usec));
  addValue<uint32_t>(telemetryIndex_);
  addValue<int32_t>(runID_);
  telemetryIndex_++;

  if (telemetryType_ == static_cast<int>(TelemetryType::HK)) {
    generateTelemetryHK();
  }
  else if (telemetryType_ == static_cast<int>(TelemetryType::WF)) {
    generateTelemetryWF(wf_division_id);
  }
  else if (telemetryType_ == static_cast<int>(TelemetryType::Status)) {
    generateTelemetryStatus();
  }
  else {
    std::cerr << "telemetry type not set appropriately: telemetry_type = " << telemetryType_ << std::endl;
    return;
  }
  writeCRC16();
  stopCode_ = 0xC5A4D279;
  addValue<uint32_t>(stopCode_);
}

void TelemetryDefinition::generateTelemetryHK() {
  addValue<int32_t>(static_cast<int32_t>(chamberPressureNEU_[0] / 1E-6));
  addValue<int32_t>(static_cast<int32_t>(jacketPressureNEU_[0] / 1E-6));
  addValue<uint16_t>(chamberPressure_);

  writeRTDTemperature();
  addValue<int32_t>(static_cast<int32_t>(chamberPressureNEU_[1] / 1E-6));
  addValue<int16_t>(TPCHVMeasure_);
  addValue<int32_t>(static_cast<int32_t>(jacketPressureNEU_[1] / 1E-6));
  addValue<int16_t>(TPCHVCurrentMeasure_);
  addValue<int16_t>(static_cast<int16_t>(CPUTemperature_ / 0.1));
  writeEnvironmentalData();
  writeAccelerationData();
  addValue<int16_t>(mainCurrent_); //main current
  addValue<int16_t>(mainVoltage_); //main voltage
  addValue<uint32_t>(lastCommandIndex_); // last command index
  addValue<uint16_t>(lastCommandCode_); // last command code
  addValue<uint16_t>(commandRejectCount_); //command reject count
  addValue<uint64_t>(softwareErrorCode_); //software error code
}

void TelemetryDefinition::generateTelemetryWF(int wf_division_id) {
  if (wf_division_id < 0 || wf_division_id >= 8) {
    std::cerr << "wf_division_id incorrect" << std::endl;
    return;
  }
  addValue<uint32_t>(eventID_);
  for (int i = 0; i < 5; i++) {
    if (i == 0) continue;
    addValue<int16_t>(eventHeader_[i]);
  }
  const int channel = wf_division_id / 2;
  const int division_id = wf_division_id % 2;
  const int num_sampling = eventData_[channel].size();
  addValue<uint16_t>(static_cast<uint16_t>(channel));
  addValue<uint16_t>(static_cast<uint16_t>(division_id));
  addValue<uint32_t>(static_cast<uint32_t>(sampleFrequency_ / 1E-3));
  addValue<uint32_t>(static_cast<uint32_t>(timeWindow_ / 1E-3));
  addValue<uint32_t>(static_cast<uint32_t>(num_sampling));

  if (division_id == 0) {
    for (int i = 0; i < num_sampling / 2; i++) {
      addValue<int16_t>(eventData_[channel][i]);
    }
  }
  else {
    for (int i = num_sampling / 2; i < num_sampling; i++) {
      addValue<int16_t>(eventData_[channel][i]);
    }
  }
}

void TelemetryDefinition::generateTelemetryStatus() {
  addValue<uint16_t>(triggerMode_);
  addValue<uint16_t>(triggerDevice_);
  addValue<uint16_t>(triggerChannel_);
  addValue<int32_t>(static_cast<int32_t>(triggerLevel_ / 1E-3));
  addValue<int32_t>(static_cast<int32_t>(triggerPosition_ / 1E-3));
  addValue<uint32_t>(static_cast<uint32_t>(sampleFrequency_ / 1E-3));
  addValue<uint32_t>(static_cast<uint32_t>(timeWindow_ / 1E-3));
  const int n_offset = ADCOffset_.size();
  for (int i = 0; i < n_offset; i++) {
    addValue<int32_t>(static_cast<int32_t>(ADCOffset_[i] / 1E-3));
  }
  const int n_range = ADCRange_.size();
  for (int i = 0; i < n_range; i++) {
    addValue<int32_t>(static_cast<int32_t>(ADCRange_[i] / 1E-3));
  }
  addValue<uint16_t>(static_cast<uint16_t>(DAQInProgress_));
  addValue<int32_t>(static_cast<int32_t>(TPCHVUpperLimit_ / 1E-3));
  addValue<int32_t>(static_cast<int32_t>(PMTHVUpperLimit_ / 1E-3));
  addValue<uint64_t>(SDCapacity_);
}

void TelemetryDefinition::writeRTDTemperature() {
  const int buf_size = 5;
  const int n = RTDTemperatureADC_.size();
  std::vector<uint16_t> temperature(buf_size, 0);
  for (int i = 0; i < n; i++) {
    if (i == buf_size) break;
    temperature[i] = RTDTemperatureADC_[i];
  }
  addVector<uint16_t>(temperature);
  for (int i = 0; i < 3; i++) {
    setChamberTemperature(i, temperature[i]);
  }
  setValveTemperature(temperature[3]);
  setOuterTemperature(temperature[4]);
}

void TelemetryDefinition::writeEnvironmentalData() {
  const int sz_comptemp = compressorTemperature_.size();
  for (int i = 0; i < 4; i++) {
    if (sz_comptemp > i) {
      addValue<int16_t>(static_cast<int16_t>(compressorTemperature_[i] / 0.1));
    }
    else {
      addValue<int16_t>(0);
    }
  }
  const int sz_chamberpress = chamberPressureNEU_.size();
  for (int i = 0; i < 3; i++) {
    if (sz_chamberpress > i + 2) {
      addValue<int32_t>(static_cast<int32_t>(chamberPressureNEU_[i + 2]) / 1E-6);
    }
    else {
      addValue<int32_t>(0);
    }
  }
  const int sz_comppress = compressorPressure_.size();
  for (int i = 0; i < 2; i++) {
    if (sz_comppress > i) {
      addValue<uint16_t>(static_cast<uint16_t>(compressorPressure_[i] / 0.1));
    }
    else {
      addValue<uint16_t>(0);
    }
  }
  const int sz_jacketpress = jacketPressureNEU_.size();
  for (int i = 0; i < 3; i++) {
    if (sz_jacketpress < i + 2) {
      addValue<int32_t>(static_cast<int32_t>(jacketPressureNEU_[i + 2] / 1E-6));
    }
    else {
      addValue<int32_t>(0);
    }
  }
}

void TelemetryDefinition::writeAccelerationData() {
  const int buf_size = 3;
  //std::vector<int16_t> accel(buf_size);
  std::vector<int16_t> gyro(buf_size);
  std::vector<int16_t> magnet(buf_size);
  for (int i = 0; i < buf_size; i++) {
    //if (i < static_cast<int>(acceleration_.size())) accel[i] = static_cast<int16_t>(acceleration_[i] / 0.01);
    if (i < static_cast<int>(gyro_.size())) gyro[i] = static_cast<int16_t>(gyro_[i] / 0.01);
    if (i < static_cast<int>(magnet_.size())) magnet[i] = static_cast<int16_t>(magnet_[i] / 0.01);
  }
  //addVector<int16_t>(accel);
  addVector<int16_t>(gyro);
  addVector<int16_t>(magnet);
  addValue<int16_t>(static_cast<int16_t>(accelSensorTemperature_ / 0.1));
}

void TelemetryDefinition::writeCRC16() {
  uint16_t crc = calcCRC16(telemetry_);
  addValue<uint16_t>(crc);
}

bool TelemetryDefinition::setTelemetry(const std::vector<uint8_t> &v) {
  int n = v.size();
  if (n < 10) {
    std::cerr << "Telemetry is too short!!: length = " << n << std::endl;
    return false;
  }

  if (v[0] != 0xEB || v[1] != 0x90 || v[2] != 0x5B || v[3] != 0x6A) {
    std::cerr << "start code incorect" << std::endl;
    return false;
  }
  if (v[n - 4] != 0xC5 || v[n - 3] != 0xA4 || v[n - 2] != 0xD2 || v[n - 1] != 0x79) {
    std::cerr << "stop code incorrect" << std::endl;
    return false;
  }

  telemetry_ = v;

  uint16_t type = getValue<uint16_t>(4);
  const int hk_telemetry_len = 132;
  const int wf_telemetry_len = 8248;
  const int status_telemetry_len = 100;
  if (type == 1) {
    if (n != hk_telemetry_len) {
      std::cerr << "Telemetry HK: Telemetry length is not correct: n = " << n << std::endl;
      return false;
    }
  }
  else if (type == 2) {
    if (n != wf_telemetry_len) {
      std::cerr << "Telemetry Status: Telemetry length is not correct: n = " << n << std::endl;
      std::cerr << "We force to resize the telemetry..." << std::endl;
      telemetry_.resize(wf_telemetry_len);
      numZeroFill_ = wf_telemetry_len - n;
      n = wf_telemetry_len;
    }
  }
  else if (type == 3) {
    if (n != status_telemetry_len) {
      std::cerr << "Telemetry Status: Telemetry length is not correct: n = " << n << std::endl;
      return false;
    }
  }
  else {
    std::cerr << "Invalid telemetry type: type = " << type << std::endl;
  }

  std::vector<uint8_t> telem_without_fotter;
  for (int i = 0; i < n - 6; i++) {
    telem_without_fotter.push_back(telemetry_[i]);
  }

  uint16_t crc_calc = calcCRC16(telem_without_fotter);
  uint16_t crc_attached = getValue<uint16_t>(n - 6);
  if (crc_calc != crc_attached) {
    if (type == 2) {
      std::cerr << "CRC16 is not appropriate.\nBut for now, we process the telemetry as valid one since it is too long." << std::endl;
      std::cerr << "Telemetry is very long: size = " << v.size() << std::endl;
      return true;
    }
    if (type != 2) {
      std::cerr << "Invalid telemetry: CRC16 not appropriate" << std::endl;
      return false;
    }
  }

  return true;
}

void TelemetryDefinition::interpret() {
  startCode_ = getValue<uint32_t>(0);
  telemetryType_ = getValue<uint16_t>(4);
  timeNow_.tv_sec = getValue<int32_t>(6);
  timeNow_.tv_usec = getValue<int32_t>(10);
  telemetryIndex_ = getValue<uint32_t>(14);
  runID_ = getValue<int32_t>(18);
  if (static_cast<int>(telemetryType_) == static_cast<int>(TelemetryType::HK)) {
    interpretHK();
  }
  else if (static_cast<int>(telemetryType_) == static_cast<int>(TelemetryType::WF)) {
    interpretWF();
  }
  else if (static_cast<int>(telemetryType_) == static_cast<int>(TelemetryType::Status)) {
    interpretStatus();
  }
  else {
    std::cerr << "could not interpret telemetry: telemetry_type = " << telemetryType_ << std::endl;
  }
}

void TelemetryDefinition::interpretHK() {
  chamberPressureNEU_.resize(5);
  chamberPressureNEU_[0] = getValue<int32_t>(22);
  jacketPressureNEU_.resize(5);
  jacketPressureNEU_[0] = getValue<int32_t>(26);
  chamberPressure_ = getValue<uint16_t>(30);
  chamberTemperature_.resize(5);
  getVector<uint16_t>(32, 5, chamberTemperature_);
  chamberPressureNEU_[1] = static_cast<float>(getValue<int32_t>(42) * 1E-6);
  TPCHVMeasure_ = getValue<uint16_t>(46);
  jacketPressureNEU_[1] = static_cast<float>(getValue<int32_t>(48)) * 1E-6;
  TPCHVCurrentMeasure_ = getValue<uint16_t>(52);
  CPUTemperature_ = static_cast<double>(getValue<int16_t>(54)) * 0.1;

  envTemperature_.resize(5);
  envHumidity_.resize(5);
  envPressure_.resize(5);
  std::vector<int16_t> comp_temp(4);
  std::vector<int32_t> chamber_press(3);
  std::vector<uint16_t> comp_press(2);
  std::vector<int32_t> jack_press(3);
  getVector<int16_t>(56, 4, comp_temp);
  getVector<int32_t>(64, 3, chamber_press);
  getVector<uint16_t>(76, 2, comp_press);
  getVector<int32_t>(80, 3, jack_press);
  for (int i = 0; i < 4; i++) {
    compressorTemperature_[i] = comp_temp[i] * 0.1;
  }
  for (int i = 0; i < 3; i++) {
    chamberPressureNEU_[i + 2] = static_cast<float>(chamber_press[i] * 1E-6);
  }
  for (int i = 0; i < 2; i++) {
    compressorPressure_[i] = static_cast<float>(comp_press[i] * 0.1);
  }

  for (int i = 0; i < 3; i++) {
    jacketPressureNEU_[i + 2] = static_cast<float>(jack_press[i] * 1E-6);
  }

  gyro_.resize(3);
  magnet_.resize(3);
  std::vector<int16_t> gyr(3);
  std::vector<int16_t> mag(3);
  getVector<int16_t>(92, 3, gyr);
  getVector<int16_t>(98, 3, mag);
  for (int i = 0; i < 3; i++) {
    gyro_[i] = static_cast<float>(gyr[i]) * 0.01;
    magnet_[i] = static_cast<float>(mag[i]) * 0.01;
  }
  accelSensorTemperature_ = static_cast<float>(getValue<int16_t>(104)) * 0.1;

  mainCurrent_ = getValue<int16_t>(106);
  mainVoltage_ = getValue<int16_t>(108);
  lastCommandIndex_ = getValue<uint32_t>(110);
  lastCommandCode_ = getValue<uint16_t>(114);
  commandRejectCount_ = getValue<uint16_t>(116);
  softwareErrorCode_ = getValue<uint64_t>(118);
  crc_ = getValue<uint16_t>(126);
  stopCode_ = getValue<uint32_t>(128);

  const double dt = (timeNow_.tv_sec - prevTime_.tv_sec) + (timeNow_.tv_usec - prevTime_.tv_usec) * 1E-6;
  const int dn = eventCount_ - prevEventCount_;
  eventRate_ = 0.0;
  const double eps = 1E-9;
  if (dt > eps) {
    eventRate_ = dn / dt;
  }
  prevTime_ = timeNow_;
  prevEventCount_ = eventCount_;
}

void TelemetryDefinition::interpretWF() {
  eventID_ = getValue<uint32_t>(22);
  eventTime_.tv_sec = getValue<int32_t>(26);
  eventTime_.tv_usec = getValue<int32_t>(30);
  const int channel = static_cast<int>(getValue<uint16_t>(34));
  const int division_id = static_cast<int>(getValue<uint16_t>(36));
  sampleFrequency_ = getValue<uint32_t>(38) * 1E-3;
  timeWindow_ = getValue<uint32_t>(42) * 1E-3;
  const int num_sampling = static_cast<int>(getValue<uint32_t>(46));

  if (static_cast<int>(eventData_.size()) <= channel) {
    eventData_.resize(channel + 1);
  }
  eventData_[channel].resize(num_sampling);

  int index = 50;
  if (division_id == 0) {
    for (int i = 0; i < num_sampling / 2; i++) {
      if (i + (numZeroFill_ + 1) / 2 < num_sampling / 2) {
        eventData_[channel][i] = getValue<int16_t>(index);
      }
      index += sizeof(int16_t);
    }
  }
  else {
    for (int i = num_sampling / 2; i < num_sampling; i++) {
      if (i + (numZeroFill_ + 1) / 2 < num_sampling) {
        eventData_[channel][i] = getValue<int16_t>(index);
      }
      index += sizeof(int16_t);
    }
  }

  crc_ = getValue<uint16_t>(index);
  stopCode_ = getValue<uint32_t>(index + 2);
  if (channel == 3 && division_id == 1) {
    wfDownloadDone_ = true;
  }
}

void TelemetryDefinition::interpretStatus() {
  triggerMode_ = getValue<uint16_t>(22);
  triggerDevice_ = getValue<uint16_t>(24);
  triggerChannel_ = getValue<uint16_t>(26);
  triggerLevel_ = getValue<int32_t>(28) * 1E-3;
  triggerPosition_ = getValue<int32_t>(32) * 1E-3;
  sampleFrequency_ = getValue<uint32_t>(36) * 1E-3;
  timeWindow_ = getValue<uint32_t>(40) * 1E-3;

  std::vector<int32_t> offset(4);
  std::vector<int32_t> range(4);
  getVector<int32_t>(44, 4, offset);
  getVector<int32_t>(60, 4, range);
  ADCOffset_.resize(4);
  ADCRange_.resize(4);
  for (int i = 0; i < 4; i++) {
    ADCOffset_[i] = static_cast<double>(offset[i]) * 1E-3;
    ADCRange_[i] = static_cast<double>(range[i]) * 1E-3;
  }

  DAQInProgress_ = static_cast<bool>(getValue<uint16_t>(76));
  TPCHVUpperLimit_ = static_cast<double>(getValue<int32_t>(78)) * 1E-3;
  PMTHVUpperLimit_ = static_cast<double>(getValue<int32_t>(82)) * 1E-3;
  SDCapacity_ = getValue<uint64_t>(86);

  crc_ = getValue<uint16_t>(94);
  stopCode_ = getValue<uint32_t>(96);
}

void TelemetryDefinition::clear() {
  telemetry_.clear();
}

void TelemetryDefinition::writeFile(const std::string &filename, bool append) {
  writeVectorToBinaryFile<uint8_t>(filename, append, telemetry_);
}

template <typename T>
void TelemetryDefinition::addValue(T input) {
  const int size = sizeof(T);
  uint64_t v = static_cast<uint64_t>(input);
  for (int i = 0; i < size; i++) {
    const int shift = 8 * (size - 1 - i);
    telemetry_.push_back(static_cast<uint8_t>((v >> shift) & 0xff));
  }
}

template <typename T>
void TelemetryDefinition::addVector(std::vector<T> &input) {
  const int n = input.size();
  for (int i = 0; i < n; i++) {
    addValue(input[i]);
  }
}

template <typename T>
T TelemetryDefinition::getValue(int index) {
  const int n = telemetry_.size();
  const int byte = sizeof(T);
  if (index + byte > n) {
    std::cerr << "TelemetryDefinition::getValue error: out of range" << std::endl;
    std::cerr << "telemetry_.size() = " << n << ", index = " << index << ", byte = " << byte << std::endl;
    return static_cast<T>(0);
  }
  if (byte > 8) {
    std::cerr << "TelemetryDefinition::getValue error: typename error" << std::endl;
    std::cerr << "byte should be equal to or less than 8: byte = " << byte << std::endl;
    return static_cast<T>(0);
  }

  uint64_t v = 0;
  for (int i = 0; i < byte; i++) {
    const int j = index + i;
    const int shift = 8 * (byte - 1 - i);
    v |= (static_cast<uint64_t>(telemetry_[j]) << shift);
  }
  return static_cast<T>(v);
}

template <typename T>
void TelemetryDefinition::getVector(int index, int num, std::vector<T> &vec) {
  const int n = telemetry_.size();
  const int byte = sizeof(T);
  if (index + byte * num > n) {
    std::cerr << "TelemetryDefinition::getVector error: out of range" << std::endl;
    std::cerr << "telemetry_.size() = " << n << ", index = " << index << ", byte = " << byte
              << ", num = " << num << std::endl;
    return;
  }
  if (byte > 8) {
    std::cerr << "TelemetryDefinition::getVector error: typename error" << std::endl;
    std::cerr << "byte should be equal to or less than 8: byte = " << byte << std::endl;
    return;
  }
  vec.clear();
  for (int i = 0; i < num; i++) {
    T v = getValue<T>(index);
    vec.push_back(v);
    index += byte;
  }
}

} /* namespace gramsballoon */
