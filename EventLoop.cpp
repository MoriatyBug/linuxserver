#include "EventLoop.h"
#include <sys/epoll.h>
#include <sys/eventfd.h>

int createEventFd() {
    int eventFd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (eventFd < 0) {
        abort();
    }
    return eventFd;
}

EventLoop::EventLoop() {
    this->wakeup_fd_ = createEventFd();
    wakeup_channel_ = SHARED_PTR_CHANNEL(new Channel(this, this->wakeup_fd_));
    wakeup_channel_->setEvents(EPOLLIN | EPOLLET);
    wakeup_channel_->setReadHandler(bind(&EventLoop::handleWakeup, this));
    wakeup_channel_->setConnHandler(bind(&EventLoop::handleConn, this));
    this->addToPoller(wakeup_channel_);
}

void EventLoop::handleConn() {
    this->updatePoller(this->wakeup_channel_);
}

void EventLoop::wakeup() {
    uint64_t buf = 1;
    ssize_t n = writen(wakeup_fd_, (void *)(&buf), sizeof(buf));
    if (n != sizeof(buf)) {
        cout << "write wakeupfd error" << endl;
    }
}

void EventLoop::handleWakeup() {
    uint64_t buf = 1;
    ssize_t n = readn(wakeup_fd_, (void *)(&buf), sizeof(buf));
    if (n != sizeof(buf)) {
        cout << "read wakeupfd error" << endl;
    }
    wakeup_channel_->setEvents(EPOLLIN | EPOLLET);
}

void EventLoop::loop() {
    assert(!looping_);
    assert(isInLoopThread());
    looping_ = true;
    quit_ = false;
    vector<SHARED_PTR_CHANNEL> ret;
    while (!quit_) {
        ret.clear();
        ret = this->poller_->poll();
        this->is_handling_event_ = true;
        for (auto &channel : ret) {
            channel->handleEvents();
        }
        this->is_handling_event_ = false;
        appendFunctorsToExecute();
        
    }
    looping_ = false;
}

void EventLoop::queueInLoop(CallBack &&func) {
    {
        MutexLockGuard mutexLockGuard(this->mutex_lock_);
        this->functor_lists_.emplace_back(std::move(func));
    }
    if (!isInLoopThread() || this->is_calling_functors_) {
        this->wakeup();
    }
}

void EventLoop::runInLoop(CallBack &&func) {
    if (this->isInLoopThread()) {
        func();
    } else {
        this->queueInLoop(move(func));
    }
}



void EventLoop::appendFunctorsToExecute() {
    vector<CallBack> functors;
    this->is_calling_functors_ = true;
    {
        MutexLockGuard mutexLockGuard(this->mutex_lock_);
        functors.swap(functor_lists_); // 先复制再遍历，防止并发操作导致 iterator 失效
    }

    for (size_t i = 0; i < functors.size(); i++) {
        functors[i]();
    }
    this->is_calling_functors_ = false;
}

void EventLoop::quit() {
    quit_ = true;
    if (!isInLoopThread()) {
        wakeup();
    }
}