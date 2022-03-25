#include "EventLoopThread.h"


EventLoopThread::EventLoopThread() 
    : event_loop_(NULL), exited_(false), mutexlock_(), condition_(mutexlock_) {}

EventLoopThread::~EventLoopThread() {
    exited_ = true;
    if (event_loop_) {
        event_loop_->quit();
        //
    }
}

EventLoop* EventLoopThread::startLoop() {
    assert(!)
}

void EventLoopThread::threadFunc() {

}