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
  return ReadDataUntilSpecificStr(data, "\n");
}
int EncodedSerialCommunication::ReadDataUntilSpecificStr(std::string &data, const std::string &end) {
  data.clear();
  uint8_t buf;
  int cnt = 0;
  const int sz_end = end.size();
  int cnt_end = 0;
  while (true) {
    if (cnt_end == sz_end) {
      break;
    }
    const int ret = sreadSingle(buf);
    if (ret < 0) {
      return ret;
    }
    data += static_cast<char>(buf);
    cnt++;
    if (cnt_end > 0 && static_cast<char>(buf) != static_cast<char>(end[cnt_end])) {
      cnt_end = 0;
    }
    if (static_cast<char>(buf) == static_cast<char>(end[cnt_end])) {
      cnt_end++;
    }
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