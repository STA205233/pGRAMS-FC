#ifndef GB_DEMO_MODE
#include "GetRaspiStatus.hh"
#include <sys/vfs.h>
#include <fstream>

using namespace anlnext;

namespace gramsballoon {

GetRaspiStatus::GetRaspiStatus()
{
  tempFile_ = "/sys/class/thermal/thermal_zone0/temp";
  path_ = "/";
}

GetRaspiStatus::~GetRaspiStatus() = default;

ANLStatus GetRaspiStatus::mod_define()
{
  define_parameter("temperature_filename", &mod_class::tempFile_);
  define_parameter("path", &mod_class::path_);
  define_parameter("chatter", &mod_class::chatter_);

  return AS_OK;
}

ANLStatus GetRaspiStatus::mod_initialize()
{
  const std::string send_telem_md = "SendTelemetry";
  if (exist_module(send_telem_md)) {
    get_module_NC(send_telem_md, &sendTelemetry_);
  }

  return AS_OK;
}

ANLStatus GetRaspiStatus::mod_analyze()
{
  std::ifstream ifstemp(tempFile_);
  ifstemp >> CPUTemperatureADC_;
  CPUTemperature_ = CPUTemperatureADC_ / 1000.0;
  ifstemp.close();

  const int status = getCapacity();
  if (status != 0) {
    std::cerr << "Error in GetRaspiStatus::mod_analyze()" << std::endl;
    if (sendTelemetry_) {
      sendTelemetry_->getErrorManager()->setError(ErrorType::GET_SD_CAPACITY_ERROR);
    }
  }
  
  if (chatter_>=1) {
    uint64_t one = 1;
    std::cout << "Free size (MB): " << capacityFree_ / (one<<20) << std::endl;
    std::cout << "All size (MB): " << capacityAll_ / (one<<20) << std::endl;
  }

  return AS_OK;
}


ANLStatus GetRaspiStatus::mod_finalize()
{
  return AS_OK;
}

int GetRaspiStatus::getCapacity()
{
  struct statfs64 capacity;
  int rslt = statfs64(path_.c_str(), &capacity);
  if (rslt < 0) {
    std::cerr << "failed in get_freesize(). Code: " << rslt << std::endl;
    return -1;
  }
  capacityFree_ = capacity.f_bfree * capacity.f_bsize;
  capacityAll_ = capacity.f_blocks * capacity.f_bsize;
  return 0;
}

} /* namespace gramsballoon */
#endif /* GB_DEMO_MODE */
