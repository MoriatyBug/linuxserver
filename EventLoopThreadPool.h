#ifndef _EVENTLOOP_THREADPOOL_H_
#define _EVENTLOOP_THREADPOOL_H_

#include "EventLoop.h"
#include "util/Util.h"
#include "EventLoopThread.h"

class EventLoopThreadPool
{
private:
    /* data */
    EventLoop*                          base_loop_;
    bool                                is_started_;
    int                                 thread_num_;
    int                                 next_;
    vector<shared_ptr<EventLoopThread>> thread_list_;
    vector<EventLoop*>                  loop_list_;
public:
    EventLoopThreadPool(EventLoop *baseLoop, int threadNum);
    ~EventLoopThreadPool();
    void start();
    EventLoop* getNextEventLoop();
};




#endif // _EVENTLOOP_THREADPOOL_H_