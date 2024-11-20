#ifndef GRAMSBalloon_MySQLIO_hh
#define GRAMSBalloon_MySQLIO_hh 1
#include "mysqlx/xdevapi.h"
#include <boost/mp11.hpp>
#include <map>
#include <optional>
#include <string>
#define TRY_AND_CATCH_MYSQL_EXCEPTIONS_BEGIN \
  try {
#define TRY_AND_CATCH_MYSQL_EXCEPTIONS_END    \
  }                                           \
  catch (const mysqlx::Error &e) {            \
    std::cerr << "Error: " << e << std::endl; \
  }

namespace gramsballoon::mysql {
struct TableContent {
  const int index;
  std::optional<mysqlx::Value> value;
  TableContent(int idx, const std::string &val) : index(idx), value(val) {}
  void Reset() { value = std::nullopt; }
};
using value_t = mysqlx::Value;
using table_t = std::map<std::string, TableContent>;
class MySQLIO {

public:
  MySQLIO() = default;
  virtual ~MySQLIO() = default;

private:
  std::map<std::string, table_t> tables_;
  std::shared_ptr<mysqlx::Session> session_;
  std::optional<mysqlx::Schema> schema_ = std::nullopt;
  bool checkExist_ = true;

public:
  void Initialize(const std::string &host, const int port, const std::string &user, const std::string &password, const std::string &database);
  void AddTable(const std::string &table_name);
  void AddColumn(const std::string &table_name, const std::string &col_name);
  void SetItem(const std::string &table_name, const std::string &col_name, const mysqlx::Value &value);
  void Insert(const std::string &table_name);
  void SetCheckExist(const bool checkExist) { checkExist_ = checkExist; }
  void PrintTableInfo(const std::string &table_name);
};

} // namespace gramsballoon::mysql
#endif //GRAMSBalloon_MySQLIO_hh