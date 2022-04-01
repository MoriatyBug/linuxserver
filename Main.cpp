#include "EventLoop.h"
#include "Server.h"

int main()
{
    int threadNum = 8;
    int port = 1997;
    EventLoop mainLoop;
    Server server(&mainLoop, threadNum, port);
    server.start();
    mainLoop.loop();
    return 0;
}