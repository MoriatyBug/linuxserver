#include "Timer.h"
#include <sys/time.h>

size_t calCurTime (int timeout = 0) {
    struct timeval now;
    gettimeofday(&now, NULL);
    size_t expired_time_ = (now.tv_sec % 10000) * 1000 + now.tv_usec / 1000 + timeout;
    return expired_time_;
}

TimerNode::TimerNode(SHARED_PTR_HTTP_PROCESSER httpProcesser, int timeout) :
    deleted_(false), http_processer_(httpProcesser) {
    expired_time_ = calCurTime(timeout);
}

bool TimerNode::isValid() {
    size_t tmp = calCurTime();
    if (tmp < this->expired_time_) {
        return true;
    }
    this->deleteNode();
    return false;
}

TimerNode::~TimerNode() {
    if(http_processer_) {
        http_processer_->handleClose();
    }
}

void TimerNode::update(int timeout) {
    this->expired_time_ = calCurTime(timeout);
}



void TimerManager::addTimer(SHARED_PTR_HTTP_PROCESSER httpProcesser, int timeout) {
    SHARED_PTR_TIMER_NODE timerNode(new TimerNode(httpProcesser, timeout));
    this->timer_priority_queue_.push(timerNode);
}

void TimerManager::handleExpire() {
    while(!this->timer_priority_queue_.empty()) {
        SHARED_PTR_TIMER_NODE timerNode = this->timer_priority_queue_.top();
        if (timerNode->isDeleted()) {
            this->timer_priority_queue_.pop();
        } else if (!timerNode->isValid()) {
            this->timer_priority_queue_.pop();
        } else {
            break;
        }
    }
}

int main() {
    // TimerNode* timerNode = new TimerNode();
}