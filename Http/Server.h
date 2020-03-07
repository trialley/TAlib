#pragma once

#include "../Log/Logging.h"
#include "../Reactor/EventLoop.h"
#include "../Threadpool/ThreadpoolEventLoop.h"
#include "../Wrapper/wrapper.h"
#include "./HttpConnection.h"

class Server {
 private:
  SP_EventLoop loop;
  SP_Channel serverchannel;
  UP_ThreadpoolEventLoop iothreadpool;
  int listenfd;
  std::unordered_map<int, SP_HttpConnection> Httpmap;
  void handleconn();
  void handleclose(WP_Channel channel);
  void deletemap(SP_Channel channel);

 public:
  Server(const char* port, int threadnum);
  ~Server();
  void start();
};
