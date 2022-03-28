#include "ThreadPool.h"

ThreadPool::ThreadPool()
    : mutex_lock_(),
      max_queue_size_(0),
      is_running_(false),
      not_full_(mutex_lock_),
      not_empty_(mutex_lock_)
{
}

ThreadPool::~ThreadPool()
{
    if (is_running_) {
        stop();
    }
}

void ThreadPool::start(int threadNums)
{
    assert(thread_pool_.empty());
    is_running_ = true;
    thread_pool_.reserve(threadNums); // 分配空间
    for (int i = 0; i < threadNums; i++) {
        cout << "thread: num_" << i << " start" << endl;
        thread_pool_.emplace_back(new Thread(bind(ThreadPool::runInThread, this)));
        thread_pool_[i]->start();  
    }
    // 如果没有线程，那么执行初始化函数
    if (threadNums == 0 && thread_init_callback_) {
        thread_init_callback_();
    }
}

void ThreadPool::stop()
{
    {
        MutexLockGuard MutexLockGuard(this->mutex_lock_);
        is_running_ = false;
        // 释放条件锁
        not_empty_.notifyAll();
        not_full_.notifyAll();
    }
    for (auto &thread : thread_pool_) {
        thread->join();
    }
}

bool ThreadPool::append(CallBack func)
{
    if (thread_pool_.empty()) {
        func();
    } else {
        MutexLockGuard lock(this->mutex_lock_);
        // 如果线程池满了，那么开始等待，直到 not_full_ 收到 notify()
        // or 等到 not full
        while(isFull() && is_running_) {
            not_full_.wait();
        }

        // 线程池都没有跑起来，肯定要 return 了
        if (!is_running_) {
            return false;
        }

        assert(!isFull()); // 如果线程池没满才能加入
        work_queue_.push_back(move(func));
        not_empty_.notify();
    }
    return true;
}


bool ThreadPool::isFull() const
{
    return max_queue_size_ > 0 && 
           work_queue_.size() >= max_queue_size_;
}

void ThreadPool::runInThread()
{
    if (thread_init_callback_) {
        thread_init_callback_();
    }
    while(is_running_) {
        CallBack func(take());
        if (func) {
            func();
        }
    }
}

CallBack ThreadPool::take()
{
    MutexLockGuard MutexLockGuard(mutex_lock_);
    // 自旋等待工作队列非空
    while(work_queue_.empty() && is_running_) {
        not_empty_.wait();
    }
    CallBack func;
    if (!work_queue_.empty()) {
        func = work_queue_.front();
        work_queue_.pop_front();
        if (max_queue_size_ > 0) {
            not_full_.notify();
        }
    }
    return func;
}


