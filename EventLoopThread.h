#include "util/Noncopyable.h"
#include "EventLoop.h"
#include "Locker.h"
#include "Condition.h"

class EventLoopThread : noncopyable {
public:
    EventLoopThread();
    ~EventLoopThread();
    EventLoop* startLoop();

private:
    void threadFunc();
    EventLoop* event_loop_;
    bool exited_;
    // Thread thread_;
    MutexLock mutexlock_;
    Condition condition_;
};