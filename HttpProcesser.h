#ifndef _HTTP_PROCESSER_H_
#define _HTTP_PROCESSER_H_
#include <string>
#include <map>
using namespace std;

enum CHECK_STATE {
    CHECK_STATE_REQUEST_LINE = 0,
    CHECK_STATE_HEADER,
    CHECK_STATE_BODY,
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

class HttpProcesser {
public:
    HttpProcesser();
    ~HttpProcesser();
    void handleClose();
    void newEvent();

private:
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