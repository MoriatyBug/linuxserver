#ifndef _UTIL_H_
#define _UTIL_H_
#include <functional>
#include <vector>
#include <memory>
#include <iostream>
#include <unistd.h>

using namespace std;

typedef __uint32_t UINT32;
typedef function<void()> CallBack;

void shutdownWR(int fd);

extern ssize_t writen(int fd, void *buf, size_t n);
extern ssize_t readn(int fd, void *buf, size_t n);



#endif