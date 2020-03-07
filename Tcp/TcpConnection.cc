#include "./TcpConnection.h"
TcpConnection::TcpConnection(SP_Channel Channel)
    : channel(Channel) {
  channel->setReadhandler(bind(&TcpConnection::handleRead, this));
  channel->setWritehandler(bind(&TcpConnection::handleSend, this));
}

TcpConnection::~TcpConnection() {
}

void TcpConnection::handleRead() {
  _inbuffer = "";
  bool zero = false;
  int readsum;
  readsum = readn(channel->getFd(), _inbuffer, zero);
  if (readsum < 0 || zero) {
    channel->setDeleted(true);
    channel->getLoop().lock()->addTimer(channel, 0);
  } else {
    channel->setRevents(EPOLLOUT | EPOLLET);
    channel->getLoop().lock()->updatePoller(channel);
    _onMessageCallback(this, _inbuffer);
  }
}
void TcpConnection::setSend(string& buffer_out) {
  _outbuffer = buffer_out;
}
void TcpConnection::setOnMessageCallback(TcpConnection::OnMessageCallback in) {
  _onMessageCallback = in;
}
void TcpConnection::handleSend() {
  const char* buffer = _outbuffer.c_str();
  if (!writen(channel->getFd(), buffer, _outbuffer.length()))
    LOG_INFO << "writen error";
  channel->setRevents(EPOLLIN | EPOLLET);
  channel->getLoop().lock()->updatePoller(channel);
}