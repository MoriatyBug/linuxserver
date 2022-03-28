#include "HttpProcesser.h"
#include <iostream>
#include <string>

using namespace std;
HttpProcesser::HttpProcesser(EventLoop *eventLoop, int fd)
    : event_loop_(eventLoop),
      channel_(new Channel(eventLoop, fd)),
      fd_(fd),
      curReadIndex(0),
      check_state_(CHECK_STATE_REQUEST_LINE),
      keepAlive_(false),
      header_state_(H_START),
      version_(HTTP_11),
      method_(HTTP_GET),
      connection_state_(C_DISCONNECTING)
{
    channel_->setReadHandler(bind(&HttpProcesser::handleRead, this));
    channel_->setWriteHandler(bind(&HttpProcesser::handleWrite, this));
    channel_->setConnHandler(bind(&HttpProcesser::handleConn, this));
}

HTTP_CODE HttpProcesser::parseRequestline()
{
  // GET /favicon.ico HTTP/1.1

    string &str = inBuffer_;
    string tmp = str;

    size_t pos = str.find('\r', curReadIndex);
    if (pos < 0) {
        return BAD_REQUEST;
    }

    string requestLine = str.substr(0, pos);
    if (str.size() > pos + 1) {
        str = str.substr(pos + 1);
    } else {
        str.clear();
    }

    int idxGet = requestLine.find("GET");
    int idxPost = requestLine.find("POST");
    int idxHead = requestLine.find("HEAD");

    if (idxGet >= 0) {
        pos = idxGet;
        method_ = HTTP_GET;
    } else if (idxPost >= 0) {
        pos = idxPost;
        method_ = HTTP_POST;
    }else if (idxHead >= 0) {
        pos = idxHead;
        method_ = HTTP_HEAD;
    } else {
        return BAD_REQUEST;
    }

    pos = requestLine.find("/", pos);
    if (pos < 0) {
        fileName_ = "index.html";
        version_ = HTTP_11;
        return BAD_REQUEST;
    } else {
        size_t idx = requestLine.find(' ', pos);
        if (idx < 0) {
            return BAD_REQUEST;
        } else {
            if (idx - pos > 1) {
                fileName_ = requestLine.substr(pos + 1, idx - pos - 1);
                // 去掉 GET 请求中携带参数部分
                size_t iidx = fileName_.find('?');
                if (iidx >= 0) {
                    fileName_ = fileName_.substr(0, iidx);
                }
            } else {
                fileName_ = "index.html";
            }
        }
        pos = idx;
    }

    cout << "fileName_: " << fileName_ << endl;

    pos = requestLine.find("/", pos);
    if (pos < 0) {
        return BAD_REQUEST;
    } else {
        if (requestLine.size() - pos <= 3) {
            return BAD_REQUEST;
        } else {
            string ver = requestLine.substr(pos + 1, 3);
            if (ver == "1.0") {
                version_ = HTTP_10;
            } else if (ver == "1.1") {
                version_ = HTTP_11;
            } else {
                return BAD_REQUEST;
            }
        }
    }
    check_state_ = CHECK_STATE_HEADER;
    return GET_REQUEST; // 返回 OK
}


