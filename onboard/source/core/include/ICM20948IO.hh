/**
 * A class for ICM20948 handler
 *
 * @author  Shota Arai, Tsubasa Tamba
 * @date 2023-04-04
 */

#ifndef GB_DEMO_MODE
#ifndef ICM20948IO_H
#define ICM20948IO_H 1

#include "ICM20948.h"
#include <string.h>
#include <chrono>
#include <thread>
#include <string>
#include <iostream>

namespace gramsballoon {

class ICM20948IO
{
public:
  ICM20948IO();
  ~ICM20948IO();

  bool initialize();
  void measure();
  void waitForProcess();
  void calibrateGyro();
  
  void setDevicePath(const std::string& s) { devicePath_ = s; }
  const IMUData* getData() const { return data_; }

private:
  ICM20948 icm_;
  ICM20948::Config conf_;
  const IMUData* data_;
  std::string devicePath_ = "";
};

} /* namespace gramsballoon */

#endif /* GB_DEMO_MODE */
#endif /*ICM20948IO_H*/