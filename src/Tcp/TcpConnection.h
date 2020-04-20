#ifndef _NET_TCPCONNECTION_HH
#define _NET_TCPCONNECTION_HH

#include <Http/HttpContext.h>
#include <Reactor/Channel.h>
#include <Socket/InetAddress.h>
#include <Socket/Socket.h>
#include <common/Buffer.h>
#include <common/CallBacks.h>

#include <memory>
#include <string>

namespace TA {

class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
public:
	TcpConnection(EventLoop* loop,
				  const std::string& name,
				  int sockfd,
				  const InetAddress& localAddr,
				  const InetAddress& peerAddr);
	~TcpConnection();

	EventLoop* getLoop() const { return p_loop; }
	const std::string& name() const { return _name; }
	void setConnectionName(const std::string& name) { _name = name; }
	void setConnectionCallBack(const NetCallBacks::ConnectionCallBack& cb) { m_connectionCallBack = cb; }
	void setMessageCallBack(const NetCallBacks::MessageCallBack& cb) { m_messageCallBack = cb; }
	void setCloseCallBack(const NetCallBacks::CloseCallBack& cb) { m_closeCallBack = cb; }

	const InetAddress& localAddress() const { return m_localAddr; }
	const InetAddress& peerAddress() const { return m_peerAddr; }

	// called when TcpServer accepts a new connection
	void connectEstablished();	// should be called only once
	// called when TcpServer has removed me from its map
	void connectDestroyed();  // should be called only once

	// void send(string&& message); // C++11
	void send(const void* message, size_t len);
	void send(const std::string& message);
	// void send(Buffer&& message); // C++11
	void send(Buffer* message);	 // this one will swap data

	void shutdown();
	void forceClose();

	bool isConnected() const { return m_state == kConnected; }
	bool isDisConnected() const { return m_state == kDisConnected; }
	const char* stateToString() const;
	HttpContext* _context;
	void setContext(HttpContext* context) {
		_context = context;
	}

	HttpContext* getContext() const {
		return _context;
	}

private:
	enum StateE { kDisConnected,
				  kConnecting,
				  kDisConnecting,
				  kConnected,
	};

	void setState(StateE s) { m_state = s; }
	void handleRead();
	void handleWrite();
	void handleError();
	void handleClose();
	void sendInLoop(const void* data, size_t len);
	void shutdownInLoop();
	void forceCloseInLoop();

	EventLoop* p_loop;
	std::string _name;
	StateE m_state;
	std::unique_ptr<Socket> p_socket;
	std::unique_ptr<Channel> p_channel;
	InetAddress m_localAddr;
	InetAddress m_peerAddr;

	NetCallBacks::ConnectionCallBack m_connectionCallBack;
	NetCallBacks::MessageCallBack m_messageCallBack;
	NetCallBacks::CloseCallBack m_closeCallBack;

	Buffer m_inputBuffer;
	Buffer m_outputBuffer;
};

}  // namespace TA

#endif