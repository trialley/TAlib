#include <Logger/Logger.h>
#include <Reactor/Reactor.h>
#include <Tcp/TcpServer.h>

void on_connection(const TA::TcpConnectionPtr& conn) {
	LOG_DEBUG << "new conn from " << conn->peerAddress().toIpPort();
}

void on_message(const TA::TcpConnectionPtr& conn, TA::Buffer* buffer, TimeStamp time) {
	LOG_DEBUG << "on message : "
			  << " bytes " << buffer->peek();  //<< time
	conn->send(buffer->peek());
	buffer->retrieve(buffer->readableBytes());
}

int main() {
	Logger::setLogLevel(Logger::DEBUG);
	//Logger::setAsync (true);

	TA::EventLoop loop;	 //负责管理整个epoll及数据读写
	TA::TcpServer tcp_server(&loop, InetAddress(8080));

	tcp_server.setConnectionCallBack(std::bind(on_connection, std::placeholders::_1));
	tcp_server.setMessageCallBack(std::bind(on_message, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	//tcp_server.setCloseCallBack(std::bind(on_close, std::placeholders::_1));
	tcp_server.startListen();

	loop.loop();
}