#include "Noncopyable.h"
#include "util.h"
#include <pthread.h>

class Thread : noncopyable {
public:
    typedef function<void()> CallBack;
    explicit Thread(const CallBack&, const string& name = "");
    ~Thread();
    void start();
    int join();
    bool started() const {
        return is_started_;
    }
    pid_t getThreadId() const {
        return pid;
    }

private:
    bool is_started_;
    pid_t pid;
    CallBack& func_;
    pthread_t thread_id_;
    string thread_name_;

};