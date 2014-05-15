devildb
=======

A C++ implementation of LevelDB with support for column families. I was inspired by the BigTable paper to try and implement my own "Log Structured Merge Tree" backed DB. This is still work in progress. 

* Support for Column Families
* AVL tree based implementation of MemTables
* An append only log using Protobufs for serialization
* A SSTable implementation with a Bloomfilter index, Row-key and column indexes
