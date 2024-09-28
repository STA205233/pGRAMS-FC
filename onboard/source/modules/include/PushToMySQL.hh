#ifndef GRAMSBalloon_PushToMySQL_hh
#define GRAMSBalloon_PushToMySQL_hh 1
#include "InterpretTelemetry.hh"
#include "MySQLIO.hh"
#include "anlnext/BasicModule.hh"
#include "mysqlx/xdevapi.h"
#include <string>
#include <vector>

namespace gramsballoon {
class InterpretTelemetry;
namespace pgrams {
class PushToMySQL: public anlnext::BasicModule {
  DEFINE_ANL_MODULE(PushToMySQL, 1.0);
  ENABLE_PARALLEL_RUN();

public:
  PushToMySQL() = default;
  virtual ~PushToMySQL() = default;

protected:
  PushToMySQL(const PushToMySQL &r) = default;

public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_finalize() override;

private:
  int chatter_ = 0;
  InterpretTelemetry *interpreter_ = nullptr;
  std::string password_ = "test";
  std::string database_ = "pgrams";
  std::string user_ = "test";
  int port_ = 33060;
  std::string host_ = "localhost";
  bool checkExist_ = true;
  mysql::MySQLIO mysqlIO_;
};
} // namespace pgrams
} // namespace gramsballoon
#endif //GRAMSBalloon_PushToMySQL_hh