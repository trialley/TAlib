#ifndef _NET_TCPSERVER_HH
#define _NET_TCPSERVER_HH

#include <functional>
#include <memory>
#include <set>
#include<string>
#include <Reactor/Acceptor.h>
#include <Reactor/EventLoopThreadPool.h>
#include <Tcp/TcpConnection.h>
#include <common/CallBacks.h>

namespace TA {

class EventLoop;

class TcpServer {
 public:
  TcpServer(EventLoop* loop, const InetAddress& listenAddr, const std::string& name = "Serv ");
  ~TcpServer();

  void startListen();

  void setConnectionCallBack(const NetCallBacks::ConnectionCallBack& cb) { m_connectionCallBack = cb; }
  void setMessageCallBack(const NetCallBacks::MessageCallBack& cb) { m_messageCallBack = cb; }
  void setCloseCallBack(const NetCallBacks::CloseCallBack& cb) { m_closeCallBack = cb; }
  void setThreadNum(int numThreads);
  EventLoop* getLoop() const { return p_loop; }
  std::string getName () { return _name; }
  std::string& getipPort () {
      return _ipPort;
  }
 private:
     std::string _ipPort;
  TcpServer& operator=(const TcpServer&);
  TcpServer(const TcpServer&);
  void newConnetion(int sockfd, const InetAddress& peerAddr);
  void removeConnection(const TcpConnectionPtr& conn);
  void removeConnectionInLoop(const TcpConnectionPtr& conn);

  //typedef std::map<std::string, TcpConnectionPtr> ConnectionMap;
  typedef std::set<TcpConnectionPtr> ConnectionsSet_t;

  EventLoop* p_loop;
  std::string _name;
  std::shared_ptr<EventLoopThreadPool> _threadPool;
  std::unique_ptr<Acceptor> p_acceptor;
  //ConnectionMap m_connectionsMap;
  ConnectionsSet_t m_connectionsSet;
  NetCallBacks::ConnectionCallBack m_connectionCallBack;
  NetCallBacks::MessageCallBack m_messageCallBack;
  NetCallBacks::CloseCallBack m_closeCallBack;
  int m_nextConnId;
};

}  // namespace TA

#endif