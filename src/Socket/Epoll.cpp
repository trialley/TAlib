#include <Logger/Logger.h>
#include <Reactor/Channel.h>
#include <Reactor/EventLoop.h>
#include <Socket/Epoll.h>
#include <assert.h>
#include <sys/epoll.h>

using namespace TA;

namespace EventFd {	 //命名空间EventFd包含了一些epoll函数的封装

int createEpollFd() {
	int epfd = epoll_create1(EPOLL_CLOEXEC);
	if (epfd < 0) {
		LOG_FATAL << "createEpollFd() Failed.";
	}

	return epfd;
}

void epollRegister(int epfd, int fd, struct epoll_event* ev) {
	int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, fd, ev);
	if (ret < 0) {
		LOG_FATAL << "epollRegister() Failed.";
	}
}

void epollUpdate(int epfd, int fd, struct epoll_event* ev) {
	int ret = epoll_ctl(epfd, EPOLL_CTL_MOD, fd, ev);
	if (ret < 0) {
		LOG_FATAL << "epollUpdate() Failed.";
	}
}

void epollDelete(int epfd, int fd, struct epoll_event* ev) {
	int ret = epoll_ctl(epfd, EPOLL_CTL_DEL, fd, ev);
	if (ret < 0) {
		LOG_FATAL << "epollDelete() Failed.";
	}
}

}  // namespace EventFd

const int Epoll::kMaxEpollConcurrencySize = 10000;
const int Epoll::kInitEpollEventsSize = 1024;

Epoll::Epoll(EventLoop* loop)
	: Poller(loop),
	  m_epfd(EventFd::createEpollFd()),
	  m_maxEventsSize(0),
	  m_epollEvents(kInitEpollEventsSize) {
}

Epoll::~Epoll() {
}

TimeStamp Epoll::poll(int timeoutMs, ChannelList* activeChannels) {
	LOG_TRACE << "Epoll::poll() maxConcurrencySize " << m_maxEventsSize;
	assert(m_epollEvents.size() > 0);
	int numEvents = ::epoll_wait(m_epfd, m_epollEvents.data(), m_maxEventsSize, timeoutMs);	 // epoll句柄 事件数组 事件数量 超时事件
																							 /*
m_epfd为用epoll_create创建之后的句柄
events是一个epoll_event*的指针，当epoll_wait这个函数操作成功之后，epoll_events里面将储存所有的读写事件。
max_events是当前需要监听的所有socket句柄数。
最后一个timeout是 epoll_wait的超时，为0的时候表示马上返回，为-1的时候表示一直等下去，直到有事件范围，为任意正整数的时候表示等这么长的时间，如果一直没有事件，则范围。一般如果网络主循环是单独的线程的话，可以用-1来等，这样可以保证一些效率，如果是和主逻辑在同一个线程的话，则可以用0来保证主循环的效率。
typedef union epoll_data {//传入
    void *ptr;//自定义
    int fd;//监听句柄
    __uint32_t u32;
    __uint64_t u64;
} epoll_data_t;

struct epoll_event {
    __uint32_t events; //返回结果
		EPOLLIN ：表示对应的文件描述符可以读（包括对端SOCKET正常关闭）；
		EPOLLOUT：表示对应的文件描述符可以写；
		EPOLLPRI：表示对应的文件描述符有紧急的数据可读（这里应该表示有带外数据到来）；
		EPOLLERR：表示对应的文件描述符发生错误；
		EPOLLHUP：表示对应的文件描述符被挂断；
	epoll_data_t data;//用户自己的数据
};

*/
	TimeStamp now(TimeStamp::now());
	if (numEvents > 0) {
		LOG_TRACE << numEvents << " events happended";
		fillActiveChannels(numEvents, activeChannels);
	} else if (numEvents == 0) {
		LOG_TRACE << " nothing happended";
	} else {
		LOG_ERROR << "Epoll::epoll_wait()" << numEvents << "  " << strerror(errno);
	}

	return now;
}

/*
 *fillActiveChannels() m_epollEvents 填充活动事件的fd， 把它对应
 *的Channel填入activeChannels。
 */

void Epoll::fillActiveChannels(int numEvents, ChannelList* activeChannels) const {
	for (EpollFdList::const_iterator it = m_epollEvents.begin();
		 it != m_epollEvents.end() && numEvents > 0; ++it) {
		assert(it->events > 0);
		--numEvents;
		ChannelMap::const_iterator ch = m_channels.find(it->data.fd);  //找到相应的文件描述符
		assert(ch != m_channels.end());
		Channel* channel = ch->second;
		assert(channel->fd() == it->data.fd);
		channel->set_revents(it->events);
		activeChannels->push_back(channel);	 //该channal被激活
	}
}

void Epoll::updateChannel(Channel* channel) {
	assertInLoopThread();
	LOG_TRACE << "fd= " << channel->fd() << " events " << channel->events();
	if (channel->index() < 0) {
		//a new one , add to epollEvents
		assert(m_channels.find(channel->fd()) == m_channels.end());
		struct epoll_event ev;
		ev.data.fd = channel->fd();
		ev.events = static_cast<uint32_t>(channel->events());
		EventFd::epollRegister(m_epfd, channel->fd(), &ev);
		m_maxEventsSize++;
		int idx = m_maxEventsSize;	//poll used, epoll not have significance, just a tag.
		channel->set_index(idx);
		m_channels[channel->fd()] = channel;
	} else {
		//update existing one
		assert(m_channels.find(channel->fd()) != m_channels.end());
		assert(m_channels[channel->fd()] == channel);
		struct epoll_event ev;
		ev.data.fd = channel->fd();
		ev.events = static_cast<uint32_t>(channel->events());
		EventFd::epollUpdate(m_epfd, channel->fd(), &ev);
	}

	if (m_maxEventsSize + 1 == m_epollEvents.capacity()) {
		int size = m_epollEvents.capacity();
		size = std::min(2 * size, kMaxEpollConcurrencySize);
		m_epollEvents.reserve(size);
	}
}

void Epoll::removeChannel(Channel* channel) {
	assertInLoopThread();
	LOG_TRACE << "fd = " << channel->fd();
	assert(m_channels.find(channel->fd()) != m_channels.end());
	assert(m_channels[channel->fd()] == channel);
	assert(channel->isNoneEvent());
	struct epoll_event ev;
	ev.data.fd = channel->fd();
	LOG_TRACE << "Epoll::removeChannel() fd " << channel->fd() << " events " << channel->events();
	EventFd::epollDelete(m_epfd, channel->fd(), &ev);
	size_t n = m_channels.erase(channel->fd());
	assert(n == 1);
	(void)n;

	m_maxEventsSize--;
}
