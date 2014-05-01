#include "columns.h"
#include "column.h"

struct ColumnKeyValueComp
{
  inline bool operator()(const Column &c, const std::string name) const {
    //std::cout << &c << " A: " << name << " -- " << c.name() << " " << c.value() << "\n" << std::endl;
    int i = std::strcmp(c.name().c_str(), name.c_str());
    return (i == 0) ? false : i < 0;

  }

  inline bool operator()(const std::string name, const Column &c) const {
    //std::cout << &c << " B: " << name << " -- " << c.name() << " " << c.value() << "\n" << std::endl;
    return std::strcmp(name.c_str(), c.name().c_str()) < 0;
  }
};

Columns::Columns() : column_count_(0) { }

void Columns::AddColumn(Column &column) {
  Column *c = new Column(column);
  columns_.init_node(*c);
  columns_.insert_unique(*c);
  column_count_++;
}

void Columns::DeleteColumn(DeletedColumn &column) {
  DeletedColumn *dc = new DeletedColumn(column);
  columns_.init_node(*dc);
  columns_.insert_unique(*dc);
  column_count_--;
}

Column* Columns::GetColumn(const char *column_name) {
  if(column_name != NULL) {
    ColumnsTree::iterator it = columns_.find(column_name, ColumnKeyValueComp());
    if(it != columns_.end() && typeid(*it) != typeid(DeletedColumn)) {
      return &*it;
    }
  }
  return NULL;
}

Columns::const_iterator Columns::begin() const {
  return columns_.begin();
}

Columns::const_iterator Columns::end() const {
  return columns_.end();
}

size_t Columns::count() const {
  return column_count_;
}


std::ostream& operator<<(std::ostream &o, const Columns &a) {
  o << "COLUMNS:(" << &a << ")" << std::endl;
  for(Columns::const_iterator it = a.columns_.begin(); it != a.columns_.end(); ++it) {
    o << "- " << *it << std::endl;
  }
  return o;
}
