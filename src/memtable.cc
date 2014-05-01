#include "memtable.h"
#include "sstable.h"
#include "commit_log.h"
#include "bloomfilter.h"

MemTable::MemTable(ColumnFamily column_family) : Rows(column_family) {
  //SSTable sst;
  commit_log_.Init(*this);
}

void MemTable::Add(const std::string column_family, const std::string row_key,
  ColumnIterator begin, ColumnIterator end) {

  Columns *c = GetRow(row_key);
  if(c == NULL) {
    c = AddRow(row_key);
  }
  for(ColumnIterator it = begin; it != end; ++it) {
    commit_log_.CommitEntry(column_family, row_key, *it);
    c->AddColumn(*it);
  }
  //std::cout << row_count() << std::endl;

  if(count() > 500) {
    //SSTable sst(*this);
  }
}

void MemTable::Add(const std::string column_family, const std::string row_key, Column &column) {
  Columns *c = GetRow(row_key);
  if(c == NULL) {
    c = AddRow(row_key);
  }
  if(c != NULL) {
    commit_log_.CommitEntry(column_family, row_key, column);
    c->AddColumn(column);
  }
}

void MemTable::Delete(const std::string column_family, const std::string row_key, DeletedColumn &column) {
  Columns *c = GetRow(row_key);
  if(c == NULL) {
    c = AddRow(row_key);
  }
  if(c != NULL) {
    commit_log_.CommitEntry(column_family, row_key, column);
    c->DeleteColumn(column);
  }
}

const Column* MemTable::Get(const std::string column_family, const std::string row_key, const char *column) {
  const Column *fc;

  if((fc = GetColumn(row_key, column)) == NULL) {
    return NULL;
  }

  return fc;
}

/*
void MemTable::AddColumnFamily(ColumnFamily *column_family) {
  if (column_family == NULL) {
    cf_map_.insert(std::make_pair(column_family->name(), column_family));
  }
}

ColumnFamily* MemTable::AddColumnFamily(const std::string &column_family_name) {
  ColumnFamily *column_family = new ColumnFamily(column_family_name);
  cf_map_.insert(std::make_pair(column_family_name, column_family));
  return column_family;
}

ColumnFamily* MemTable::GetColumnFamily(const std::string &column_family) const {
  ColumnFamilyMap::const_iterator it = cf_map_.find(column_family);
  return (cf_map_.empty() || it == cf_map_.end()) ? NULL : it->second;
}
*/


/*
void MemTable::Delete(const std::string &column_family, const char *row_key,
  ColumnIterator begin, ColumnIterator end) {

  if(row_key == NULL) {
    return;
  }

  Bytes rk(row_key);
  ColumnFamily *cf;
  if((cf = GetColumnFamily(column_family)) == NULL) {
    return;
  }

  Columns *c = cf->GetRow(row_key);
  if(c == NULL) {
    c = cf->AddRow(row_key);
  }
  for(ColumnIterator it = begin; it != end; ++it) {
    commit_log_.CommitEntry(column_family, rk, *it);
    c->DeleteColumn(*it);
  }
}
*/
