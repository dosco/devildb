#include <fstream>
#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>

#include "commit_log.h"
#include "formats.pb.h"
#include "memtable.h"

using namespace boost::interprocess;

CommitLog::CommitLog() {
  commit_log = NULL;
}

void CommitLog::Init(MemTable &mt) {
  CommitHeader h;
  ColumnCommitEntry cce;

  std::string filename = "data/" + mt.GetColumnFamily().name() + ".log";
  int i = 0;
  int pos = 0;

  try {
    file_mapping m_file(filename.c_str(), read_only);
    mapped_region region( m_file, read_only);
    void *addr = region.get_address();
    char *mem = static_cast<char*>(addr);
    int size = region.get_size();

    std::cout << "Reading commit log " << filename << " ..." << std::endl;

    while(pos <= size) {
      // Read in size of header and entry
      memcpy(&h, mem + pos, sizeof(CommitHeader));
      pos += sizeof(CommitHeader);
      if(h.size <= 0) continue;

      cce.ParseFromArray(mem + pos, h.size);
      pos += h.size;

      std::string rk = cce.row_key();
      Columns *c = mt.GetRow(rk);
      if(c == NULL) {
        c = mt.AddRow(rk);
      }

      if(h.type == COLUMN_ADD) {
        Column column(cce.column().c_str(), (char *) cce.value().c_str(),
          cce.value().size(), cce.timestamp());
        c->AddColumn(column);
      }

      else if(h.type == COLUMN_DELETE) {
        DeletedColumn column(cce.column().c_str(), (char *) cce.value().c_str(),
          cce.value().size(), cce.timestamp());
        c->DeleteColumn(column);
      }

      i++;
    }
    std::cout << "Successfully read " << i << " commit log entries" << std::endl;
  }
  catch(interprocess_exception &ex) { }

  commit_log = new std::ofstream(filename,
    std::ios::out | std::ios::binary | std::ios_base::app);
  commit_log->rdbuf()->pubsetbuf(0, 0);
}

CommitLog::~CommitLog() {
  if(commit_log != NULL) commit_log->close();
}

void CommitLog::CommitEntry(const std::string column_family, const std::string row_key, const Column &column) {
  if(commit_log != NULL) {
    CommitHeader h;
    h.type = (typeid(column) == typeid(DeletedColumn))
      ? COLUMN_DELETE : COLUMN_ADD;

    ColumnCommitEntry e;
    e.set_row_key(row_key);
    e.set_column_family(column_family);
    e.set_column(column.name());
    e.set_value(column.value().data(), column.value().size());
    e.set_timestamp(column.timestamp());
    h.size = e.ByteSize();

    int hdr_size = sizeof(CommitHeader);
    int size = hdr_size + h.size;
    char buffer[hdr_size + h.size];

    memcpy(buffer, &h, hdr_size);
    e.SerializeToArray(buffer + hdr_size, h.size);
    commit_log->write(buffer, size);
  }
}
