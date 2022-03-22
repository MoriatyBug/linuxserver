#ifndef _CHANNEL_H_
#define _CHANNEL_H_

#include <functional>
#include <memory>
#include <sys/epoll.h>
#include "HttpProcesser.h"
#include "EventLoop.h"

using namespace std;

typedef __uint32_t UINT32;
typedef shared_ptr<Channel> SHARED_PTR_CHANNEL;

class Channel
{
private:
    typedef function<void()> CallBack;
    int fd_;
    UINT32 events_;
    UINT32 revents_;
    
    weak_ptr<HttpProcesser> holder_; // 设置为虚指针从而不会影响 channel 的引用计数
    CallBack read_handler_;
    CallBack write_handler_;
    CallBack error_handler_;
    CallBack conn_handler_;

public:
    Channel(EventLoop* loop);
    Channel(EventLoop* loop, int fd);
    int getFd();
    void setFd(int fd);

    shared_ptr<HttpProcesser> getHolder() {
        shared_ptr<HttpProcesser> ret(holder_.lock()); // ?啥意思
        return ret;
    }

    void setReadHandler(CallBack &&readHandler) {
        read_handler_ = readHandler;
    }

    void setWriteHandler(CallBack &&writeHandler) {
        write_handler_ = writeHandler;
    }

    void setErrorHandler(CallBack &&errorHandler) {
        error_handler_ = errorHandler;
    }

    void setConnHandler(CallBack &&connHandler) {
        conn_handler_ = connHandler;
    }

    void handleRead();
    void handleWrite();
    void handleConn();
    void handleError();
    void setEvents(UINT32 events) {
        events_ = events;
    }

    void setRevents(UINT32 revents) {
        revents_ = revents;
    }

    UINT32 &getEvents() {
        return events_;
    }

    void handleEvents() {
        events_ = 0;
        if ((revents_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP))) {
            handleRead();
        }
    }
};

#endif // _CHANNEL_H_