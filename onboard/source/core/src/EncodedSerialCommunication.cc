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
int EncodedSerialCommunication::ReadDataUntilBreak(std::string &data) {
  data.clear();
  uint8_t buf;
  int cnt = 0;
  while (true) {
    const int ret = sreadSingle(buf);
    if (ret < 0) {
      return ret;
    }
    if (buf == '\n') {
      break;
    }
    data += static_cast<char>(buf);
    cnt++;
  }
  return cnt;
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