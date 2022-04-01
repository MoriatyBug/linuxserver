#include "Epoller.h"
#include <sys/time.h>
Epoller::Epoller()
    : epoll_fd_(epoll_create1(EPOLL_CLOEXEC)),
      events_(4096),
      timer_manager_(new TimerManager())
{
   assert(epoll_fd_ > 0); 
}

Epoller::~Epoller() {}


void Epoller::epollAdd(SHARED_PTR_CHANNEL channel, int timeout) {
    int fd = channel->getFd();
    if (timeout > 0) {
        addTimer(channel, timeout);
        this->epollFdToHttpProcesser[fd] = channel->getHolder();
    }
    epoll_event event;
    event.data.fd = fd;
    event.events = channel->getEvents();

    this->epollFdToChannel[fd] = channel;
    channel->compareAndsetLastEvents();
    if (epoll_ctl(this->epoll_fd_, EPOLL_CTL_ADD, fd, &event) < 0) {
        this->epollFdToChannel[fd].reset(); // 释放对象
    }
}

void Epoller::epollDelete(SHARED_PTR_CHANNEL channel) {
    int fd = channel->getFd();
    epoll_event event;
    event.data.fd = fd;
    event.events = channel->getLastEvents();

    if (epoll_ctl(this->epoll_fd_, EPOLL_CTL_DEL, fd, &event) < 0) {
        cout << "del epoll error" << endl;
    }

    this->epollFdToChannel[fd].reset();
    this->epollFdToHttpProcesser[fd].reset();
}


void Epoller::epollUpdate(SHARED_PTR_CHANNEL channel, int timeout) {
    int fd = channel->getFd();
    addTimer(channel, timeout);
    if (!channel->compareAndsetLastEvents()) {
        epoll_event event;
        event.data.fd = fd;
        event.events = channel->getEvents();
        if (epoll_ctl(this->epoll_fd_, EPOLL_CTL_MOD, fd, &event) < 0) {
            this->epollFdToChannel[fd].reset();
        }
    }
}

// TODO std move 优化
vector<SHARED_PTR_CHANNEL> Epoller::poll() {
    struct timeval start;
    gettimeofday(&start, NULL);
    while (true) {
        int eventCount = epoll_wait(this->epoll_fd_, &*events_.begin(), 
                                    this->events_.size(), 10000);
        if (eventCount < 0) {
            // cout << "epoll_wait error" << endl;
        }
        vector<SHARED_PTR_CHANNEL> activeChannelLists = getActiveEvents(eventCount);
        if (activeChannelLists.size() > 0) {
            struct timeval now;
            gettimeofday(&now, NULL);
            return activeChannelLists;
        }
    }
}

// TODO std move 优化
vector<SHARED_PTR_CHANNEL> Epoller::getActiveEvents(int eventCount) {
    vector<SHARED_PTR_CHANNEL> activeChannelLists;
    for (int i = 0; i < eventCount; i++) {
        int fd = this->events_[i].data.fd;
        SHARED_PTR_CHANNEL curChannel = this->epollFdToChannel[fd];
        if (curChannel) {
            curChannel->setRevents(this->events_[i].events);
            curChannel->setEvents(0);
            activeChannelLists.push_back(curChannel);
        } else {
            cout << "curChannel error" << endl;
        }
    }

    return activeChannelLists;
}

void Epoller::handleExpire()
{
    this->timer_manager_->handleExpire();
}

void Epoller::addTimer(SHARED_PTR_CHANNEL channel, int timeout)
{
    shared_ptr<HttpProcesser> httpProcesser = channel->getHolder();
    this->timer_manager_->addTimer(httpProcesser, timeout);
}