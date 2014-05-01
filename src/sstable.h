#ifndef BORG_SSTABLE_H_
#define BORG_SSTABLE_H_

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "memtable.h"
#include "bloomfilter.h"

class SSTable {
  private:
  //const char SIGNATURE = "SST";
  MemTable mt_;
  BloomFilter bf_;
  std::string filename_;

  void* mmapped_region_;
  size_t length_;

  const char* index_;
  size_t index_length_;

  int Write(const char *data, size_t len);
  void ReadIndex();
  void ReadBloomFilter();
  void WriteBloomFilter();
  void WriteData();

  public:
  SSTable(MemTable &mt);
};
#endif
