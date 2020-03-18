#ifndef _EVENTLOOPTHREADPOOL_H
#define _EVENTLOOPTHREADPOOL_H
#include <Logger/ptr_vector.h>
#include <functional>
#include <memory>
#include <vector>

namespace TA {
class EventLoop;
class EventLoopThread;

class EventLoopThreadPool {
 public:
  //typedef std::function<void(EventLoop*)> ThreadInitCallback;

  EventLoopThreadPool(EventLoop* m_baseLoop, const std::string& nameArg, int numThreads = 3);
  ~EventLoopThreadPool();
  void setThreadNum(int numThreads) { m_numThreads = numThreads; }
  void start();

  // valid after calling start()
  /// round-robin
  EventLoop* getNextLoop();

  /// with the same hash code, it will always return the same EventLoop
  EventLoop* getLoopForHash(size_t hashCode);

  std::vector<EventLoop*> getAllLoops();

  bool started() const { return m_started; }

  const std::string& name() const { return _name; }

 private:
  EventLoop* m_baseLoop;
  std::string _name;
  bool m_started;
  int m_numThreads;
  int m_next;
  myself::ptr_vector<EventLoopThread> m_threads;
  std::vector<EventLoop*> m_loops;
};
}  // namespace TA

#endif  // TA_NET_EVENTLOOPTHREADPOOL_H
