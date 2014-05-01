#ifndef BORG_COLUMNS_H_
#define BORG_COLUMNS_H_

#include <boost/intrusive/avl_set.hpp>
#include "column.h"

class Columns {
  friend std::ostream& operator<<(std::ostream &o, const Columns &a);

  public:
  typedef member_hook<Column, avl_set_member_hook<>, &Column::member_hook_> MemberOption;
  typedef avltree<Column, MemberOption> ColumnsTree;
  //typedef ColumnsTree::const_reverse_iterator::value_type ConstColumnFamily;
  typedef ColumnsTree::const_iterator const_iterator;

  Columns();

  void AddColumn(Column &column);
  void DeleteColumn(DeletedColumn &column);
  Column* GetColumn(const char *column_name);

  const_iterator begin() const;
  const_iterator end() const;

  size_t count() const;

  private:
  ColumnsTree columns_;
  size_t column_count_;

};
#endif