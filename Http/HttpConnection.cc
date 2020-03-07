#include "HttpConnection.h"

void HttpConnection::initmsg() {
  path = "";
  filetype = "";
  keepalive = false;
}

HttpConnection::HttpConnection(SP_Channel Channel)
    : channel(Channel),
      storage("./www/"),
      parsestate(PARSE_METHOD),
      pos(0),
      inbuffer(""),
      path(""),
      filetype(""),
      keepalive(false),
      size(0) {
  handleparse[0] = bind(&HttpConnection::parseError, this);
  handleparse[1] = bind(&HttpConnection::parseMethod, this);
  handleparse[2] = bind(&HttpConnection::parseHeader, this);
  handleparse[3] = bind(&HttpConnection::parseSuccess, this);
  channel->setReadhandler(bind(&HttpConnection::parse, this));
}

HttpConnection::~HttpConnection() {
}

/*
 从inbuffer中找到从pos开始的第一个c字符
 */
bool HttpConnection::Read(string &msg, string str) {
  int next = inbuffer.find(str, pos);
  if (string::npos == next)
    return false;
  msg = inbuffer.substr(pos, next - pos);
  pos = next + str.length();
  return true;
}

PARSESTATE HttpConnection::parseMethod() {
  string msg;
  if (!Read(msg, " /"))
    return PARSE_ERROR;
  else if ("GET" == msg)
    method = METHOD_GET;
  else if ("POST" == msg)
    method = METHOD_POST;
  else
    return PARSE_ERROR;
  if (!Read(path, " "))
    return PARSE_ERROR;
  struct stat sbuf;
  if ("" == path)
    path = "index.html";
  if (stat((storage + path).c_str(), &sbuf) < 0) {
    LOG_INFO << "no file";
    parsestate = PARSE_ERROR;
    return PARSE_ERROR;
  }
  size = sbuf.st_size;
  if (!Read(msg, "\r\n"))
    return PARSE_ERROR;
  else if ("HTTP/1.0" == msg)
    version = HTTP_10;
  else if ("HTTP/1.1" == msg)
    version = HTTP_11;
  else
    return PARSE_ERROR;
  return PARSE_HEADER;
}

PARSESTATE HttpConnection::parseHeader() {
  if (inbuffer[pos] == '\r' && inbuffer[pos + 1] == '\n')
    return PARSE_SUCCESS;
  string key, value;
  if (!Read(key, ": "))
    return PARSE_ERROR;
  if (!Read(value, "\r\n"))
    return PARSE_ERROR;
  header[key] = value;
  return PARSE_HEADER;
}

/*CONTENTSTATE HttpConnection::parseContent(){
//暂时只处理get,get没有正文部分
}*/

PARSESTATE HttpConnection::parseError() {
  LOG_INFO << "parse error";
  inbuffer = "";
  pos = 0;
  initmsg();
  header.clear();
  channel->setRevents(EPOLLOUT | EPOLLET);
  channel->getLoop().lock()->updatePoller(channel);
  channel->setWritehandler(bind(&HttpConnection::handleError, this, 400, "Bad Request"));
  return PARSE_METHOD;
}

PARSESTATE HttpConnection::parseSuccess() {
  channel->setRevents(EPOLLOUT | EPOLLET);
  channel->getLoop().lock()->updatePoller(channel);
  inbuffer = inbuffer.substr(pos + 2);
  pos = 0;
  if (HTTP_11 == version && header.find("Connection") != header.end() && ("Keep-Alive" == header["Connection"] || "keep-alive" == header["Connection"]))
    keepalive = true;
  int dot_pos = path.rfind('.');  //获取后面的扩展名，防止xxx.min.css情况
  if (string::npos == dot_pos)
    filetype = Mimetype::getMime("default");
  else
    filetype = Mimetype::getMime(path.substr(dot_pos));
  channel->setWritehandler(bind(&HttpConnection::send, this));
  header.clear();
  return PARSE_METHOD;
}

void HttpConnection::parse() {
  bool zero = false;
  int readsum;
  readsum = readn(channel->getFd(), inbuffer, zero);
  if (readsum < 0 || zero) {
    initmsg();
    channel->setDeleted(true);
    channel->getLoop().lock()->addTimer(channel, 0);
    return;
  }
  while (inbuffer.length() && ~inbuffer.find("\r\n", pos))
    parsestate = handleparse[parsestate]();
}

void HttpConnection::send() {
  string outbuffer = "";
  if (METHOD_POST == method) {
  } else if (METHOD_GET == method) {
    if (keepalive) {
      outbuffer = "HTTP/1.1 200 OK\r\n";
      outbuffer += string("Connection: Keep-Alive\r\n") + "Keep-Alive: timeout=" + to_string(20000) + "\r\n";
      channel->getLoop().lock()->addTimer(channel, 20000);
    } else {
      outbuffer = "HTTP/1.0 200 OK\r\n";
      channel->getLoop().lock()->addTimer(channel, 0);
    }
    outbuffer += "Content-Type: " + filetype + "\r\n";
    outbuffer += "Content-Length: " + to_string(size) + "\r\n";
    outbuffer += "Server: WWQ's Web Server\r\n";
    outbuffer += "\r\n";
  }
  const char *buffer = outbuffer.c_str();
  {
    if (!writen(channel->getFd(), buffer, outbuffer.length()))
      LOG_INFO << "writen error";
  }
  initmsg();
  channel->setRevents(EPOLLIN | EPOLLET);
  channel->getLoop().lock()->updatePoller(channel);
}

void HttpConnection::handleError(int errornum, string msg) {  //暂时统一用400,Bad Request来处理,留接口可在上层修改错误码和错误信息
  string body = "<html><title>哎~出错了</title>";
  body += "<head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"></head>";
  body += "<body bgcolor=\"ffffff\">";
  body += to_string(errornum) + msg;
  body += "<hr><em> WWQ's Web Server</em>\n</body></html>";
  string outbuffer = "HTTP/1.1 " + to_string(errornum) + msg + "\r\n";
  outbuffer += "Content-Type: text/html\r\n";
  outbuffer += "Connection: Close\r\n";
  outbuffer += "Content-Length: " + to_string(body.size()) + "\r\n";
  outbuffer += "Server: WWQ's Web Server\r\n";
  ;
  outbuffer += "\r\n";
  outbuffer += body;
  const char *buffer = outbuffer.c_str();

  {
    if (!writen(channel->getFd(), buffer, outbuffer.length()))
      LOG_INFO << "writen error";
  }
  channel->setRevents(EPOLLIN | EPOLLET);
  channel->getLoop().lock()->updatePoller(channel);
}
