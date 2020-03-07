#include "Server.h"

Server::Server(const char *port, int threadnum)
    : loop(newElement<EventLoop>(), deleteElement<EventLoop>),
      serverchannel(newElement<Channel>(loop), deleteElement<Channel>),
      iothreadpool(newElement<ThreadpoolEventLoop>(threadnum), deleteElement<ThreadpoolEventLoop>) {
  listenfd = tcp_listen(NULL, port, NULL);
  setnonblocking(listenfd);
  serverchannel->setFd(listenfd);
}

Server::~Server() {
  Close(listenfd);
}

void Server::handleconn() {
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
    connchannel->setClosehandler(bind(&Server::handleclose, this, wpchannel));
    {
      connchannel->setRevents(EPOLLIN | EPOLLET);
      SP_HttpConnection connhttp(newElement<HttpConnection>(connchannel), deleteElement<HttpConnection>);
      Httpmap[connfd] = move(connhttp);
    }
    nextloop->queueInLoop(bind(&EventLoop::addPoller, nextloop, move(connchannel)));
  }
}

void Server::start() {
  iothreadpool->start();
  serverchannel->setRevents(EPOLLIN | EPOLLET);
  serverchannel->setReadhandler(bind(&Server::handleconn, this));
  loop->addPoller(serverchannel);
  LOG_INFO << "start";
  loop->loop();
}

void Server::handleclose(WP_Channel channel) {
  SP_Channel spchannel = channel.lock();
  loop->queueInLoop(bind(&Server::deletemap, this, spchannel));
  spchannel->getLoop().lock()->removePoller(spchannel);
}

void Server::deletemap(SP_Channel channel) {
  Httpmap.erase(channel->getFd());
}
