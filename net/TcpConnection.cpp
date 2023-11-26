#include "TcpConnection.hpp"
#include "log.hpp"
#include "Socket.hpp"
#include "Channel.hpp"
#include "EventLoop.hpp"

#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <unistd.h>

static EventLoop *CheckLoopNotNull(EventLoop *loop)
{
    if (loop == nullptr)
    {
        LOG_FATAL("%s:%s:%d TcpConnection Loop is null", __FILE__, __FUNCTION__, __LINE__);
    }
    return loop;
}

TcpConnection::TcpConnection(EventLoop *loop,
                             const std::string &nameArg,
                             int sockfd,
                             const inetAddress &localAddr,
                             const inetAddress &peerAddr)
    : loop_(CheckLoopNotNull(loop)),
      name_(nameArg),
      state_(kConnecting),
      reading_(true),
      socket_(new Socket(sockfd)),
      channel_(new Channel(loop, sockfd)),
      localAddr_(localAddr),
      peerAddr_(peerAddr),
      highWaterMark_(64 * 1024 * 1024)
{
    channel_->setReadCallback(
        std::bind(&TcpConnection::handleRead, this, std::placeholders::_1));
    channel_->setWriteCallback(
        std::bind(&TcpConnection::handleWrite, this));
    channel_->setCloseCallback(
        std::bind(&TcpConnection::handleClose, this));
    channel_->setErrorCallback(
        std::bind(&TcpConnection::handleError, this));

    LOG_INFO("TcpConnection::ctor[%s] at fd=%d", name_.c_str(), sockfd);
    socket_->setKeepAlive(true);
}

TcpConnection::~TcpConnection()
{
    LOG_INFO("TcpConnection::dtor[%s] at fd=%d state=%d",
             name_.c_str(), channel_->GetFd(), (int)state_);
}

void TcpConnection::send(const std::string &buf)
{
    if (state_ == kConnected)
    {
        if (loop_->isInLoopThread())
            sendInLoop(buf.c_str(), buf.size());
        else
        {
            loop_->runInLoop(std::bind(
                &TcpConnection::sendInLoop,
                this,
                buf.c_str(),
                buf.size()));
        }
    }
}

void TcpConnection::shutdown()
{
    if (state_ == kConnected)
    {
        TcpConnection::setState(kDisconnecting);
        loop_->runInLoop(
            std::bind(&TcpConnection::shutdownInLoop, this));
    }
}

void TcpConnection::connectEstablished()
{
    TcpConnection::setState(kConnected);
    channel_->tie(shared_from_this());
    channel_->EnableReading();
    connectionCallback_(shared_from_this());
}

void TcpConnection::connectDestroyed()
{
    if (state_ == kConnected)
    {
        TcpConnection::setState(kDisconnected);
        channel_->DisableAll();
        connectionCallback_(shared_from_this());
    }
    channel_->remove();
}

void TcpConnection::handleRead(Timestamp receiveTime)
{
    int savedErrno = 0;
    ssize_t n = inputBuffer_.readFd(channel_->GetFd(), &savedErrno);
    if (n > 0)
        messageCallback_(shared_from_this(), &inputBuffer_, receiveTime);
    else if (n == 0)
        handleClose();
    else
    {
        errno = savedErrno;
        LOG_ERROR("TcpConnection::handleRead: %d", errno);
        handleError();
    }
}

void TcpConnection::handleWrite()
{
    if (channel_->isWriteEvent())
    {
        int savedErrno = 0;
        ssize_t n = outputBuffer_.writeFd(channel_->GetFd(), &savedErrno);
        if (n > 0)
        {
            outputBuffer_.retrieve(n);
            if (outputBuffer_.readableBytes() == 0)
            {
                channel_->DisableWriting();
                if (writeCompleteCallback_)
                {
                    loop_->QueueInLoop(
                        std::bind(writeCompleteCallback_, shared_from_this()));
                }
                if (state_ == kDisconnecting)
                    shutdownInLoop();
            }
        }
        else
            LOG_ERROR("TcpConnection::handleWrite");
    }
    else
        LOG_ERROR("TcpConnection fd=%d is down, no more writing", channel_->GetFd());
}

void TcpConnection::handleClose()
{
    LOG_INFO("TcpConnection::handleClose fd=%d state=%d", channel_->GetFd(), (int)state_);
    TcpConnection::setState(kDisconnected);
    channel_->DisableAll();

    TcpConnectionPtr connPtr(shared_from_this());
    connectionCallback_(connPtr);
    closeCallback_(connPtr);
}

void TcpConnection::handleError()
{
    int optval;
    socklen_t optlen = sizeof(optval);
    int err = 0;
    if (::getsockopt(channel_->GetFd(), SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0)
        err = errno;
    else
        err = optval;
    LOG_ERROR("TcpConnection::handleError name:%s -> SO_ERROR:%d", name_.c_str(), err);
}

void TcpConnection::sendInLoop(const void *data, size_t len)
{
    ssize_t nwrote = 0;
    size_t remaining = len;
    bool faultError = false;

    if (state_ == kDisconnected)
    {
        LOG_ERROR("disconnected, give up writing");
        return;
    }

    if (!channel_->isWriteEvent() && outputBuffer_.readableBytes() == 0)
    {
        nwrote = ::write(channel_->GetFd(), data, len);
        if (nwrote >= 0)
        {
            remaining = len - nwrote;
            if (remaining == 0 && writeCompleteCallback_)
            {
                loop_->QueueInLoop(
                    std::bind(writeCompleteCallback_, shared_from_this()));
            }
        }
        else
        {
            nwrote = 0;
            if (errno != EWOULDBLOCK)
            {
                LOG_ERROR("TcpConnection::sendInLoop: %d", errno);
                if (errno == EPIPE || errno == ECONNRESET)
                    faultError = true;
            }
        }
    }

    if (!faultError && remaining > 0)
    {
        size_t oldLen = outputBuffer_.readableBytes();
        if (oldLen + remaining >= highWaterMark_ && oldLen < highWaterMark_ && highWaterMarkCallback_)
        {
            loop_->QueueInLoop(
                std::bind(highWaterMarkCallback_, shared_from_this(), oldLen + remaining));
        }

        outputBuffer_.append((char *)data + nwrote, remaining);

        if (!channel_->isWriteEvent())
            channel_->EnableWriting();
    }
}

void TcpConnection::shutdownInLoop()
{
    if (!channel_->isWriteEvent())
        socket_->shutdownWrite();
}
