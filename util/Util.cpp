#include "Util.h"

ssize_t writen(int fd, void *buf, size_t n)
{
    size_t left = n;
    ssize_t written = 0;
    ssize_t writtenCount = 0;
    char *str = (char *)buf;
    while (left > 0) {
        // Write N bytes of BUF to FD.  Return the number written, or -1
        if ((written = write(fd, str, left)) <= 0) {
            if (written < 0) {
                // 被系统调用中断
                if (errno == EINTR) {
                    // 重写
                    written = 0;
                    continue;
                    // 重试, 说明写完了
                } else if (errno == EAGAIN) {
                    return writtenCount;
                } else {
                    return -1;
                }
            }
        }
        writtenCount += written;
        left -= written;
        str += written;
    }
    return writtenCount;
}

ssize_t readn(int fd, void *buf, size_t n)
{
    size_t left = n;
    ssize_t toRead = 0;
    ssize_t readCount = 0;
    char *str = (char *)buf;
    while (left > 0) {
        if ((toRead = read(fd, str, left)) <= 0) {
            if (toRead < 0) {
                if (errno == EINTR) {
                    toRead = 0;
                    continue;
                } else if (errno == EAGAIN) {
                    return readCount;
                } else {
                    return -1;
                }
            }
        }
        readCount += toRead;
        left -= toRead;
        str += toRead;
    }
    return readCount;
}