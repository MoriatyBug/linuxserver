#include "EventLoopThread.h"

EventLoopThread::EventLoopThread() 
    : event_loop_(NULL),
      exited_(false),
      thread_(bind(&EventLoopThread::threadFunc, this)),
      mutex_lock_(),
      condition_(mutex_lock_) 
{
} 

EventLoopThread::~EventLoopThread() {
    exited_ = true;
    if (event_loop_) {
        event_loop_->quit();
        thread_.join();
    }
}

EventLoop* EventLoopThread::startLoop() {
    assert(!thread_.started());
    thread_.start();
    {
        MutexLockGuard mutexLockGuard(mutex_lock_);
        while (event_loop_ == NULL) {
            condition_.wait();
        }
        return event_loop_;
    }
}

void EventLoopThread::threadFunc() {
    EventLoop eventLoop;
    {
        MutexLockGuard mutexLockGuard(mutex_lock_);
        event_loop_ = &eventLoop;
        condition_.notify();
    }
    eventLoop.loop();
    event_loop_ = NULL; // 这一步比较关键，但是可以没有吗？
}