#ifndef _LOGGER_HH
#define _LOGGER_HH

#include <Logger/TimeStamp.h>
#include <string.h>
#include "LogStream.h"

// CAUTION: do not write:
//
// if (good)
//   LOG_INFO << "Good news";
// else
//   LOG_WARN << "Bad news";
//
// this expends to
//
// if (good)
//   if (logging_INFO)
//     logInfoStream << "Good news";
//   else
//     logWarnStream << "Bad news";
//

class Logger {
 public:
  enum LogLevel {
    TRACE,
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL,
    NUM_LOG_LEVELS,
  };

  //compile time calculation of basename of source file

  Logger(const char* filename, int line, LogLevel level, const char* func);

  ~Logger();

  static void setLogLevel(LogLevel level);
  static LogLevel getLogLevel();

  LogStream& stream() { return _stream; }

  typedef void (*outputFunc)(const char* msg, int len);
  typedef void (*flushFunc)();

  static void setOutput(outputFunc);
  static void setFlush(flushFunc);

 private:
  void _formatTime();
  int _line;
  TimeStamp _time;
  std::string _filename;
  static LogLevel _level;
  LogStream _stream;
  Logger(const Logger& lg);  //no copyable
  Logger& operator=(const Logger& lg);
};

const char* strerror_tl(int savedErrno);
#define LOG_TRACE                             \
  if (Logger::getLogLevel() <= Logger::TRACE) \
  Logger(__FILE__, __LINE__, Logger::TRACE, __func__).stream()
#define LOG_DEBUG                             \
  if (Logger::getLogLevel() <= Logger::DEBUG) \
  Logger(__FILE__, __LINE__, Logger::DEBUG, __func__).stream()
#define LOG_INFO                             \
  if (Logger::getLogLevel() <= Logger::INFO) \
  Logger(__FILE__, __LINE__, Logger::INFO, __func__).stream()
#define LOG_WARN                             \
  if (Logger::getLogLevel() <= Logger::WARN) \
  Logger(__FILE__, __LINE__, Logger::WARN, __func__).stream()
#define LOG_ERROR                             \
  if (Logger::getLogLevel() <= Logger::ERROR) \
  Logger(__FILE__, __LINE__, Logger::ERROR, __func__).stream()
#define LOG_FATAL                             \
  if (Logger::getLogLevel() <= Logger::FATAL) \
  Logger(__FILE__, __LINE__, Logger::FATAL, __func__).stream()

#endif