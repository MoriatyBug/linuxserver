#include "Thread.h"
#include <linux/unistd.h>
#include <unistd.h>
#include <assert.h>
#include <sys/syscall.h>  

namespace CurrentThread
{
    __thread int cached_thread_id = 0;
}

pid_t gettid()
{
    return static_cast<pid_t>(::syscall(SYS_gettid));
}

Thread::Thread(const CallBack& func)
    : is_started_(false),
      is_joined_(false),
      tid_(0),
      func_(func),
      thread_id_(0)
{
}

void CurrentThread::cacheThreadId()
{
    if (cached_thread_id == 0) {
        cached_thread_id = gettid();
    }
}

Thread::~Thread()
{
    if (is_started_) {
        pthread_detach(thread_id_);
    }
}

void* Thread::startThread(void *obj)
{
    Thread* thread = static_cast<Thread *>(obj);
    thread->func_();
    return obj;
}

void Thread::start()
{
    assert(!is_started_);
    is_started_ = true;
    // startThread 函数需要声明为 static，因为需要直接调用，否则会报错
    if (pthread_create(&thread_id_, NULL, &startThread, this)) { 
        is_started_ = false;
    } else {
        cout << "thread started" << endl;
        assert(thread_id_ > 0);
    }
}

int Thread::join()
{
    assert(is_started_);
    assert(!is_joined_);
    is_joined_ = true;
    return pthread_join(thread_id_, NULL);
}

