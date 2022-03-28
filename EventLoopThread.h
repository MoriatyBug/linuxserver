#ifndef _EVENTLOOP_THREAD_H_
#define _EVENTLOOP_THREAD_H_

#include "util/Noncopyable.h"
#include "EventLoop.h"
#include "Locker.h"
#include "Condition.h"
#include "Thread.h"

class EventLoopThread : noncopyable {
public:
    EventLoopThread();
    ~EventLoopThread();
    EventLoop* startLoop();
    void threadFunc();


private:
    EventLoop* event_loop_;
    bool exited_;
    Thread thread_;
    MutexLock mutex_lock_;
    Condition condition_;
};

#endif // _EVENTLOOP_THREAD_H_