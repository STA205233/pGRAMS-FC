#include "DumpSerial.hh"
#include "DateManager.hh"
#include <fstream>
#include <iostream>
using namespace anlnext;
namespace gramsballoon {
ANLStatus DumpSerial::mod_define() {
  define_parameter("binary_filename_base", &mod_class::binaryFilenameBase_);
  define_parameter("chatter", &mod_class::chatter_);
  return AS_OK;
}
ANLStatus DumpSerial::mod_initialize() {
  timeStampStr_ = getTimeStr();
  const std::string receiverName = "ReceiveTelemetry";
  if (exist_module(receiverName)) {
    get_module(receiverName, &receiveTelemetry_);
  }
  else {
    std::cerr << "DumpSerial: ReceiveTelemetry module not found" << std::endl;
    return AS_ERROR;
  }
  const std::string filename = CreateFilename();
  std::ofstream ofs(filename, std::ios::binary | std::ios::binary);
  if (!ofs) {
    std::cerr << "DumpSerial: cannot open file " << binaryFilenameBase_ << std::endl;
    return AS_ERROR;
  }
  ofs.close();
  return AS_OK;
}
ANLStatus DumpSerial::mod_analyze() {
  if (receiveTelemetry_ == nullptr) {
    return AS_ERROR;
  }
  const std::vector<uint8_t> &telemetry = receiveTelemetry_->Telemetry();
  if (chatter_ >= 1) {
    std::cout << "DumpSerial: telemetry size = " << telemetry.size() << std::endl;
  }
  if (binaryFilenameBase_.empty()) {
    std::cerr << "DumpSerial: output filename is empty" << std::endl;
    return AS_ERROR;
  }
  const std::string filename = CreateFilename();
  std::ofstream ofs(filename, std::ios::binary | std::ios::app);
  if (!ofs) {
    std::cerr << "DumpSerial: cannot open file " << binaryFilenameBase_ << std::endl;
    return AS_ERROR;
  }
  ofs.write(reinterpret_cast<const char *>(telemetry.data()), telemetry.size());
  ofs.flush();
  ofs.close();
  eventIndex_++;
  return AS_OK;
}
std::string DumpSerial::CreateFilename() {
  if (eventIndex_ == numEventsPerFile_) {
    eventIndex_ = 0;
    currentFileID_++;
  }
  std::ostringstream id_sout;
  id_sout << std::setfill('0') << std::right << std::setw(6) << currentFileID_;
  const std::string id_str = id_sout.str();
  const std::string filename = binaryFilenameBase_ + "_" + timeStampStr_ + "_" + id_str + ".dat";
  return filename;
}
} // namespace gramsballoon