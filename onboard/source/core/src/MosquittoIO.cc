#include "MosquittoIO.hh"

namespace gramsballoon::pgrams {
MosquittoIO::Connect() {
  connect(host_, port_, keepAlive_);
}
MosquittoIO::on_connect(int rc) {
  if (rc == 0) {
    std::cout << "Connected" << std::endl;
  }
  else {
    std::cout << "Connection failed: error code" << mosqpp::strerror(rc) << std::endl;
  }
}
MosquittoIO::on_disconnect(int rc) {
  if (rc == 0) {
    std::cout << "Disconnected" << std::endl;
  }
  else {
    std::cout << "Disconnection failed: error code" << mosqpp::strerror(rc) << std::endl;
  }
}
MosquittoIO::on_publish(int mid) {
  std::cout << "Published message with id: " << mid << std::endl;
}
MosquittoIO::on_message(const struct mosquitto_message *message) {
  std::cout << "Received message: " << message->payload << std::endl;
}
MosquittoIO::on_subscribe(int mid, int qos_count, const int *granted_qos) {
  std::cout << "Subscribed\ngranted_qos: " << *granted_qos << std::endl;
}
} // namespace gramsballoon::pgrams