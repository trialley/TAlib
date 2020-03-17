#include <assert.h>

#include <Reactor/Poller.h>
#include <Reactor/Channel.h>
#include <Reactor/EventLoop.h>

using namespace muduo;

Poller::Poller(EventLoop* loop)
  : p_Loop(loop)
{
}

Poller::~Poller()
{
}

bool Poller::hasChannel(Channel* channel) const
{
  assertInLoopThread();
  ChannelMap::const_iterator it = m_channels.find(channel->fd());
  return it != m_channels.end() && it->second == channel;
}

void Poller::assertInLoopThread() const
{
  p_Loop->assertInLoopThread();
}