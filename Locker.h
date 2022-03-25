#include "util/Noncopyable.h"
#include <pthread.h>

class MutexLock : noncopyable {
public:
    MutexLock() {
        pthread_mutex_init(&mutex_, NULL);
    }
    ~MutexLock() {
        pthread_mutex_lock(&mutex_);
        pthread_mutex_destroy(&mutex_);
    }
    void lock() {
        pthread_mutex_lock(&mutex_);
    }
    void unlock() {
        pthread_mutex_unlock(&mutex_);
    }
    pthread_mutex_t* getMutex() {
        return &mutex_;
    }

private:
    pthread_mutex_t mutex_;

    friend class Condition;
};

class MutexLockGuard : noncopyable {
public:
    explicit MutexLockGuard(MutexLock &mutexLock) : mutex_lock_(mutexLock) {
            mutex_lock_.lock();
    }

    ~MutexLockGuard() {
        mutex_lock_.unlock();
    }
    
private:
    MutexLock &mutex_lock_;
};