#include <Logger/Logger.h>
#include <Reactor/Reactor.h>
#include <Tcp/TcpServer.h>

void on_connection(const TA::TcpConnectionPtr& conn) {
  LOG_DEBUG << "new conn from " << conn->peerAddress().toIpPort();
}

void on_message(const TA::TcpConnectionPtr& conn, TA::Buffer* buffer, ssize_t len) {
  LOG_DEBUG << "on message : " << len << " bytes " << buffer->peek();
  conn->send(buffer->peek());
  buffer->retrieve(len);
}

int main() {
  Logger::setLogLevel(Logger::DEBUG);

  TA::EventLoop loop;

  InetAddress localAddr(8080);
  TA::TcpServer tcp_server(&loop, localAddr);

  tcp_server.setConnectionCallBack(std::bind(on_connection, std::placeholders::_1));
  tcp_server.setMessageCallBack(std::bind(on_message, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
  //tcp_server.setCloseCallBack(std::bind(on_close, std::placeholders::_1));
  tcp_server.start();

  loop.loop();
}