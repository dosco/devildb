#include "column.h"

Column::Column(const Column& c)
  : name_(c.name_), value_(c.value_), timestamp_(c.timestamp_) { }

Column::Column(const std::string name, const char *value, int value_len, const long int timestamp)
  : name_(name), value_(value, value_len), timestamp_(timestamp) {}

Column::Column(const std::string name, const char *value, const long int timestamp)
  : name_(name), value_(value), timestamp_(timestamp) {}

Column::Column(const std::string name, const Slice &value, const long int timestamp)
  : name_(name), value_(value), timestamp_(timestamp) {}

Column::~Column() {
  //delete name_;
}

const std::string Column::name() const {
  return name_;
}

const Slice Column::value() const {
  return value_;
}

const long int Column::timestamp() const {
  return timestamp_;
}

bool operator< (const Column &a, const Column &b) {
  int i = std::strcmp(a.name_.c_str(), b.name_.c_str());
  return (i == 0) ? a.timestamp_ > b.timestamp_ : (i < 0);
}

bool operator> (const Column &a, const Column &b) {
  int i = std::strcmp(a.name_.c_str(), b.name_.c_str());
  return (i == 0) ? a.timestamp_ < b.timestamp_ : (i > 0);
}

bool operator== (const Column &a, const Column &b) {
  //return std::strcmp(a.name_, b.name_) == 0;
  return a.name_ == b.name_;
}

std::ostream& operator<<(std::ostream &o, const Column &a) {
  o << "COLUMN:(" << &a << ") "<< a.name_ << "=" << a.value_ << ", " << a.timestamp_;
  return o;
}

// Deleted column tombstone

DeletedColumn::DeletedColumn(const std::string name, const long int timestamp)
    : Column(name, 0, 0, timestamp) { }

DeletedColumn::DeletedColumn(const std::string name, const char *value,
  int value_len, const long int timestamp)
  : Column(name, value, value_len, timestamp) { }
