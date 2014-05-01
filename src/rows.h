#ifndef BORG_ROWS_H_
#define BORG_ROWS_H_

#include <map>
#include "columns.h"

class ColumnFamily {
  private:
  const std::string name_;

  public:
  ColumnFamily(const std::string &name);
  const std::string& name() const;
};

class Rows {
  friend std::ostream& operator<<(std::ostream &o, const Rows &a);

  public:
  typedef std::map<const std::string, Columns *> RowMap;
  typedef RowMap::const_iterator const_iterator;

  Rows();
  Rows(const ColumnFamily &cf);

  Columns* AddRow(const std::string rk);
  Columns* GetRow(const std::string rk) const;

  const_iterator begin() const;
  const_iterator end() const;

  const Column* GetColumn(const std::string rk, const char *column_name) const;
  const ColumnFamily& GetColumnFamily() const;
  size_t count() const;

  private:
  ColumnFamily column_family_;
  RowMap rows_;
};

#endif