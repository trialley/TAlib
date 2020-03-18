#include "pool.h"
#include <stdlib.h>

// for DEBUG
using std::cout;
using std::endl;

// static field init
string Pool::host_ = "";
string Pool::user_ = "";
string Pool::pass_ = "";
string Pool::dbname_ = "";
int Pool::poolSize_ = 0;
Pool* Pool::pool_ = NULL;
pthread_mutex_t Pool::mutex;
pthread_mutex_t Pool::execmutex;
pthread_mutex_t Pool::initmutex;
pthread_mutex_t Pool::logmutex;

void Pool::lock() {
  pthread_mutex_lock(&mutex);
}

void Pool::unlock() {
  pthread_mutex_unlock(&mutex);
}

void Pool::locke() {
  pthread_mutex_lock(&execmutex);
}

void Pool::unlocke() {
  pthread_mutex_unlock(&execmutex);
}

void Pool::locki() {
  pthread_mutex_lock(&initmutex);
}

void Pool::unlocki() {
  pthread_mutex_unlock(&initmutex);
}

void Pool::lockl() {
  pthread_mutex_lock(&logmutex);
}

void Pool::unlockl() {
  pthread_mutex_unlock(&logmutex);
}

Pool::Pool() {
  vec = new some[Pool::poolSize_];

  for (int i = 0; i < poolSize_; i++) {
    Connection* conn = new Connection(Pool::host_, Pool::user_, Pool::pass_, Pool::dbname_);
    //std::cout << conn << std::endl;
    //std::cout << host << " " << user << " " << pass << " " << dbname << " " << poolSize << std::endl;
    if (!conn) {
      cout << "xPool: new Connection Operation failed" << endl;
      exit(-1);
    }
    vec[i].first = conn;
    vec[i].second = false;
  }
}

Pool::~Pool() {
  for (int i = 0; i < poolSize_; i++) {
    delete vec[i].first;
  }
  delete[] vec;
  //mysql_library_end();
}

int Pool::initPool(string host, string user, string pass, string dbname, int poolSize) {
  host_ = host;
  user_ = user;
  pass_ = pass;
  dbname_ = dbname;
  poolSize_ = poolSize;

  return 0;
}

void Pool::destroyPool() {
  if (pool_) {
    delete pool_;
    pool_ = NULL;
  }
}

Connection* Pool::getConnection() {
  // init pool, open connections
  Pool::locki();
  if (pool_ == NULL) {
    pthread_mutex_init(&mutex, 0);
    pthread_mutex_init(&execmutex, 0);

    //mysql_library_init(0,NULL,NULL);

    pool_ = new Pool();
  }
  Pool::unlocki();

  //get connection operation
  Connection* ret = NULL;
  while (true) {
    Pool::lock();
    bool flag = false;
    for (int i = 0; i < poolSize_; i++) {
      if (pool_->vec[i].second == false) {
        pool_->vec[i].second = true;
        ret = pool_->vec[i].first;
        flag = true;
        break;
      }
    }
    if (flag == true) {
      Pool::unlock();
      break;
    } else {
      //cout << "wait" << endl;
      Pool::unlock();
      usleep(1000);
      continue;
    }
  }
  return ret;
}

int Pool::releaseConnection(Connection* conn) {
  lock();
  for (int i = 0; i < poolSize_; i++) {
    if (pool_->vec[i].first == conn) {
      pool_->vec[i].second = false;
      break;
    }
  }
  unlock();
  return 1;
}

Connection::Connection(string host, string user, string pass, string dbname) {
  static int connectionCount = 0;

  //cout << "C#:" << connectionCount++ << endl;

  // 初始化连接
  conn = mysql_init(NULL);

  //cout << "conn:" << conn << endl;

  // 执行物理的tcp连接动作，完成三次握手
  if (!mysql_real_connect(conn, host.c_str(), user.c_str(), pass.c_str(), dbname.c_str(), 0, NULL, 0)) {
    printf("xPool: Error connecting to database: %s\n", mysql_error(conn));
    exit(-1);
  } else {
  }
}

Connection::~Connection() {
  //mysql_thread_end();

  // 关闭TCP连接，四次挥手
  mysql_close(conn);
}

QueryResult Connection::executeQuery(string statement) {
  //Pool::locke();

  //cout << "0.start query" << endl;

  const char* query = statement.c_str();

  //cout << "1.before mysql_real_query" << endl;

  //cout << "strlen=[" << strlen(query) << "]" << endl;

  unsigned int len = (unsigned int)strlen(query);

  char q[100];
  strncpy(q, query, len);
  q[len] = 0;

  int status = mysql_real_query(conn, q, len);

  //cout << "1.after mysql_real_query" << endl;

  if (status) {
    printf("Error making query: %s\n",
           mysql_error(conn));
  } else {
    //printf("[%s] made...\n", query);
  }

  MYSQL_RES* resultSet;

  //cout << "2.before mysql_store_result" << endl;

  resultSet = mysql_store_result(conn);

  //cout << "2.after mysql_store_result" << endl;

  //cout << "3.before mysql_fetch_row" << endl;

  QueryResult queryResult;
  while (true) {
    MYSQL_ROW row;
    if (!(row = mysql_fetch_row(resultSet))) {
      break;
    }

    vector<string> string_row;
    for (int i = 0; i < mysql_num_fields(resultSet); i++) {
      string_row.push_back(row[i]);
    }

    queryResult.addRow(string_row);
  }

  //cout << "3.after mysql_fetch_row" << endl;

  //cout << "4.before mysql_free_result" << endl;

  mysql_free_result(resultSet);  //free result after you get the result

  //cout << "4.after mysql_free_result" << endl;

  //cout << "0.finish query" << endl;

  //Pool::unlocke();

  return queryResult;
}
