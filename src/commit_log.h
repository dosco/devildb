#ifndef BORG_COMMITLOG_H_
#define BORG_COMMITLOG_H_

#include "column.h"

class MemTable;

enum CommitType { COLUMN_ADD, COLUMN_DELETE };
struct CommitHeader {
  CommitType type;
  int size;
};

class CommitLog {
private:
  std::ofstream *commit_log;

public:
  CommitLog();
  ~CommitLog();

  void Init(MemTable &mt);
  void CommitEntry(const std::string column_family, const std::string row_key, const Column &column);
};
#endif