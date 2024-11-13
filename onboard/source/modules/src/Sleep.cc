#include "Sleep.hh"
#include <chrono>
#include <thread>
using namespace anlnext;

namespace gramsballoon::pgrams {
ANLStatus Sleep::mod_define() {
  define_parameter("sleep_sec", &mod_class::sleep_sec);
  return AS_OK;
}
ANLStatus Sleep::mod_analyze() {
  if (sleep_sec > 0) {
    std::cout << "Sleeping for " << sleep_sec << " seconds" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(sleep_sec));
  }
  return AS_OK;
}
} // namespace gramsballoon::pgrams