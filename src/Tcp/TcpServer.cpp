#include <assert.h>

#include <Logger/Logger.h>
#include <Reactor/Acceptor.h>
#include <Reactor/EventLoop.h>
#include <Socket/SocketHelp.h>
#include <Tcp/TcpServer.h>

using namespace TA;

void NetCallBacks::defaultConnectionCallback() {
  LOG_TRACE << "defaultConnectionCallback ";
}

TcpServer::TcpServer(EventLoop* loop, const InetAddress& listenAddr, const std::string& name)
    : p_loop(loop),
      _name(name),
      _threadPool(new EventLoopThreadPool(loop, name)),
      p_acceptor(new Acceptor(loop, listenAddr)),
      m_nextConnId(1) ,
    _ipPort(listenAddr.toIpPort()){
  p_acceptor->setNewConnectionCallBack(
      std::bind(&TcpServer::newConnetion, this, std::placeholders::_1, std::placeholders::_2));
}
void TcpServer::setThreadNum(int numThreads) {
  assert(0 <= numThreads);
  _threadPool->setThreadNum(numThreads);
}

TcpServer::~TcpServer() {
}

void TcpServer::startListen() {
  assert(!p_acceptor->listenning());
  _threadPool->start();

  p_loop->runInLoop(
      std::bind(&Acceptor::listen, p_acceptor.get()));
}

void TcpServer::newConnetion(int sockfd, const InetAddress& peerAddr) {
  LOG_TRACE << "TcpServer::newConnetion() ";
  p_loop->assertInLoopThread();

  char buf[64];
  snprintf(buf, sizeof buf, "#%d", m_nextConnId);
  ++m_nextConnId;
  std::string connName = _name + buf;

  LOG_INFO << "TcpServer::newConnetion() [" << _name
           << "] - new connection [" << connName
           << "] from " << peerAddr.toIpPort();

  InetAddress localAddr(sockets::getLocalAddr(sockfd));

  //进行分派
  EventLoop* loop;
  loop = _threadPool->getNextLoop();
  LOG_INFO << "Loop " << loop;
  TcpConnectionPtr conn(new TcpConnection(loop,
                                          connName, sockfd, localAddr, peerAddr));
  //m_connectionsMap[connName] = conn;
  m_connectionsSet.insert(conn);
  conn->setConnectionCallBack(m_connectionCallBack);
  conn->setMessageCallBack(m_messageCallBack);
  conn->setCloseCallBack(std::bind(&TcpServer::removeConnection, this, std::placeholders::_1));
  loop->runInLoop(std::bind(&TcpConnection::connectEstablished, conn));
}

void TcpServer::removeConnection(const TcpConnectionPtr& conn) {
  // FIXME: unsafe
  //LOG_TRACE << "trace conn use " << conn->name() << " used count " << conn.use_count();
  p_loop->runInLoop(std::bind(&TcpServer::removeConnectionInLoop, this, conn));
  //LOG_TRACE << "trace conn use " << conn->name() << " used count " << conn.use_count();
}

void TcpServer::removeConnectionInLoop(const TcpConnectionPtr& conn) {
  p_loop->assertInLoopThread();
  LOG_INFO << "TcpServer::removeConnectionInLoop [" << _name
           << "] - connection " << conn->name();

  //LOG_TRACE << "trace conn use " << conn->name() << " used count " << conn.use_count();
  size_t n = m_connectionsSet.erase(conn);
  (void)n;
  assert(n == 1);
  //LOG_TRACE << "trace conn use " << conn->name() << " used count " << conn.use_count();

  if (m_closeCallBack) {
    m_closeCallBack(conn);
  }

  //LOG_TRACE << "trace conn use " << conn->name() << " used count " << conn.use_count();

  //assert(conn.use_count() == 2);

  EventLoop* ioLoop = conn->getLoop();
  ioLoop->queueInLoop(  //´ËÊ±µÄConnÎª×îºóÒ»¸öshared_ptr.Àë¿ª×÷ÓÃÓÚºóÎö¹¹´ËTcpConnection.
      std::bind(&TcpConnection::connectDestroyed, conn));
}
