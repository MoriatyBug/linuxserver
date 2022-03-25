#ifndef _UTIL_H_
#define _UTIL_H_
#include <functional>
#include <vector>
#include <memory>
#include <iostream>

using namespace std;

typedef __uint32_t UINT32;
typedef function<void()> CallBack;

void shutdownWR(int fd);

#endif