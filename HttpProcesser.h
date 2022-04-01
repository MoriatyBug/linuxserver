#ifndef _HTTP_PROCESSER_H_
#define _HTTP_PROCESSER_H_
#include <string>
#include <map>
#include "EventLoop.h"
#include "Channel.h"
#include "util/Util.h"
#include "Timer.h"
using namespace std;

class EventLoop;
class Channel;
class TimerNode;
typedef shared_ptr<Channel> SHARED_PTR_CHANNEL;
enum CHECK_STATE {
    CHECK_STATE_REQUEST_LINE = 0,
    CHECK_STATE_HEADER,
    CHECK_STATE_BODY,
    CHECK_STATE_ANALYSIS,
    CHECK_STATE_FINISH,
};

enum LINE_STATUS {
    LINE_OK = 0,
    LINE_BAD,
    LINE_OPEN,
};

enum HTTP_CODE {
    NO_REQUEST = 0,
    GET_REQUEST,
    BAD_REQUEST,
    FORBIDDEN_REQUEST,
    INTERNAL_ERROR,
    CLOSED_CONNECTION,
    ANALYSIS_SUCCESS,
};

enum HTTP_METHOD {
    HTTP_POST = 0,
    HTTP_GET,
    HTTP_HEAD
};

enum HTTP_VERSION {
    HTTP_10 = 0,
    HTTP_11,
};

enum HEADER_STATE {
    H_START,
    H_KEY,
    H_COLON,
    H_SPACE,
    H_VALUE,
    H_CR,
    H_LF,
    H_END_CR,
    H_END_LF,
};

enum CONNECTION_STATE {
    C_DISCONNECTING,
    C_CONNECTED,
    C_DISCONNECTED,
};

class HttpProcesser : public enable_shared_from_this<HttpProcesser> {
public:
    HttpProcesser(EventLoop *eventLoop, int fd);
    ~HttpProcesser();
    void handleClose();
    void newEvent();
    SHARED_PTR_CHANNEL getChannel() {
        return channel_;
    }
    void setTimer(shared_ptr<TimerNode> timer) {
        if (!timer_.lock())
            timer_ = timer;
    }
    void seperateTimer();

public:
    EventLoop *event_loop_;
    SHARED_PTR_CHANNEL channel_;
    int fd_;
    string inBuffer_;
    string outBuffer_;
    HTTP_CODE http_code_;
    LINE_STATUS line_status_;

    string fileName_;
    string path_;
    int curReadIndex;
    CHECK_STATE check_state_;
    bool keepAlive_;
    map<string, string> headMap;
    HEADER_STATE header_state_;
    HTTP_VERSION version_;
    HTTP_METHOD method_;
    CONNECTION_STATE connection_state_;
    bool error_;
    weak_ptr<TimerNode> timer_;

    void handleRead();
    void handleWrite();
    void handleConn();
    void handleError();
    LINE_STATUS parseLine();
    HTTP_CODE parseRequestline();
    HTTP_CODE parseHeaders();
    HTTP_CODE analysisRequest();

};

#endif //_HTTP_PROCESSER_H_