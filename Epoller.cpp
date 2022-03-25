#include "Epoller.h"


void Epoller::epollAdd(SHARED_PTR_CHANNEL channel, int timeout) {
    int fd = channel->getFd();
    if (timeout > 0) {
        // TODO 添加计时器

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
    // add_timer();
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
    while (true) {
        int eventCount = epoll_wait(this->epoll_fd_, &this->events_[0], 
                                    this->events_.size(), EPOLLWAIT_TIME);
        if (eventCount < 0) {
            cout << "epoll_wait error" << endl;
        }
        vector<SHARED_PTR_CHANNEL> activeChannelLists = getActiveEvents(eventCount);
        if (activeChannelLists.size() > 0) {
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