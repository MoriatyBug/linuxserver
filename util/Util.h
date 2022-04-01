#ifndef _UTIL_H_
#define _UTIL_H_
#include <functional>
#include <vector>
#include <memory>
#include <iostream>
#include <unistd.h>
#include <sys/epoll.h>

using namespace std;

typedef __uint32_t UINT32;
extern UINT32  DEFAULT_EVENT;
typedef function<void()> CallBack;

void shutdownWR(int fd);

extern ssize_t writen(int fd, void *buf, size_t n);
extern ssize_t readn(int fd, void *buf, size_t n);
extern ssize_t writen(int fd, string &buf);
extern int setSocketNonBlocking(int fd);
extern void setSocketNodelay(int fd);
extern void handleForSigpipe();
extern int bindSocket(int port);
extern ssize_t readn(int fd, string &buf, bool &zero);
#endif