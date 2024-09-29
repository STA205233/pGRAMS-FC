#include "MySQLIO.hh"
#include <iostream>
namespace gramsballoon::mysql {

void MySQLIO::AddTable(const std::string &table_name) {
  if (!schema_) {
    std::cerr << "Schema is not initialized" << std::endl;
    return;
  }
  const auto it = tables_.find(table_name);
  if (it != tables_.end()) {
    std::cerr << "Table (" << table_name << ") is already resistered to gramsballoon::mysql::MySQLIO" << std::endl;
    return;
  }
  tables_[table_name] = table_t();
}
void MySQLIO::Insert(const std::string &table_name) {
  if (!schema_) {
    std::cerr << "Schema is not initialized" << std::endl;
    return;
  }
  TRY_AND_CATCH_MYSQL_EXCEPTIONS_BEGIN;
  auto table_insert = schema_->getTable(table_name, checkExist_).insert();
  std::vector<mysqlx::Value> cols;
  const int n = tables_[table_name].size();
  cols.resize(n);
  for (auto &col: tables_[table_name]) {
    if (col.second.value == std::nullopt) {
      cols[col.second.index] = mysqlx::nullvalue;
    }
    else {
      cols[col.second.index] = col.second.value.value();
    }
  }
  table_insert.values(cols);
  table_insert.execute();
  TRY_AND_CATCH_MYSQL_EXCEPTIONS_END;
  for (auto &col: tables_[table_name]) {
    col.second.Reset();
  }
}
void MySQLIO::Initialize(const std::string &host, const int port, const std::string &user, const std::string &password, const std::string &database) {
  TRY_AND_CATCH_MYSQL_EXCEPTIONS_BEGIN;
  session_ = std::make_shared<mysqlx::Session>(host, port, user, password);
  schema_ = session_->getSchema(database, checkExist_);
  TRY_AND_CATCH_MYSQL_EXCEPTIONS_END;
}
void MySQLIO::AddColumn(const std::string &table_name, const std::string &col_name) {
  const auto it = tables_.find(table_name);
  if (it == tables_.end()) {
    std::cerr << "Table (" << table_name << ") must be resisgered to gramsballoon::mysql::";
    return;
  }
  const auto it2 = it->second.find(col_name);
  if (it2 != it->second.end()) {
    std::cerr << col_name << "is already resisgered in Table(" << table_name << ")" << std::endl;
    return;
  }
  const int sz = it->second.size();
  it->second.insert(std::make_pair(col_name, TableContent(sz, "")));
}
void MySQLIO::SetItem(const std::string &table_name, const std::string &col_name, const std::string &value) {
  const auto it = tables_.find(table_name);
  if (it == tables_.end()) {
    std::cerr << "Table (" << table_name << ") is not resistered to gramsballoon::mysql::MySQLIO" << std::endl;
    return;
  }
  const auto it2 = it->second.find(col_name);
  if (it2 == it->second.end()) {
    std::cerr << "Table (" << table_name << ") doesn't have column (" << col_name << ")." << std::endl;
    return;
  }
  it2->second.value = value;
}
} // namespace gramsballoon::mysql