#include "util/Noncopyable.h"
#include "Locker.h"
#include <time.h>
#include <errno.h>

class Condition : noncopyable {
public:
    explicit Condition(MutexLock &mutexLock) : mutex_lock_(mutexLock) {
        pthread_cond_init(&cond_, NULL);
    }
    ~Condition() {
        pthread_cond_destroy(&cond_);
    }

    void wait() {
        pthread_cond_wait(&cond_, mutex_lock_.getMutex());
    }

    void notify() {
        pthread_cond_signal(&cond_);
    }

    void notifyAll() {
        pthread_cond_broadcast(&cond_);
    }

    bool waitForSeconds(int seconds) {
        struct timespec abstime;
        clock_gettime(CLOCK_REALTIME, &abstime);
        abstime.tv_sec += static_cast<time_t>(seconds);
        return ETIMEDOUT == 
               pthread_cond_timedwait(&cond_, mutex_lock_.getMutex(), &abstime);
    }

private:
    MutexLock &mutex_lock_;
    pthread_cond_t cond_;
};