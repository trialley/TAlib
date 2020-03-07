#pragma once

#include <vector>
#include "../MemPool/MemPool.h"
#include "ThreadEventLoop.h"
class ThreadpoolEventLoop {
 private:
  std::vector<SP_ThreadEventLoop> elv;
  int threadnum;
  int index;

 public:
  ThreadpoolEventLoop(int Threadnum);
  ~ThreadpoolEventLoop();
  void start();
  SP_EventLoop getNextloop();
};

typedef std::unique_ptr<ThreadpoolEventLoop, decltype(deleteElement<ThreadpoolEventLoop>)*> UP_ThreadpoolEventLoop;
