#include "Channel.h"

Channel::Channel(EventLoop* loop)
    : fd_(0), loop_(loop), events_(0), last_events_(0) {}

Channel::Channel(EventLoop* loop, int fd)
    : fd_(fd), loop_(loop), events_(0), last_events_(0) {}

