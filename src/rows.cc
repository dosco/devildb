#include <fstream>

#include "rows.h"

// Column Family code

ColumnFamily::ColumnFamily(const std::string &name) : name_(name) { }

const std::string& ColumnFamily::name() const {
  return name_;
}

Rows::Rows(const ColumnFamily &cf) : column_family_(cf) { }

Columns* Rows::AddRow(const std::string rk) {
  Columns *c = new Columns();
  rows_.insert(std::make_pair(rk, c));
  return c;
}

Columns* Rows::GetRow(const std::string rk) const {
  RowMap::const_iterator it = rows_.find(rk);
  return (rows_.empty() || it == rows_.end()) ?  NULL : it->second;
}

const Column* Rows::GetColumn(const std::string rk, const char *column_name) const {
  RowMap::const_iterator it = rows_.find(rk);
  if (it != rows_.end()) {
    Columns *columns = it->second;
    return columns->GetColumn(column_name);
  }
  return NULL;
}

const ColumnFamily& Rows::GetColumnFamily() const {
  return column_family_;
}

Rows::const_iterator Rows::begin() const {
  return rows_.begin();
}

Rows::const_iterator Rows::end() const {
  return rows_.end();
}

size_t Rows::count() const {
  return rows_.size();
}

std::ostream& operator<<(std::ostream &o, const Rows &a) {
  o << "ROWS:(" << &a << ")" << std::endl;
  for(Rows::RowMap::const_iterator it = a.rows_.begin(); it != a.rows_.end(); ++it) {
    o << "* " << (*it).first << " -> " << *(*it).second << std::endl;
  }
  return o;
}