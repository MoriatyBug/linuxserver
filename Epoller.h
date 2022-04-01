#ifndef _EPOLLER_H_
#define _EPOLLER_H_

#include "Channel.h"
#include "util/Util.h"
#include "HttpProcesser.h"
#include "Timer.h"

class Channel;
class TimerManager;
typedef shared_ptr<Channel> SHARED_PTR_CHANNEL;
const int EPOLLWAIT_TIME = 10000;
class Epoller
{
private:
    static const int MAXFDNUMS = 10000;
    int epoll_fd_;
    vector<epoll_event> events_;
    // 用来反向查找 channel
    SHARED_PTR_CHANNEL epollFdToChannel[MAXFDNUMS]; 
    // 用来反向查找 httpProcesser
    shared_ptr<HttpProcesser> epollFdToHttpProcesser[MAXFDNUMS];
    unique_ptr<TimerManager> timer_manager_;

public:
    Epoller();
    ~Epoller();
    void epollAdd(SHARED_PTR_CHANNEL channel, int timeout = 0);
    void epollDelete(SHARED_PTR_CHANNEL channel);
    void epollUpdate(SHARED_PTR_CHANNEL channel, int timeout = 0);
    /* 返回活跃的事件对应的 channel 列表，一个 channel 维护一个 fd */
    vector<SHARED_PTR_CHANNEL> poll();
    int getEpollFd() {
        return epoll_fd_;
    }
    vector<SHARED_PTR_CHANNEL> getActiveEvents(int eventCount); // std move
    void handleExpire();
    void addTimer(SHARED_PTR_CHANNEL channel, int timeout);
};

#endif