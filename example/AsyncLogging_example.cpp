#include <stdio.h>
#include <sys/resource.h>
#include <unistd.h>
#include <Logger/AsyncLogging.cpp>
#include <Logger/Logger.cpp>
#include <string>
using namespace std;
off_t kRollSize = 500 * 1000 * 1000;

void bench(bool longLog) {
  Logger::setAsync(basename("./test1.txt"), kRollSize);
  int cnt = 0;
  const int kBatch = 1000;
  string empty = " ";
  string longStr(3000, 'X');
  longStr += " ";

  for (int t = 0; t < 30; ++t) {
    TimeStamp start = TimeStamp::now();
    for (int i = 0; i < kBatch; ++i) {
      LOG_INFO << "Hello 0123456789"
               << " abcdefghijklmnopqrstuvwxyz "
               << (longLog ? longStr : empty)
               << cnt;
      ++cnt;
    }
    TimeStamp end = TimeStamp::now();
    struct timespec ts = {0, 500 * 1000 * 1000};
    nanosleep(&ts, NULL);
  }
}

int main(int argc, char* argv[]) {
  {
    // set max virtual memory to 2GB.
    size_t kOneGB = 1000 * 1024 * 1024;
    rlimit rl = {2 * kOneGB, 2 * kOneGB};
    setrlimit(RLIMIT_AS, &rl);
  }

  printf("pid = %d\n", getpid());

  char name[256] = {0};
  strncpy(name, argv[0], sizeof name - 1);

  bool longLog = argc > 1;
  bench(longLog);
}
