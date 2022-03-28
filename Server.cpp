#include "Server.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#define MAX_FD_NUM 100000


// void handle_for_sigpipe() {
//     struct sigaction sa;
//     memset(&sa, '\0', sizeof(sa));
//     sa.sa_handler = SIG_IGN;
//     sa.sa_flags = 0;
//     if (sigaction(SIGPIPE, &sa, NULL)) return;
// }
typedef shared_ptr<HttpProcesser> SHARED_PTR_HTTP_PROCESSER;

Server::Server(EventLoop *eventLoop, int threadNum, int port)
    : event_loop_(eventLoop),
      thread_num_(threadNum),
      thread_pool_(new EventLoopThreadPool(event_loop_, threadNum)),
      is_started_(false),
      channel_(new Channel(event_loop_)),
      port_(port),
      listen_fd_(0)
{
    channel_->setFd(listen_fd_);
    // 将 pipe 的 signal 处理函数设置为 SIG_IGN 忽略异常，防止服务器退出
    // handle_for_sigpipe();
    // setSocketNonBlocking(listenFd_);
}

void Server::start()
{
    thread_pool_->start(); // 启动线程池
    channel_->setEvents(EPOLLIN | EPOLLET);
    channel_->setReadHandler(bind(&Server::handleNewConn, this));
    channel_->setConnHandler(bind(&Server::updateEpoll, this));
}

void Server::handleNewConn()
{
    struct sockaddr_in clientAddr;
    socklen_t len = sizeof(clientAddr);
    memset(&clientAddr, 0, len);
    int acceptFd = 0;
    while ((acceptFd = accept(listen_fd_, (struct sockaddr *)&clientAddr, &len)) > 0) {
        EventLoop *eventLoop = thread_pool_->getNextEventLoop();

        if (acceptFd >= MAX_FD_NUM) {
            close(acceptFd);
            continue;
        }

        // 设置非阻塞模式
        // if (setSocketNonBlocking() < 0) {

        // }

        SHARED_PTR_HTTP_PROCESSER httpProcesser(new HttpProcesser(eventLoop, acceptFd));

    }
    
}