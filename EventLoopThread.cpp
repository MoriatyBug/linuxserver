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
        // 等待线程真正启动时创建 eventLoop
        while (event_loop_ == NULL) {
            condition_.wait();
        }
        return event_loop_;
    }
}

/* 线程实际运行的函数 */
void EventLoopThread::threadFunc() {
    EventLoop eventLoop;
    {
        // 当 eventLoop 创建成功的时候，通知外层保存 eventLoop
        MutexLockGuard mutexLockGuard(mutex_lock_);
        event_loop_ = &eventLoop;
        condition_.notify();
    }
    eventLoop.loop();
    event_loop_ = NULL; // 这一步比较关键，但是可以没有吗？
}