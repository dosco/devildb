#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <thread>

#include "memtable.h"
#include "sstable.h"

void gen_random(char *s, const int len) {
  static const char alphanum[] =
      "0123456789"
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "abcdefghijklmnopqrstuvwxyz";

  for (int i = 0; i < len; ++i) {
      s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
  }

  s[len] = 0;
}

void db_bench(MemTable &mt, int count) {
  char buff[200];

  for(int i=0;i<count;i++) {
    long int ts = static_cast<long int>(time(NULL));

    boost::ptr_vector<Column> values;
    gen_random(buff, 100);
    values.push_back(new Column("name", buff, ts));
    gen_random(buff, 100);
    values.push_back(new Column("phone", buff, ts));
    gen_random(buff, 100);
    values.push_back(new Column("email", buff, ts));
    gen_random(buff, 100);
    mt.Add("profile", buff, values.begin(), values.end());
  }
}

int main(int argc, const char* argv[]) {
  MemTable mt(ColumnFamily("profile"));

  long int ts = static_cast<long int>(time(NULL));
  std::cout << DeletedColumn("test", ts) << std::endl;

  ts = static_cast<long int>(time(NULL));
  boost::ptr_vector<Column> values;
  values.push_back(new Column("name",   "mike", ts));
  values.push_back(new Column("phone",  "12345", ts));
  values.push_back(new Column("email",  "mike@myspace.com", ts));
  mt.Add("profile", "ABCDEFGHIJK", values.begin(), values.end());

  //db_bench(mt, 10000);
  SSTable sst(mt);

  std::cout << "----=======Press Any Key========-----" << std::endl;
  std::cin.ignore();

  ts = static_cast<long int>(time(NULL));
  DeletedColumn dc("phone", ts);
  mt.Delete("profile", "ABCDEFGHIJK", dc);

  const Column *found = mt.Get("profile", "ABCDEFGHIJK", "email");
  if(found != NULL) std::cout << "FOUND: " << *found << std::endl;
  else std::cout << "NOT FOUND" << std::endl;

  found = mt.Get("profile", "ABCDEFGHIJK", "phone");
  if(found != NULL) std::cout << "FOUND: " << *found << std::endl;
  else std::cout << "NOT FOUND" << std::endl;

  // Display the number of processors/cores
  //std::cout << boost::thread::hardware_concurrency() << " processors/cores detected." << std::endl;
  //std::cout << "---------" << std::endl;

  return 0;
}

