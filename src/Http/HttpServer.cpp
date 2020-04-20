// Copyright 2010, Shuo Chen.  All rights reserved.
// http://code.google.com/p/TA/
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

// Author: Shuo Chen (chenshuo at chenshuo dot com)
//

#include <Http/HttpServer.h>
#include <Logger/Logger.h>
#include <Tcp/TcpServer.h>
#include <common/noncopyable.h>

#include <functional>
#include <string>
using namespace TA;
using namespace std;

void defaultHttpCallback(const HttpRequest&, HttpResponse* resp) {
	resp->setStatusCode(HttpResponse::k404NotFound);
	resp->setStatusMessage("Not Found");
	resp->setCloseConnection(true);
}

HttpServer::HttpServer(EventLoop* loop,
					   const InetAddress& listenAddr,
					   const string& name)
	: server_(loop, listenAddr, name),
	  httpCallback_(defaultHttpCallback) {
	server_.setConnectionCallBack(
		std::bind(&HttpServer::onConnection, this, std::placeholders::_1));
	server_.setMessageCallBack(
		std::bind(&HttpServer::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

void HttpServer::start() {
	LOG_WARN << "HttpServer[" << server_.getName()
			 << "] starts listenning on " << server_.getipPort();
	server_.startListen();
}

void HttpServer::onConnection(const TcpConnectionPtr& conn) {
	LOG_INFO << "on connection";
	if (conn->isConnected()) {
		conn->setContext(new HttpContext());
	}
}

void HttpServer::onMessage(const TcpConnectionPtr& conn,
						   Buffer* buf,	 //ssize_t size,
						   TimeStamp receiveTime) {
	HttpContext* context = (HttpContext*)(conn->_context);	//TODOC++Ö¸Õë

	if (!context->parseRequest(buf, receiveTime)) {
		conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
		conn->shutdown();
	}

	if (context->gotAll()) {
		onRequest(conn, context->request());
		context->reset();
	}
}

void HttpServer::onRequest(const TcpConnectionPtr& conn, const HttpRequest& req) {
	LOG_INFO << "on req";

	const string& connection = req.getHeader("Connection");
	bool close = connection == "close" ||
				 (req.getVersion() == HttpRequest::kHttp10 && connection != "Keep-Alive");
	HttpResponse response(close);
	httpCallback_(req, &response);
	Buffer buf;
	response.appendToBuffer(&buf);
	conn->send(&buf);
	if (response.closeConnection()) {
		conn->shutdown();
	}
}
