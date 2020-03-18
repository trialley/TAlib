#pragma once
#include <mysql/mysql.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <map>
#include <string>
#include <vector>
using std::string;
using std::vector;

class Connection;

class some {
 public:
  some() {}
  Connection* first;
  bool second;
};

class Pool {
 public:
  static int initPool(string host, string user, string pass, string dbname, int poolSize);
  static void destroyPool();

  static Connection* getConnection();
  static int releaseConnection(Connection*);
  static void lock();
  static void unlock();

  static void locke();
  static void unlocke();

  static void locki();
  static void unlocki();

  static void lockl();
  static void unlockl();

 private:
  static pthread_mutex_t mutex;
  static pthread_mutex_t execmutex;
  static pthread_mutex_t initmutex;
  static pthread_mutex_t logmutex;
  static Pool* pool_;

 private:
  some* vec;  // need an instance to init

 public:
  static string host_;
  static string user_;
  static string pass_;
  static string dbname_;
  static int poolSize_;

 public:
  Pool();
  virtual ~Pool();
};

class QueryResult;

class Connection {
 public:
  Connection(string host, string user, string pass, string dbname);
  virtual ~Connection();

  QueryResult executeQuery(string statement);

 private:
  // MYSQL代表了一条TCP连接
  MYSQL* conn;
};

class QueryResult {
 public:
  int getRowCount() { return string_table.size(); }
  int getColumnCount() { return string_table[0].size(); };
  string getElement(int row, int column) { return string_table[row][column]; }
  void addRow(const vector<string>& row) { string_table.push_back(row); }

 private:
  vector<vector<string> > string_table;
};