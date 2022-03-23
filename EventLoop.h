#ifndef _EVENTLOOP_H_
#define _EVENTLOOP_H_

#include "Epoller.h"
#include "util/util.h"
#include <assert.h>

class Channel;
typedef shared_ptr<Channel> SHARED_PTR_CHANNEL;

class EventLoop
{
public:
    void loop();
    void quit();
    void runInLoop(CallBack &&functor);
    void queueInLoop(CallBack &&functor);
    bool isInLoopThread() {
        return thread_id_ == 0;
    }

    void assertInLoopThread() {
        assert(isInLoopThread());
    }

    void shutdown(SHARED_PTR_CHANNEL channel) {
        // shutdownWR(channel->getFd());
    }


    /* 封装 poll 的接口与 poll 交互 */
    void removeFromPoller(SHARED_PTR_CHANNEL channel) {

    }

    void updatePoller(SHARED_PTR_CHANNEL channel, int timeout = 0) {

    }

    void addToPoller(SHARED_PTR_CHANNEL channel, int timeout = 0) {

    }

private:
    bool looping_;
    shared_ptr<Epoller> poller_;
    bool quit_;
    
    vector<CallBack> functor_lists_;
    bool is_calling_functors_;
    int thread_id_;
    int wakeupFd_; // 用来唤醒线程来执行回调函数
    SHARED_PTR_CHANNEL wakeup_channel_;

    void wakeup();
    void handleRead();
    void appendFunctorsToExecute();
    void handleConn();
};

#endif //_EVENTLOOP_H_