#ifndef _NET_POLLER_HH
#define _NET_POLLER_HH

#include <common/TimeStamp.h>

#include <map>
#include <vector>

namespace TA {

class EventLoop;
class Channel;

///
/// Base class for IO Multiplexing
///
/// This class doesn't own the Channel objects.
class Poller {	//poller是疯转epoll的抽象类
public:
	typedef std::vector<Channel*> ChannelList;

	Poller(EventLoop* loop);
	virtual ~Poller();

	/// Polls the I/O events.
	/// Must be called in the loop thread.
	virtual TimeStamp poll(int timeoutMs, ChannelList* activeChannels) = 0;

	/// Changes the interested I/O events.
	/// Must be called in the loop thread.
	virtual void updateChannel(Channel* channel) = 0;

	/// Remove the channel, when it destructs.
	/// Must be called in the loop thread.
	virtual void removeChannel(Channel* channel) = 0;

	virtual bool hasChannel(Channel* channel) const;

	static Poller* newDefaultPoller(EventLoop* loop);

	void assertInLoopThread() const;

protected:
	Poller(const Poller&);
	const Poller& operator=(const Poller&);

	typedef std::map<int, Channel*> ChannelMap;
	ChannelMap m_channels;

private:
	EventLoop* p_Loop;
};

}  // namespace TA

#endif
