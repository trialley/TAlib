#include <string>
class RedisCache {
 public:
  const std::string REDIS_CACHE_PRE = "REDIS_CACHE_PRE";

  static RedisCache* getRedisCache();
  bool set(std::string&, std::string&);
  bool get(std::string&, std::string&);
  ~RedisCache();

 private:
  RedisCache();
  RedisCache(const RedisCache&);
  RedisCache& operator=(const RedisCache&);
};