#include <Logger/Logger.h>
#include <Redis/RedisConnect.h>
#include <string>

int main() {
  string val;
  RedisConnect::Setup("127.0.0.1");
  //从连接池中获取一个连接
  shared_ptr<RedisConnect> redis = RedisConnect::Instance();
  LOG_INFO << "get pool\n";

  //设置一个键值
  redis->set("key", "val");
  LOG_INFO << "set key\n";

  //获取键值内容
  redis->get("key", val);

  //执行expire命令设置超时时间
  redis->execute("expire", "key", 60);

  //获取超时时间(与ttl(key)方法等价)
  redis->execute("ttl", "key");

  //调用getStatus方法获取ttl命令执行结果
  LOG_INFO << "超时时间：" << redis->getStatus();

  //执行del命令删除键值
  redis->execute("del", "key");
}