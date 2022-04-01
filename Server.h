#ifndef _SERVER_H_
#define _SERVER_H_

#include "EventLoop.h"
#include "EventLoopThreadPool.h"
#include "util/Util.h"

class Server
{
private:
    EventLoop *event_loop_;
    int thread_num_;
    unique_ptr<EventLoopThreadPool> thread_pool_;
    bool is_started_;
    int port_;
    int listen_fd_;
    shared_ptr<Channel> channel_;

public:
    Server(EventLoop *eventLoop, int threadNum, int port);
    ~Server(){};
    EventLoop *getLoop() const {
        return event_loop_;
    }
    void start();
    void handleNewConn();
    void updateEpoll();
};
#endif // _SERVER_H_