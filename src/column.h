#ifndef BORG_COLUMN_H_
#define BORG_COLUMN_H_

#include <boost/intrusive/avl_set.hpp>
#include "slice.h"

using namespace boost::intrusive;

class Column : public avl_set_base_hook<optimize_size<true> > {
  friend bool operator< (const Column &a, const Column &b);
  friend bool operator> (const Column &a, const Column &b);
  friend bool operator== (const Column &a, const Column &b);
  friend std::ostream& operator<<(std::ostream &o, const Column &a);

  protected:
  const std::string name_;
  const Slice value_;
  const long int timestamp_;

  public:
  avl_set_member_hook<> member_hook_;
  Column(const Column& c);
  Column(const std::string name, const char *value, int value_len, const long int timestamp);
  Column(const std::string name, const char *value, const long int timestamp);
  Column(const std::string name, const Slice &bytes, const long int timestamp);
  virtual ~Column();

  const std::string name() const;
  const Slice value() const;
  const long int timestamp() const;
};

class DeletedColumn : public Column {
  public:
  DeletedColumn(const std::string name, const long int timestamp);
  DeletedColumn(const std::string name, const char *value, int value_len, const long int timestamp);
};


#endif
