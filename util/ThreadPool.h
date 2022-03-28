#ifndef _THREADPOLL_H
#define _THREADPOLL_H

#include "Noncopyable.h"
#include "util.h"
#include <list>
#include "../EventLoop.h"
#include "../Locker.h"
#include "../Condition.h"
#include "../Thread.h"
#define MAX_THREAD_NUM 8

class ThreadPool : noncopyable
{
public:
    typedef function<void()> CallBack;
    ThreadPool();
    ~ThreadPool();
    bool append(CallBack func);
    void setThreadInitCallBack(const CallBack& func) {
        thread_init_callback_ = func;
    }
    void run(CallBack func);
    void stop();
    void start(int threadNums);
    bool isFull() const;
    CallBack take();

private:
    void runInThread();
    CallBack thread_init_callback_;
    /* 线程池固有参数 */
    vector<unique_ptr<Thread>>      thread_pool_;                   // 线程池
    list<CallBack>      work_queue_;                    // 请求队列
    mutable MutexLock   mutex_lock_;                    // 互斥锁
    int                 max_queue_size_;                // 请求队列最大长度
    bool                is_running_;     
    Condition           not_full_;   
    Condition           not_empty_; 

};

#endif // _THREADPOLL_H