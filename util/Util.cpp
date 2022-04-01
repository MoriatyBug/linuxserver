#include "Util.h"
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <string.h>
#include <signal.h>



UINT32 DEFAULT_EVENT = (EPOLLIN | EPOLLET);
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

ssize_t readn(int fd, string &buf, bool &zero)
{
    ssize_t toRead = 0;
    ssize_t readCount = 0;
    while (true) {
        char tmp[4096];
        if ((toRead = read(fd, tmp, 4096)) < 0) {
            if (errno == EINTR) {
                continue;
            } else if (errno == EAGAIN) {
                return readCount;
            } else {
                cout << "read error" << endl;
                return -1;
            }
        } else if(toRead == 0) {
            zero = true;
            break;
        }
        readCount += toRead;
        buf += string (tmp, tmp + toRead);
    }
    return readCount;
}

ssize_t writen(int fd, string &buf)
{
    size_t left = buf.size();
    ssize_t written = 0;
    ssize_t writtenCount = 0;
    const char *str = buf.c_str();
    while (left) {
        if ((written = write(fd, str, left)) <= 0) {
            if (written < 0) {
                if (errno == EINTR) {
                    written = 0;
                    continue;
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

    if (writtenCount == static_cast<int>(buf.size())) {
        buf.clear();
    } else {
        buf = buf.substr(writtenCount);
    }
    return writtenCount;
}

int setSocketNonBlocking(int fd)
{
    int oldOption = fcntl(fd, F_GETFL);
    int newOption = oldOption | O_NONBLOCK;
    fcntl(fd, F_SETFL, newOption);
    return oldOption;
}

void setSocketNodelay(int fd) {
    int enable = 1;
    setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (void *)&enable, sizeof(enable));
}

void handleForSigpipe() {
    struct sigaction sa;
    memset(&sa, '\0', sizeof(sa));
    sa.sa_handler = SIG_IGN;
    sa.sa_flags = 0;
    if (sigaction(SIGPIPE, &sa, NULL)) return;
}

int bindSocket(int port)
{
    if (port < 0 || port > 65535) {
        return -1;
    }
    int listenFd = 0;
    if ((listenFd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        return -1;
    }

    int optval = 1;
    if (setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
        close(listenFd);
        return -1;
    }
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons((unsigned short) port);
    if (bind(listenFd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        close(listenFd);
        return -1;
    }

    if (listen(listenFd, 2048) == -1) {
        close(listenFd);
        return -1;
    }

    if (listenFd == -1) {
        close(listenFd);
        return -1;
    }
    return listenFd;
}