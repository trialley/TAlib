#include <common/TimeStamp.h>
#include <inttypes.h>
#include <sys/time.h>

TimeStamp TimeStamp::now() {
  struct timeval tv;
  gettimeofday(&tv, 0);
  int64_t seconds = tv.tv_sec;
  return TimeStamp(seconds * kMicroSecondsPerSecond + tv.tv_usec);
}

std::string TimeStamp::toString() const {
  char buf[32] = {0};
  int64_t seconds = m_microSecondsSinceEpoch / kMicroSecondsPerSecond;
  int64_t microseconds = m_microSecondsSinceEpoch % kMicroSecondsPerSecond;
  snprintf(buf, sizeof(buf) - 1, "%" PRId64 ".%06" PRId64 "", seconds, microseconds);
  return buf;
}

std::string TimeStamp::toFormattedString(bool showMicroseconds) const {
  char buf[64] = {0};
  time_t seconds = static_cast<time_t>(m_microSecondsSinceEpoch / kMicroSecondsPerSecond);
  struct tm tm_time;
  gmtime_r(&seconds, &tm_time);

  if (showMicroseconds) {
    int microseconds = static_cast<int>(m_microSecondsSinceEpoch % kMicroSecondsPerSecond);
    snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d.%06d",
             tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
             tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,
             microseconds);
  } else {
    snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d",
             tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
             tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
  }
  return buf;
}