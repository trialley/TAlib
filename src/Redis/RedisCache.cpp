#include "RedisCache.h"
#include <memory>
#include "RedisConnect.h"

RedisCache::RedisCache() {
  RedisConnect::Setup("127.0.0.1");
}
RedisCache* RedisCache::getRedisCache() {
  static RedisCache cache;
  return &cache;
}

bool RedisCache::set(std::string& key, std::string& value) {
  shared_ptr<RedisConnect> redis = RedisConnect::Instance();

  int result = redis->set(key, value);
  if (result == RedisConnect::PARAMERR) {
    return false;
  } else {
    return true;
  }
}
bool RedisCache::get(std::string& key, std::string& value) {
  shared_ptr<RedisConnect> redis = RedisConnect::Instance();
  int result = redis->get(key, value);
  if (result != RedisConnect::OK ) {
    return false;
  } else {
    return true;
  }
}
RedisCache::~RedisCache() {}