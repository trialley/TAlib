#pragma once

#include <memory>
#include <unordered_map>
#include <vector>
#include "../MemPool/MemPool.h"
#include "../Wrapper/wrapper.h"
#include "./Channel.h"

#define MAXFDS 10000
#define EVENTS 4096
#define EPOLLWAIT_TIME -1

class EventLoop;
class Channel;
typedef std::shared_ptr<EventLoop> SP_EventLoop;
typedef std::weak_ptr<EventLoop> WP_EventLoop;
typedef std::shared_ptr<Channel> SP_Channel;

class Epoll {
 private:
  int epollfd;
  std::vector<SE> events;
  std::unordered_map<int, SP_Channel> Channelmap;

 public:
  Epoll();
  ~Epoll();
  void add(const SP_Channel &request);
  void update(const SP_Channel &request);
  void del(const SP_Channel &request);
  void poll(std::vector<SP_Channel> &req);
};

typedef std::shared_ptr<Epoll> SP_Epoll;
