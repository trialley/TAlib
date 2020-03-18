#include <Logger/AsyncLogging.h>
#include <Logger/Logger.h>
#include <assert.h>

#include <common/TimeStamp.h>
#include <errno.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <thread>
#define gettid() syscall(__NR_gettid)

__thread char t_time[64];
__thread time_t t_lastSecond;
__thread char t_errnobuf[512];

static const char black[] = {0x1b, '[', '1', ';', '3', '0', 'm', 0};
static const char red[] = {0x1b, '[', '1', ';', '3', '1', 'm', 0};
static const char green[] = {0x1b, '[', '1', ';', '3', '2', 'm', 0};
static const char yellow[] = {0x1b, '[', '1', ';', '3', '3', 'm', 0};
static const char blue[] = {0x1b, '[', '1', ';', '3', '4', 'm', 0};
static const char purple[] = {0x1b, '[', '1', ';', '3', '5', 'm', 0};
static const char normal[] = {0x1b, '[', '0', ';', '3', '9', 'm', 0};
const char* LogLevelColor[Logger::NUM_LOG_LEVELS + 1] = {
    purple,
    blue,
    green,
    yellow,
    red,
    black,
    normal,
};
const char* LogLevelName[Logger::NUM_LOG_LEVELS] = {
    "[TRACE]",
    "[DEBUG]",
    "[INFO ]",
    "[WARN ]",
    "[ERROR]",
    "[FATAL]",
};
const char* strerror_tl(int savedErrno) {
  return strerror_r(savedErrno, t_errnobuf, sizeof(t_errnobuf));
}

Logger::LogLevel g_logLevel = Logger::TRACE;

void Logger::setLogLevel(LogLevel level) {
  g_logLevel = level;
}

Logger::LogLevel Logger::getLogLevel() {
  return g_logLevel;
}

// helper class for known string length at compile time
// class T {
//  public:
//   T(const char* str, unsigned len)
//       : m_str(str),
//         m_len(len) {
//     assert(strlen(str) == m_len);
//   }

//   const char* m_str;
//   const unsigned m_len;
// };

void defaultOutput(const char* msg, int len) {
  size_t n = fwrite(msg, 1, len, stdout);
  (void)n;
}

void defaultFlush() {
  fflush(stdout);
}
void asyncOutput(const char* msg, int len) {
  AsyncLogging::getInstance()->append(msg, len);
}

void asyncFlush() {
  fflush(stdout);
}
Logger::outputFunc g_output = defaultOutput;
Logger::flushFunc g_flush = defaultFlush;
void Logger::setAsync(const std::string filePath, off_t rollSize, double flushInterval) {
  AsyncLogging* temp = AsyncLogging::getInstance();
  temp->set(filePath, rollSize, flushInterval);
  temp->start();

  Logger::setOutput(asyncOutput);
  Logger::setFlush(asyncFlush);
}
void Logger::setOutput(outputFunc out) {
  g_output = out;
}

void Logger::setFlush(flushFunc flush) {
  g_flush = flush;
}
#ifdef _WIN32
#define filenamecut(x) (strrchr(x, '\\') ? strrchr(x, '\\') + 1 : x)
#elif __linux
#define filenamecut(x) (strrchr(x, '/') ? strrchr(x, '/') + 1 : x)
#endif
Logger::Logger(const char* filename, int line, LogLevel level, const char* func)
    : _time(TimeStamp::now()) {
  _formatTime();  //打印时间
  /*打印 [带颜色的等级] [文件:行-函数]  TODO:打印线程协程号*/
  _stream << LogLevelColor[level] << LogLevelName[level] << LogLevelColor[6] << ' '
          << " [pid:" << getpid() << "] [tid:" << gettid() << "] " << '[' << filenamecut(filename) << ':' << line << "] [" << func << "] "
          << LogLevelColor[level] << "|" << LogLevelColor[6];
}

Logger::~Logger() {
  _stream << "\n";
  const LogStream::Buffer& buf(stream().buffer());
  g_output(buf.data(), buf.length());
}

void Logger::_formatTime() {
  int64_t microSecondsSinceEpoch = _time.microSecondsSinceEpoch();
  time_t seconds = static_cast<time_t>(microSecondsSinceEpoch / TimeStamp::kMicroSecondsPerSecond);
  int microseconds = static_cast<int>(microSecondsSinceEpoch % TimeStamp::kMicroSecondsPerSecond);
  if (seconds != t_lastSecond) {
    t_lastSecond = seconds;
    struct tm tm_time;

    ::gmtime_r(&seconds, &tm_time);  // FIXME TimeZone::fromUtcTime

    int len = snprintf(t_time, sizeof(t_time), "%4d-%02d-%02d %02d:%02d:%02d",
                       tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
                       tm_time.tm_hour + 8, tm_time.tm_min, tm_time.tm_sec);
    assert(len == 19);
    (void)len;
  }

  Fmt us(".%06d ", microseconds);
  assert(us.length() == 8);
  _stream << t_time << us.data();
}
