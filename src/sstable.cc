#include <boost/thread.hpp>
#include <fstream>

#include "sstable.h"
#include "iterator.h"

void ThreadFunction()
{
    int counter = 0;

    for(;;)
    {
        std::cout << "thread iteration " << ++counter << " Press Enter to stop" << std::endl;

        try
        {
            // Sleep and check for interrupt.
            // To check for interrupt without sleep,
            // use boost::this_thread::interruption_point()
            // which also throws boost::thread_interrupted
            boost::this_thread::sleep(boost::posix_time::milliseconds(500));
        }
        catch(boost::thread_interrupted&)
        {
            std::cout << "Thread is stopped" << std::endl;
            return;
        }
    }
}

class ValueIterator : public Iterator {
  private:
  const char *data_;
  const char *data_start_;
  const char *data_current_;
  const char *data_next_;
  size_t data_len_;

  const char *key_;
  size_t key_len_;

  const char *value_;
  size_t value_len_;

  long int timestamp_;

  inline const char* process_block() {
    const char *base = data_current_;

    key_len_ = value_len_ = 0;
    memcpy(&key_len_, base, sizeof(size_t));
    base += sizeof(size_t);
    key_ = base;
    base += key_len_;

    memcpy(&value_len_, base, sizeof(size_t));
    base += sizeof(size_t);
    value_ = base;
    base += value_len_;

    memcpy(&timestamp_, base, sizeof(long int));
    base += sizeof(long int);

    return base;
  }

  public:
  ValueIterator(const char *start, size_t len)
    : Iterator() {
    data_len_ = len;
    data_ = start + sizeof(size_t);
    data_start_ = data_;
    data_current_ = data_;
    data_next_ = process_block();
  }

  ~ValueIterator() { }

  virtual void SeekToFirst() {
    data_current_ = data_start_;
  }

  virtual void SeekToLast() { }

  virtual void Seek(const Slice& target) {
    for(;Valid() && key() != target; Next());
  }

  virtual void Next() {
    data_current_ = data_next_;
    data_next_ = process_block();
  }

  virtual void Prev() {  }

  virtual bool Valid() const {
    return data_next_ < (data_start_ + data_len_);
  }

  Slice key() const {
    return Slice(key_, key_len_);
  }

  Slice value() const {
    return Slice(value_, value_len_);
  }
};

class IndexIterator : public Iterator {
  private:
  const char *data_;
  const char *index_start_;
  const char *index_current_;
  const char *index_next_;
  size_t index_len_;

  const char *key_;
  size_t key_len_;

  size_t value_index_;
  size_t value_len_;

  inline const char* process_block() {
    const char *base = index_current_;

    key_len_ = value_len_ = 0;
    memcpy(&key_len_, base, sizeof(size_t));
    base += sizeof(size_t);
    key_ = base;
    base += key_len_;

    memcpy(&value_index_, base, sizeof(size_t));
    base += sizeof(size_t);

    memcpy(&value_len_, base, sizeof(size_t));
    base += sizeof(size_t);

    return base;
  }

  public:
  IndexIterator(const char *data, const char *start, size_t len)
    : Iterator(),
      data_(data),
      index_start_(start),
      index_len_(len),
      index_current_(start),
      index_next_(0) {

    index_next_ = process_block();
  }

  ~IndexIterator() { }

  virtual void SeekToFirst() {
    index_current_ = index_start_;
  }

  virtual void SeekToLast() { }

  virtual void Seek(const Slice& target) {
    for(;Valid() && key() != target; Next());
  }

  virtual void Next() {
    index_current_ = index_next_;
    index_next_ = process_block();
  }

  virtual void Prev() {  }

  virtual bool Valid() const {
    return index_next_ < (index_start_ + index_len_);
  }

  Slice key() const {
    return Slice(key_, key_len_);
  }

  Slice value() const {
    return NULL;
  }

  size_t value_index() const {
    return value_index_;
  }

  size_t value_len() const {
    return value_len_;
  }
};

int SSTable::Write(const char *data, size_t len) {
  //sst_->write((const char *) &len, sizeof(size_t));
  //sst_->write((const char *) data, len);

  return len;
}

void SSTable::ReadBloomFilter() {
  std::cout << "Reading Bloomfilter from disk..." << std::endl;

  std::ifstream is(filename_ + ".filter", std::ios::in | std::ios::binary);
  bf_.deserialize(is);
  is.close();
}

