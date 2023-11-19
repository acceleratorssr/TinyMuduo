#ifndef _ACCEPTOR_HPP_
#define _ACCEPTOR_HPP_

#include "noncopyable.hpp"
#include "Socket.hpp"
#include "Channel.hpp"

class EventLoop;
class inetAddress;

class Acceptor
{
public:
    using NewConnectionCallback = std::function<void(int sockfd, const inetAddress &)>;

    Acceptor(EventLoop *loop, const inetAddress &listenAddr, bool reuseport);
    ~Acceptor();

    void setNewConnectionCallback(const NewConnectionCallback &cb)
    {
        newconnectioncallback_ = std::move(cb);
    }

    bool listenning() const { return listenning_; }
    void listen();

private:
    void handleRead();

    EventLoop *loop_;
    Socket acceptSocket_;
    Channel acceptChannel_;
    NewConnectionCallback newconnectioncallback_;
    bool listenning_;
};

#endif // _ACCEPTOR_HPP_