HTTP_CODE HttpProcesser::parseHeaders()
{
// Host: 7.222.76.31:1997
// Connection: keep-alive
    string &str = inBuffer_;
    int keyStart = -1;
    int keyEnd = -1;
    int valueStart = -1;
    int valueEnd = -1;
    int nowReadIndex = 0;
    size_t i = 0;
    bool isFinished = false;
    while (i < str.size() && !isFinished) {
        char c = str[i];
        switch(header_state_) {
            case H_START: {
                if (c == '\r' || c == '\n') {
                    break;
                }
                header_state_ = H_KEY;
                keyStart = i;
                break;
            }
            case H_KEY: {
                if (c == ':') {
                    header_state_ = H_COLON;
                    keyEnd = i;
                    if (keyEnd - keyStart <= 0 || keyEnd - keyStart > 255) {
                        return BAD_REQUEST;
                    }
                } else if (c == '\n' || c == '\r') {
                    return BAD_REQUEST;
                }
                break;
            }
            case H_COLON: {
                if (c == ' ') {
                    header_state_ = H_SPACE;
                } else {
                    return BAD_REQUEST;
                }
                break;
            }
            case H_SPACE: {
                if (c == '\r' || c == '\n') {
                    return BAD_REQUEST;
                }
                header_state_ = H_VALUE;
                valueStart = i;
                break;
            }
            case H_VALUE: {
                if (c == '\r') {
                    header_state_ = H_CR;
                    valueEnd = i;
                    if (valueEnd - valueStart <= 0 || valueEnd - valueStart > 255) {
                        return BAD_REQUEST;
                    }
                } else if (i - valueStart > 255){
                    return BAD_REQUEST;
                }
                break;
            }
            case H_CR: {
                if (c == '\n') {
                    header_state_ = H_LF;
                    string key(str.begin() + keyStart, str.begin() + keyEnd);
                    string value(str.begin() + valueStart, str.begin() + valueEnd);
                    headMap[key] = value;
                    nowReadIndex = i;
                } else {
                    return BAD_REQUEST;
                }
                break;
            }
            case H_LF: {
                if (c == '\r') {
                    header_state_ = H_END_CR;
                } else if (c == '\n') {
                    return BAD_REQUEST;
                } else {
                    header_state_ = H_KEY;
                    keyStart = i;
                }
                break;
            }
            case H_END_CR: {
                if (c == '\n') {
                    header_state_ = H_END_LF;
                } else {
                    return BAD_REQUEST;
                }
                break;
            }
            case H_END_LF: {
                isFinished = true;
                nowReadIndex = i;
                keyStart = i;
                break;
            }
        }
        i++;
    }

    if (header_state_ == H_END_LF) {
        str = str.substr(nowReadIndex);
        if (method_ == HTTP_GET) {
            check_state_ = CHECK_STATE_ANALYSIS;
        } else if (method_ == HTTP_POST) {
            check_state_ = CHECK_STATE_BODY;
        }
        return GET_REQUEST;
    }
    str = str.substr(nowReadIndex);
    return NO_REQUEST;
}

HTTP_CODE HttpProcesser::analysisRequest()
{
    if (method_ == HTTP_GET || method_ == HTTP_HEAD) {
        string header;

    }

    return INTERNAL_ERROR;
}

void HttpProcesser::handleRead()
{
    while (true) {
        // int read_num = readn(fd_, inBuffer_, zero); 
        if (connection_state_ == C_DISCONNECTING) {
            inBuffer_.clear();
            break;
        }
        cout << inBuffer_ << endl;

        if (check_state_ == CHECK_STATE_REQUEST_LINE) {
            HTTP_CODE ret = this->parseRequestline();
            if (ret == BAD_REQUEST) {
                inBuffer_.clear();
                cout << "parserequest error" << endl;
                break;
            } else if (ret == NO_REQUEST) {
                break;
            }
        }

        if (check_state_ == CHECK_STATE_HEADER) {
            HTTP_CODE ret = this->parseHeaders();
            if (ret == BAD_REQUEST) {
                inBuffer_.clear();
                cout << "parse headers error" << endl;
                break;
            } else if (ret == NO_REQUEST) {
                break;
            }
        }

        if (check_state_ == CHECK_STATE_BODY) {
            int contentLength = -1;
            if (headMap.count("Content-length") > 0) {
                contentLength = stoi(headMap["Content-length"]);
            } else {
                cout << "no body" << endl;
                break;
            }
            if (static_cast<int>(inBuffer_.size()) < contentLength) break;
            check_state_ = CHECK_STATE_ANALYSIS;
        }

        if (check_state_ == CHECK_STATE_ANALYSIS) {
            HTTP_CODE ret = this->analysisRequest();
            //
            if (ret == ANALYSIS_SUCCESS) {
                check_state_ = CHECK_STATE_FINISH;
                break;
            } else {
                cout << "analysis failed" << endl;
                break;
            }
        }
    }
}

void HttpProcesser::handleConn()
{
    
}

void HttpProcesser::handleClose()
{
    
}

// int main()
// {
//     HttpProcesser* httpProcesser = new HttpProcesser();
//     // HTTP_CODE ret = httpProcesser->parseRequestline();
//     httpProcesser->inBuffer_ = "Host: 7.222.76.31:1997\r\nConnection: keep-alive\r\n\r\n";
//     HTTP_CODE ret = httpProcesser->parseHeaders();
//     for (auto key : httpProcesser->headMap) {
//         cout << key.first << "\t" << key.second << endl;
//     }
//     cout << ret << endl;
//     return 0;
// }