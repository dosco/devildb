#ifndef BORG_BYTES_H_
#define BORG_BYTES_H_

#include <fstream>

class Bytes {
  friend std::ostream& operator<<(std::ostream &o, const Bytes &a);
  friend bool operator< (const Bytes &a, const Bytes &b);
  friend bool operator> (const Bytes &a, const Bytes &b);
  friend bool operator== (const Bytes &a, const Bytes &b);

  private:
  char *data_;
  ssize_t len_;
  ssize_t pos_;

  void set_data(const char *data, ssize_t len);

  public:
  Bytes(const Bytes& other);
  Bytes(const char *str);
  Bytes(const char *data, ssize_t len);
  ~Bytes();

  ssize_t len() const;
  const char* data() const;
  static Bytes LongIntToBytes(long int number);
};
#endif
