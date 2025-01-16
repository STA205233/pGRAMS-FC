#include "ReceiveTelemetry.hh"
using namespace anlnext;

namespace gramsballoon {
ANLStatus ReceiveTelemetry::mod_define() {
  define_parameter("topic", &mod_class::subTopic_);
  define_parameter("qos", &mod_class::qos_);
  define_parameter("chatter", &mod_class::chatter_);
  return AS_OK;
}

ANLStatus ReceiveTelemetry::mod_initialize() {
  const std::string mosq_md = "MosquittoManager";
  if (exist_module(mosq_md)) {
    get_module_NC(mosq_md, &mosquittoManager_);
  }
  else {
    std::cerr << "Error in ReceiveTelemetry::mod_initialize: MosquittoManager module not found." << std::endl;
    return AS_ERROR;
  }
  mosq_ = mosquittoManager_->getMosquittoIO();
  if (!mosq_) {
    std::cerr << "MosquittoIO is nullptr" << std::endl;
    return AS_ERROR;
  }
  mosq_->Subscribe(subTopic_, qos_);
  return AS_OK;
}
ANLStatus ReceiveTelemetry::mod_analyze() {
  if (mosq_ == nullptr) {
    return AS_OK;
  }
  const auto &payload = mosq_->getPayload();
  if (payload.empty()) {
    if (chatter_ >= 1) {
      std::cout << "ReceiveTelemetry: No payload" << std::endl;
    }
    return AS_OK;
  }
  if (chatter_ >= 1) {
    std::cout << "ReceiveTelemetry Num_packet:" << payload.size() << std::endl;
  }
  valid_ = false;
  telemetry_.clear();
  constexpr int i = 0;
  const auto &packet = payload[i]->payload;
  const int sz_packet = packet.size();
  if (chatter_ >= 1) {
    std::cout << "size: " << sz_packet << std::endl;
    for (int j = 0; j < sz_packet; j++) {
      std::cout << "telemetry[" << j << "] = " << static_cast<int>(packet[j]) << std::endl;
    }
  }
  if (sz_packet < 3) {
    std::cerr << "Packet size is not correct: sz_packet = " << sz_packet << std::endl;
  }
  else if (packet[0] == 0xEB && packet[1] == 0x90 && packet[2] == 0x5B && packet[3] == 0x6A && packet[sz_packet - 4] == 0xC5 && packet[sz_packet - 3] == 0xA4 && packet[sz_packet - 2] == 0xD2 && packet[sz_packet - 1] == 0x79) {
    for (int j = 0; j < sz_packet; j++) {
      telemetry_.push_back(packet[j]);
    }
    valid_ = true;
  }
  else {
    std::cerr << "Packet header or footer is not correct" << std::endl;
    std::cerr << "header: " << std::hex << static_cast<int>(packet[0]) << " " << static_cast<int>(packet[1]) << " " << static_cast<int>(packet[2]) << " " << static_cast<int>(packet[3]) << std::endl;
    std::cerr << "footer: " << std::hex << static_cast<int>(packet[sz_packet - 4]) << " " << static_cast<int>(packet[sz_packet - 3]) << " " << static_cast<int>(packet[sz_packet - 2]) << " " << static_cast<int>(packet[sz_packet - 1]) << std::endl;
  }
  mosq_->popPayloadFront();
  return AS_OK;
}

ANLStatus ReceiveTelemetry::mod_finalize() {
  return AS_OK;
}

} // namespace gramsballoon
