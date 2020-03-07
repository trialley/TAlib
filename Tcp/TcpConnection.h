#pragma once

#include <sys/mman.h>
#include <iostream>
#include <string>
#include <unordered_map>
#include "../MemPool/MemPool.h"
#include "../Reactor/EventLoop.h"
#include "../Wrapper/wrapper.h"
using namespace std;

class Channel;
typedef shared_ptr<Channel> SP_Channel;

class TcpConnection {
 public:
  typedef std::function<void(TcpConnection* conn, string& in)> OnMessageCallback;
  TcpConnection(SP_Channel channel);
  ~TcpConnection();
  void setOnMessageCallback(OnMessageCallback in);
  void setSend(string& buffer_out);

 private:
  SP_Channel channel;
  string _inbuffer;
  string _outbuffer;
  OnMessageCallback _onMessageCallback;
  void handleRead();
  void handleSend();
};

typedef shared_ptr<TcpConnection> SP_TcpConnection;
