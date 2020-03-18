#ifndef _ASYNC_LOGGING_HH
#define _ASYNC_LOGGING_HH

#include <memory>
#include <mutex>
#include <string>

#include <Logger/Condition.h>
#include <Logger/Thread.h>
#include <Logger/ptr_vector.h>
#include "LogStream.h"

class AsyncLogging {
 public:
  ~AsyncLogging();
  static AsyncLogging* getInstance();
  void start() {
    if (!m_isRunning) {
      m_isRunning = true;
      m_thread.start();
    }
  }
  void set(const std::string filePath, off_t rollSize, double flushInterval) {
    m_filePath = filePath;
    m_rollSize = rollSize;
    m_flushInterval = flushInterval;
  }
  void stop() {
    m_isRunning = false;
    m_cond.notify();
    m_thread.join();
  }

  void append(const char* logline, std::size_t len);

 private:
  AsyncLogging();
  AsyncLogging(const AsyncLogging&);
  AsyncLogging& operator=(const AsyncLogging&);

  void threadRoutine();

  typedef LogBuffer<kLargeBuffer> Buffer;
  typedef myself::ptr_vector<Buffer> BufferVector;
  typedef std::unique_ptr<Buffer> BufferPtr;

  std::string m_filePath;
  off_t m_rollSize;
  double m_flushInterval;

  bool m_isRunning;
  Thread m_thread;
  std::mutex m_mutex;
  Condition m_cond;

  BufferPtr m_currentBuffer;
  BufferPtr m_nextBuffer;
  BufferVector m_buffers;
};

#endif
