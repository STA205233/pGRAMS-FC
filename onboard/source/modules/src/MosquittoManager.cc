#include "MosquittoIO.hh"
using namespace anlnext;
namespace gramsballoon::pgrams {
ANLStatus MosquittoManager::mod_define() {
  define_parameter("host", &mod_class::host_);
  define_parameter("port", &mod_class::port_);
  define_parameter("topicPub", &mod_class::topicPub_);
  define_parameter("topicSub", &mod_class::topicSub_);
  define_parameter("qos", &mod_class::qos_);
  define_parameter("chatter", &mod_class::chatter_);
  return AS_OK;
}
ANLStatus MosquittoManager::mod_initialize() {
  mosquittoIO_ = std::make_shared<MosquittoIO>("MosquittoManager", host_, port_);
  mosquittoIO_->Connect();
  return AS_OK;
}
} // namespace gramsballoon::pgrams