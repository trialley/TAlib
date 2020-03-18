#ifndef NET_CALLBACKS_H
#define NET_CALLBACKS_H

#include <functional>
#include <memory>
#include<common/TimeStamp.h>
//#include "TimeStamp.h>

namespace TA {

class Buffer;
class TcpConnection;
typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;

namespace NetCallBacks
{

// All client visible callbacks go here.

typedef std::function<void()> TimerCallBack;
typedef std::function<void(const TcpConnectionPtr& )> ConnectionCallBack;
typedef std::function<void(const TcpConnectionPtr& , Buffer*, TimeStamp )> MessageCallBack;
typedef std::function<void(const TcpConnectionPtr& )> CloseCallBack;

void defaultConnectionCallback();

}

};

#endif  // NET_CALLBACKS_H
