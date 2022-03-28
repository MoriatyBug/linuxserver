#ifndef _LY_THREAD_H_
#define _LY_THREAD_H_
#include "util/Noncopyable.h"
#include "util/Util.h"
#include <pthread.h>
#include <stdint.h>

class Thread : noncopyable {
public:
    typedef function<void()> CallBack;
    explicit Thread(const CallBack& func);
    ~Thread();
    void start();
    int join();
    bool started() const {
        return is_started_;
    }
    pid_t getThreadId() const {
        return thread_id_;
    }
    static void *startThread(void *obj);

private:
    bool is_started_;
    bool is_joined_;
    pid_t tid_;
    CallBack func_;
    pthread_t thread_id_;
};

namespace CurrentThread
{
    extern __thread int cached_thread_id;
    void cacheThreadId();
    inline int getThreadId() {
        if(__builtin_expect(cached_thread_id == 0, 0)) { // 分支预测，等于 0 的可能性比较小
            cacheThreadId();
        }
        return cached_thread_id;
    }
}

#endif // _LY_THREAD_H_