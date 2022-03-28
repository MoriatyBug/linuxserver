#ifndef _TIMER_H_
#define _TIMER_H_


#include "util/Util.h"
#include <queue>
#include "HttpProcesser.h"

typedef shared_ptr<HttpProcesser> SHARED_PTR_HTTP_PROCESSER;
class TimerNode {
public:
    TimerNode(SHARED_PTR_HTTP_PROCESSER httpProcesser, int timeout);
    ~TimerNode();
    TimerNode(TimerNode &timerNode);
    size_t getExpireTime() const {
        return expired_time_;
    }
    void deleteNode() {
        deleted_ = true;
    }
    bool isDeleted() {
        return deleted_;
    }
    bool isValid();
    void update(int timeout);

private:
    bool deleted_;
    size_t expired_time_;
    SHARED_PTR_HTTP_PROCESSER http_processer_;
};

typedef shared_ptr<TimerNode> SHARED_PTR_TIMER_NODE;

// 最小堆，将 expire time 最小的放到最上面，开始清除最近过期的时间。
struct TimerCmp {
    bool operator()(SHARED_PTR_TIMER_NODE &a, SHARED_PTR_TIMER_NODE &b) const {
        return a->getExpireTime() > b->getExpireTime();
    }
};

class TimerManager {
public:
    void addTimer(SHARED_PTR_HTTP_PROCESSER httpProcesser, int timeout);
    void handleExpire();

private:
    priority_queue<SHARED_PTR_TIMER_NODE, deque<SHARED_PTR_TIMER_NODE>, TimerCmp> timer_priority_queue_;
};

#endif