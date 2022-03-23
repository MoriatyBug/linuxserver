#ifndef _EPOLLER_H_
#define _EPOLLER_H_
#include "util/util.h"

class HttpProcesser;
class Channel;
typedef shared_ptr<Channel> SHARED_PTR_CHANNEL;

class Epoller
{
public:
    void epollAdd(SHARED_PTR_CHANNEL channel, int timeout = 0);
    void epollDelete(SHARED_PTR_CHANNEL channel);
    void epollUpdate(SHARED_PTR_CHANNEL channel, int timeout = 0);
    /* 返回活跃的事件对应的 channel 列表，一个 channel 维护一个 fd */
    vector<SHARED_PTR_CHANNEL> poll();
    int getEpollFd() {
        return epoll_fd_;
    }


private:
    static const int MAXFDNUMS = 10000;
    int epoll_fd_;
    SHARED_PTR_CHANNEL epollFdToChannel[MAXFDNUMS]; // 用来反向查找 channel
    shared_ptr<HttpProcesser> epollFdToHttpProcesser[MAXFDNUMS]; // 用来反向查找 httpProcesser
};

#endif