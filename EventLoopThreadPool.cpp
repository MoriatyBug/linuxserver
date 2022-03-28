#include "EventLoopThreadPool.h"

void EventLoopThreadPool::start()
{
    base_loop_->assertInLoopThread();
    is_started_ = true;
    for (int i = 0; i < thread_num_; i++) {
        shared_ptr<EventLoopThread> sharedPtrThread(new EventLoopThread());
        thread_list_.push_back(sharedPtrThread); // 保存线程
        EventLoop* loop = sharedPtrThread->startLoop(); // 启动 loop 对应的线程
        loop_list_.push_back(loop); // 保存 loop
    }
}

EventLoop *EventLoopThreadPool::getNextEventLoop()
{
    base_loop_->assertInLoopThread();
    assert(is_started_);
    EventLoop *eventLoop = base_loop_; // 也就是 mainLoop
    if (!loop_list_.empty()) {
        eventLoop = loop_list_[next_];
        next_ = (next_ + 1) % thread_num_;
    }
    return eventLoop;
}