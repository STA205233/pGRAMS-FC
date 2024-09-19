#ifndef SerialCommunication_H
#define SerialCommunication_H 1

/**
 * A class of serial communication
 *
 * @author Tsubasa Tamba, Shota Arai
 * @date 2023-03-02
 */

#include "termios.h"
#include "unistd.h"
#include <fcntl.h>
#include <iostream>
#include <memory>
#include <string>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <vector>

#ifdef __APPLE__
#  define TCSETS TIOCSETA
#endif

namespace gramsballoon {

class SerialCommunication {
public:
  SerialCommunication();
  SerialCommunication(const std::string &serial_path, speed_t baudrate, mode_t open_mode);
  ~SerialCommunication();
  int initialize();
  int sreadSingle(uint8_t &buf);
  int sread(std::vector<uint8_t> &buf, int length);
  int swrite(const std::vector<uint8_t> &buf);
  int WaitForTimeOut(timeval timeout);
  void setBaudrate(speed_t v) { baudrate_ = v; }
  void setSerialPath(const std::string &s) { serialPath_ = s; }
  void setOpenMode(mode_t c) { openMode_ = c; }

  int FD() { return fd_; }

private:
  std::unique_ptr<termios> tio_ = nullptr;
  int fd_ = 0;
  speed_t baudrate_ = B9600;
  std::string serialPath_;
  mode_t openMode_;
};

} /* namespace gramsballoon */

#endif /* SerialCommunication_H */
