//#include <iomanip>

#include "bytes.h"

Bytes::Bytes(const Bytes& other) : len_(other.len_), pos_(0) {
  set_data(other.data_, other.len_);
};

Bytes::Bytes(const char *str) : len_(strlen(str)), pos_(0) {
  set_data((char *) str, len_);
};

Bytes::Bytes(const char *data, ssize_t len) : len_(len), pos_(0) {
  set_data((char *) data, len);
};

ssize_t Bytes::len() const {
  return len_;
};

void Bytes::set_data(const char *data, ssize_t len) {
  if(data != NULL && len > 0) {
    data_ = new char[len];
    memcpy(data_, data, len);
  }
};

const char* Bytes::data() const {
  return data_;
};

Bytes::~Bytes() {
  delete[] data_;
};

Bytes Bytes::LongIntToBytes(long int number) {
  return Bytes((char *) &number, sizeof(long int));
}

bool operator< (const Bytes &a, const Bytes &b) {
  return (a.len_ != b.len_) ? (a.len_ < b.len_) : (memcmp(a.data_, b.data_, a.len_) < 0);
};

bool operator> (const Bytes &a, const Bytes &b) {
  return (a.len_ != b.len_) ? (a.len_ > b.len_) : (memcmp(a.data_, b.data_, a.len_) > 0);
};

bool operator== (const Bytes &a, const Bytes &b) {
  return (a.len_ != b.len_) ? false : (memcmp(a.data_, b.data_, a.len_) == 0);
};

std::ostream& operator<<(std::ostream &o, const Bytes &a) {
  o << "BYTES:(" << &a << ") value=";
  for(int i=0;i<a.len_;++i) { o << a.data_[i]; }
  o << ", len=" << a.len_;

  return o;
};
