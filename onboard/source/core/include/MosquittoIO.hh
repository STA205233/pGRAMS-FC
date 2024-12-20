#ifndef GB_MosquittoIO_hh
#define GB_MosquittoIO_hh 1
#include "mosquittopp.h"
#include <string>
#include <vector>

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
  int Publish(const std::string &message, const std::string &topic);
  void on_connect(int rc) override;
  void on_disconnect(int rc) override;
  void on_publish(int mid) override;
  void on_message(const struct mosquitto_message *message) override;
  void on_subscribe(int mid, int qos_count, const int *granted_qos) override;
  void on_unsubscribe(int mid) override;
  const std::vector<std::string> &getMessage() const { return message_; }
  std::string getHost() const { return host_; }
  const std::vector<std::string> &getTopicSub() const { return topicSub_; }
  const std::vector<std::string> &getTopicPub() const { return topicPub_; }
  int getPort() const { return port_; }

private:
  std::vector<std::string> message_;
  std::string host_;
  std::vector<std::string> topicPub_;
  std::vector<std::string> topicSub_;

  int port_;
  int keepAlive_;
};
} // namespace gramsballoon::pgrams

#endif //GB_MosquittoIO_hh