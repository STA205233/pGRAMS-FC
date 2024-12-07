#ifndef GB_MosquittoIO_hh
#define GB_MosquittoIO_hh 1
#include "mosquittopp.h"
#include <string>

namespace gramsballoon::pgrams {
class MosquittoIO: public mosqpp::mosquittopp {
public:
  MosquittoIO(const char *id, const std::string &host, int port) : mosquittopp(id) {
    mosqpp::lib_init();
    host_ = host;
    port_ = port;
    keepAlive_ = 60;
  }
  virtual ~MosquittoIO() = default;
  void Connect();
  int Read();
  int Write(const std::string &message, const std::string &topic);
  void on_connect(int rc) override;
  void on_disconnect(int rc) override;
  void on_publish(int mid) override;
  void on_message(const struct mosquitto_message *message) override;
  void on_subscribe(int mid, int qos_count, const int *granted_qos) override;
  void on_unsubscribe(int mid) override;
  std::string getMessage() const { return message_; }
  std::string getHost() const { return host_; }
  std::string getTopic() const { return topic_; }
  int getPort() const { return port_; }

private:
  std::string message_;
  std::string host_;
  std::string topic_;
  int port_;
  int keepAlive_;
};
} // namespace gramsballoon::pgrams

#endif //GB_MosquittoIO_hh