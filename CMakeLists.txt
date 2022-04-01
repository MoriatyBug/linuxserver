cmake_minimum_required(VERSION 3.14)
project(linuxServer)

set(CXX_FLAGS
        -g
        -Wall
        -std=c++11
        # -D_PTHREADS
        -Wno-unused-parameter
)


set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS_DEBUG "-O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O0")

string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")

set(SRC_LISTS
    Channel.cpp
    Epoller.cpp
    EventLoop.cpp
    HttpProcesser.cpp
    Main.cpp
    Poller.cpp
    Timer.cpp
    Thread.cpp
    EventLoopThread.cpp
    Server.cpp
    EventLoopThreadPool.cpp
)

# include_directories(${PROJECT_SRC_PATH}/util)

add_executable(linuxServer ${SRC_LISTS})

target_link_libraries(linuxServer linux_server_lib pthread)

add_subdirectory(util)