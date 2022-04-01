#include "Server.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#define MAX_FD_NUM 100000

typedef shared_ptr<HttpProcesser> SHARED_PTR_HTTP_PROCESSER;

Server::Server(EventLoop *eventLoop, int threadNum, int port)
    : event_loop_(eventLoop),
      thread_num_(threadNum),
      thread_pool_(new EventLoopThreadPool(event_loop_, threadNum)),
      is_started_(false),
      port_(port),
      listen_fd_(bindSocket(port)),
      channel_(new Channel(event_loop_, listen_fd_))
{
    // 将 pipe 的 signal 处理函数设置为 SIG_IGN 忽略异常，防止服务器退出
    handleForSigpipe();
    setSocketNonBlocking(listen_fd_);
}

void Server::start()
{
    is_started_ = true;
    thread_pool_->start(); // 启动线程池
    channel_->setEvents(DEFAULT_EVENT);
    channel_->setReadHandler(bind(&Server::handleNewConn, this));
    channel_->setConnHandler(bind(&Server::updateEpoll, this));
    event_loop_->addToPoller(channel_);
}

void Server::handleNewConn()
{
    struct sockaddr_in clientAddr;
    socklen_t len = sizeof(clientAddr);
    memset(&clientAddr, 0, len);
    int acceptFd = 0;
    while ((acceptFd = accept(listen_fd_, (struct sockaddr *)&clientAddr, &len)) > 0) {
        EventLoop *eventLoop = thread_pool_->getNextEventLoop();
        cout <<"thread id: " << eventLoop->getThreadId() << endl;
        if (acceptFd >= MAX_FD_NUM) {
            close(acceptFd);
            continue;
        }

        // 设置非阻塞模式
        setSocketNonBlocking(acceptFd);
        // 关闭 nagle 算法
        setSocketNodelay(acceptFd);
        SHARED_PTR_HTTP_PROCESSER httpProcesser(new HttpProcesser(eventLoop, acceptFd));
        httpProcesser->getChannel()->setHolder(httpProcesser);
        event_loop_->queueInLoop(bind(&HttpProcesser::newEvent, httpProcesser));
    }
}

void Server::updateEpoll()
{
    event_loop_->updatePoller(channel_);
}