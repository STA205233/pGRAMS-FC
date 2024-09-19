#include "EncodedSerialCommunication.hh"
namespace gramsballoon::pgrams {
int EncodedSerialCommunication::ReadData(std::string &data, int length) {
  data.clear();
  std::vector<uint8_t> buf(length);
  const int ret = sread(buf, length);
  std::cout << "ret: " << ret << std::endl;
  if (ret < 0) {
    return ret;
  }
  std::cout << "b: " << ret << std::endl;
  for (const auto &b: buf) {
    data += static_cast<char>(b);
  }
  return ret;
}
int EncodedSerialCommunication::WriteData(const std::string &data) {
  const int sz = data.size();
  std::vector<uint8_t> buf(sz);
  for (int i = 0; i < sz; i++) {
    buf[i] = static_cast<uint8_t>(data[i]);
  }
  const int ret = swrite(buf);
  return ret;
}
} // namespace gramsballoon::pgrams