void SSTable::ReadIndex() {
  const char *base = reinterpret_cast<const char*>(mmapped_region_);
  const char *end = base + length_;

  Slice signature(end - 3, 3);
  memcpy(&index_length_, end - signature.size() - sizeof(size_t), sizeof(size_t));
  index_ = end - signature.size() - sizeof(size_t) - index_length_;

  std::cout << "Signature: " << signature  << std::endl;
  std::cout << "Index Length: " << index_length_ << std::endl;

  IndexIterator *i = new IndexIterator(base, index_, index_length_);
  Slice s("zvZUwt`7tg53c3U88UzyNl0gL0fEuZylir2YngQCiESOBwgKjuXTAvyuHVA747kHnjOEL8az9TXI3hYEZryC8izXPy9KYdsEwTg1s", 100);
  //for(i->Seek(s);i->Valid() ;i->Next()) {
  for(;i->Valid() ;i->Next()) {
    std::cout << i->key().ToString() << " = " << i->value().size() << std::endl;
    ValueIterator *v = new ValueIterator(base + i->value_index(), i->value_len());
    //for(;v->Valid() ;v->Next()) {
      //std::cout << v->key().ToString() << " = " << v->value().ToString() << std::endl;
    //}
  }
}

void SSTable::WriteBloomFilter() {
  std::cout << "Writing Bloomfilter to disk..." << std::endl;

  //TODO: Could by optimized in the future
  int row_column_count = mt_.count();
  for(Rows::RowMap::const_iterator it = mt_.begin(); it != mt_.end(); ++it) {
    Columns *c = it->second;
    row_column_count += c->count();
  }

  //Create the filter
  BloomFilter bf(row_column_count, 0.2);
  for(Rows::RowMap::const_iterator it = mt_.begin(); it != mt_.end(); ++it) {
    Columns *c = it->second;
    bf.add(it->first);
    for(Columns::const_iterator cit = c->begin(); cit != c->end(); ++cit) {
      bf.add(it->first + ":" + cit->name());
    }
  }

  std::ofstream os(filename_ + ".filter", std::ios::out | std::ios::binary);
  os.rdbuf()->pubsetbuf(0, 0);
  bf.serialize(os);
  os.close();
}

inline size_t write_column(std::ofstream &os, const Column &c) {
  size_t bytes_written = 0;

  size_t name_len = c.name().size();
  os.write((const char *) &name_len, sizeof(size_t));
  os.write((const char *) c.name().c_str(), name_len);
  bytes_written += sizeof(size_t) + name_len;

  size_t value_len = c.value().size();
  os.write((const char *) &value_len, sizeof(size_t));
  os.write((const char *) c.value().data(), value_len);
  bytes_written += sizeof(size_t) + value_len;

  long int ts = c.timestamp();
  os.write((const char *) &ts, sizeof(long int));
  bytes_written += sizeof(long int);

  return bytes_written;
}

struct CFIndexInfo {
  size_t index;
  size_t size;
};

void SSTable::WriteData() {
  std::cout << "Writing SSTable to disk..." << std::endl;

  std::map<std::string, CFIndexInfo *> rowindex;
  size_t i = 0;
  //int BLOCK_SIZE = 8192;

  std::ofstream os(filename_ + ".sst", std::ios::out | std::ios::binary);
  os.rdbuf()->pubsetbuf(0, 0);

  // write columns
  for(Rows::RowMap::const_iterator it = mt_.begin(); it != mt_.end(); ++it) {
    CFIndexInfo *cfif = new CFIndexInfo();
    cfif->index = i;

    Columns *c = it->second;
    for(Columns::const_iterator cit = c->begin(); cit != c->end(); ++cit) {
      i += write_column(os, *cit);
    }
    cfif->size = i - cfif->index - i;
    rowindex.insert(std::make_pair(it->first, cfif));
  }

  // write index
  size_t index_block_size = 0;

  for(std::map<std::string, CFIndexInfo *>::const_iterator it = rowindex.begin(); it != rowindex.end(); ++it) {
    size_t row_key_len = it->first.size();
    os.write((const char *) &row_key_len, sizeof(size_t));
    os.write((const char *) it->first.c_str(), row_key_len);

    CFIndexInfo *cfif = it->second;
    //std::cout << block_pos << std::endl;

    os.write((const char *) &cfif->index, sizeof(size_t));
    os.write((const char *) &cfif->size, sizeof(size_t));

    index_block_size += row_key_len + sizeof(size_t) + sizeof(size_t) + sizeof(size_t);
  }

  // write index size
  os.write((const char *) &index_block_size, sizeof(size_t));
  os.write((const char *) "SST", 3);

  os.close();
}

SSTable::SSTable(MemTable &mt) : mt_(mt), filename_("data/" + mt_.GetColumnFamily().name()) {
   //boost::thread t(&ThreadFunction);
  std::cout << "Initializing SSTable..." << std::endl;

  std::string fname = filename_ + ".sst";
  int fd = open(fname.c_str(), O_RDONLY);

  //is_ = new std::ifstream(filename_ + ".filter", std::ios::in | std::ios::binary | std::ios::ate);
  if(fd < 0) {
    WriteBloomFilter();
    WriteData();

    return;
  }

  // File size
  struct stat buf;
  fstat(fd, &buf);
  length_ = buf.st_size;

  mmapped_region_ = mmap(NULL, length_, PROT_READ, MAP_SHARED, fd, 0);
  std::cout << "Length: " << length_ << std::endl;

  //ReadBloomFilter();
  ReadIndex();
}

