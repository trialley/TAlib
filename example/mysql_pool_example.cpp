#include <MySQL/pool.h>
#include <stdlib.h>
#include <unistd.h>
using std::cout;
using std::endl;

void* handler(void* arg) {
  long tid = (long)arg;

  //cout << "tid =[" << tid << "]" << endl;
  Connection* conn = Pool::getConnection();

  if (!conn) {
    cout << "getConnection NULL pointer" << endl;
    exit(-1);
  }

  cout << "Connection.this:" << conn << endl;

  const char* query;
  query = "select * from q;";

  QueryResult queryResult = conn->executeQuery(query);

  Pool::releaseConnection(conn);

  for (int i = 0; i < queryResult.getRowCount(); i++) {
    for (int j = 0; j < queryResult.getColumnCount(); j++) {
      cout << queryResult.getElement(i, j) << " ";
    }
    cout << endl;
  }
}

int main() {
  string host = "127.0.0.1";
  string user = "root";
  string pass = "haosql";
  string dbname = "test";
  int poolSize = 20;
  const int THREAD_COUNT = 500;

  Pool::initPool(host, user, pass, dbname, poolSize);

  ///std::cin.get();

  //   if (argc > 1) {
  //     count = atoi(argv[1]);
  //   }

  pthread_t threads[THREAD_COUNT];
  for (long i = 0; i < THREAD_COUNT; i++) {
    pthread_create(&threads[i], NULL, handler, (void*)i);
    //sleep(1);
  }

  for (int i = 0; i < THREAD_COUNT; i++) {
    pthread_join(threads[i], 0);
  }

  Pool::destroyPool();

  return 0;
}