#ifndef BORG_ITERATOR_H_
#define BORG_ITERATOR_H_

#include "slice.h"

class Iterator {
  public:
  Iterator();
  virtual ~Iterator();

  virtual bool Valid() const = 0;
  virtual void SeekToFirst() = 0;
  virtual void SeekToLast() = 0;
  virtual void Seek(const Slice& target) = 0;
  virtual void Next() = 0;
  virtual void Prev() = 0;
  virtual Slice key() const = 0;
  virtual Slice value() const = 0;

 private:
  // No copying allowed
  Iterator(const Iterator&);
  void operator=(const Iterator&);
};

#endif