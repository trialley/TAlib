#pragma once

#include "../Log/Logging.h"
#include "../Reactor/EventLoop.h"
#include "../Threadpool/ThreadpoolEventLoop.h"
#include "../Wrapper/wrapper.h"
#include "./TcpConnection.h"

class TcpServer {
 private:
  SP_EventLoop loop;
  SP_Channel TcpServerchannel;
  UP_ThreadpoolEventLoop iothreadpool;
  int listenfd;
  std::unordered_map<int, shared_ptr<TcpConnection>> _TcpMap;
  void handleconn();
  void handleclose(WP_Channel channel);
  void deletemap(SP_Channel channel);
  TcpConnection::OnMessageCallback _onMessageCallback;

 public:
  void setOnMessageCallback(TcpConnection::OnMessageCallback in);
  TcpServer(const char* port, int threadnum);
  ~TcpServer();
  void start();
};
