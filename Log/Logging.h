#pragma once

#include <sys/time.h>
#include "./AsyncLogging.h"
#include "./FixedBuffer.h"

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

 private:
  LogStream _stream;
  int _line;
  std::string _filename;
  static LogLevel _level;
  void _formatTime();

 public:
  std::string getBaseName();
  LogStream& stream();
  int getLine();
  static LogLevel getLogLevel();
  static std::string logFileName;
  Logger(const char* filename, int line, LogLevel level, const char* func);
  ~Logger();
  static std::string getLogFileName();
};

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
