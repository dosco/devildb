#ifndef BORG_MEMTABLE_H_
#define BORG_MEMTABLE_H_

#include <unordered_map>
#include <boost/ptr_container/ptr_vector.hpp>

#include "column.h"
#include "columns.h"
#include "rows.h"
#include "commit_log.h"

typedef boost::ptr_vector<Column>::iterator ColumnIterator;
typedef std::string RowKey;
//typedef std::unordered_map<std::string, ColumnFamily *> ColumnFamilyMap;

class MemTable : public Rows {
  private:
  CommitLog commit_log_;

  public:
  MemTable(ColumnFamily column_family);
  void Add(const std::string column_family, const std::string row_key, ColumnIterator begin, ColumnIterator end);
  void Add(const std::string column_family, const std::string row_key, Column &column);
  void Delete(const std::string column_family, const std::string row_key, DeletedColumn &column);

  const Column* Get(const std::string column_family, const std::string row_key, const char *column);
};
#endif
