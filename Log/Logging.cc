#include "Logging.h"
#include <iostream>
#include<thread>
#include<sstream>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>  
#define gettid() syscall(__NR_gettid)
static const char black[] = { 0x1b, '[', '1', ';', '3', '0', 'm', 0 };
static const char red[] = { 0x1b, '[', '1', ';', '3', '1', 'm', 0 };
static const char green[] = { 0x1b, '[', '1', ';', '3', '2', 'm', 0 };
static const char yellow[] = { 0x1b, '[', '1', ';', '3', '3', 'm', 0 };
static const char blue[] = { 0x1b, '[', '1', ';', '3', '4', 'm', 0 };
static const char purple[] = { 0x1b, '[', '1', ';', '3', '5', 'm', 0 };
static const char normal[] = { 0x1b, '[', '0', ';', '3', '9', 'm', 0 };

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
static pthread_once_t once_control = PTHREAD_ONCE_INIT;

std::string Logger::logFileName;

UP_AsyncLogging& getAsyncLogging () {
    static UP_AsyncLogging AsyncLogger (newElement<AsyncLogging> (Logger::getLogFileName ()), deleteElement<AsyncLogging>);
    return AsyncLogger;
}

//异步日志主线程
void init () {
    Logger::logFileName = "./log.txt";
    getAsyncLogging ()->start ();
}

//日志输出函数，控制输出到异步还是到控制台
void output (const char* msg, int len) {
    pthread_once (&once_control, init);
    std::cout << msg;
    getAsyncLogging ()->append (msg, len);
}
//void output(const char* msg, int len) {
//  std::cout << msg;
//}
void Logger::_formatTime () {
    struct timeval tv;
    time_t time;
    char str_t[26] = { 0 };
    gettimeofday (&tv, NULL);
    time = tv.tv_sec;
    struct tm* p_time = localtime (&time);
    strftime (str_t, 26, "%Y-%m-%d %H:%M:%S ", p_time);
    _stream << str_t;
}
Logger::LogLevel Logger::getLogLevel () {
    return _level;
}
Logger::LogLevel Logger::_level = Logger::TRACE;

#ifdef _WIN32
#define filenamecut(x) (strrchr(x,'\\')?strrchr(x,'\\')+1:x)
#elif __linux
#define filenamecut(x) (strrchr(x,'/')?strrchr(x,'/')+1:x)
#endif

typedef unsigned long long ULL;

std::string getThreadIdOfString (const std::thread::id& id) {
    std::stringstream in;
    in << id;
    return in.str ();
}

ULL getThreadIdOfULL (const std::thread::id& id) {
    return std::stoull (getThreadIdOfString (id));
}

Logger::Logger (const char* filename, int line, LogLevel level, const char* func) {
    _formatTime ();  //打印时间
    /*打印 [带颜色的等级] [文件:行-函数]  TODO:打印线程协程号*/
    _stream << LogLevelColor[level] << LogLevelName[level]  << LogLevelColor[6] << ' '
       <<" [pid:" << getpid () <<"] [tid:"<< gettid () <<"] " << '[' << filenamecut (filename) << ':' << line << "] [" << func << "] ";
}

Logger::~Logger () {
    stream () << '\n';
    const LogStream::Buffer& buf (_stream.buffer ());
    output (buf.data (), buf.length ());
}

std::string Logger::getLogFileName () {
    return logFileName;
}

LogStream& Logger::stream () {
    return _stream;
}
