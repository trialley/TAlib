#include "TcpServer.h"

TcpServer::TcpServer(const char *port, int threadnum)
    : loop(newElement<EventLoop>(), deleteElement<EventLoop>),
      TcpServerchannel(newElement<Channel>(loop), deleteElement<Channel>),
      iothreadpool(newElement<ThreadpoolEventLoop>(threadnum), deleteElement<ThreadpoolEventLoop>) {
  listenfd = tcp_listen(NULL, port, NULL);
  setnonblocking(listenfd);
  TcpServerchannel->setFd(listenfd);

}

TcpServer::~TcpServer() {
  Close(listenfd);
}
void TcpServer::setOnMessageCallback(TcpConnection::OnMessageCallback in) {
  _onMessageCallback = in;
}
void TcpServer::handleconn() {
  struct sockaddr_storage cliaddr;
  socklen_t clilen = sizeof(cliaddr);
  int connfd;
  while ((connfd = Accept(listenfd, (SA *)&cliaddr, &clilen)) >= 0) {
    LOG_INFO << "accept fd=" << connfd;
    setnonblocking(connfd);
    SP_EventLoop nextloop = iothreadpool->getNextloop();
    SP_Channel connchannel(newElement<Channel>(nextloop), deleteElement<Channel>);
    connchannel->setFd(connfd);
    WP_Channel wpchannel = connchannel;
    connchannel->setClosehandler(bind(&TcpServer::handleclose, this, wpchannel));
    {
      connchannel->setRevents(EPOLLIN | EPOLLET);
      SP_TcpConnection connTcp(newElement<TcpConnection>(connchannel), deleteElement<TcpConnection>);
      connTcp->setOnMessageCallback(_onMessageCallback);
      _TcpMap[connfd] = move(connTcp);
    }
    nextloop->queueInLoop(bind(&EventLoop::addPoller, nextloop, move(connchannel)));
  }
}

void TcpServer::start() {
  iothreadpool->start();
  TcpServerchannel->setRevents(EPOLLIN | EPOLLET);
  TcpServerchannel->setReadhandler(bind(&TcpServer::handleconn, this));
  loop->addPoller(TcpServerchannel);
  LOG_INFO << "start";
  loop->loop();
}

void TcpServer::handleclose(WP_Channel channel) {
  SP_Channel spchannel = channel.lock();
  loop->queueInLoop(bind(&TcpServer::deletemap, this, spchannel));
  spchannel->getLoop().lock()->removePoller(spchannel);
}

void TcpServer::deletemap(SP_Channel channel) {
  _TcpMap.erase(channel->getFd());
}
