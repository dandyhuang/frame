// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#ifndef COMMON_NET_HTTP_SERVER_HANDLER_H
#define COMMON_NET_HTTP_SERVER_HANDLER_H
#pragma once

#include "thirdparty/monitor_sdk_common/net/http/request.h"
#include "thirdparty/monitor_sdk_common/net/http/response.h"

namespace common {

// Abstract base class of all concrete HttpHandler classes
class HttpHandler {
protected:
    HttpHandler() {}
public:
    virtual ~HttpHandler() {}
    virtual void HandleRequest(const HttpRequest* req, HttpResponse* resp);
    virtual void HandleGet(const HttpRequest* req, HttpResponse* resp);
    virtual void HandlePost(const HttpRequest* req, HttpResponse* resp);
    virtual void HandlePut(const HttpRequest* req, HttpResponse* resp);
    virtual void HandleHead(const HttpRequest* req, HttpResponse* resp);
    virtual void HandleDelete(const HttpRequest* req, HttpResponse* resp);
    virtual void HandleOptions(const HttpRequest* req, HttpResponse* resp);
    virtual void HandleTrace(const HttpRequest* req, HttpResponse* resp);
    virtual void HandleConnect(const HttpRequest* req, HttpResponse* resp);
protected:
    void MethodNotAllowed(const HttpRequest* req, HttpResponse* resp);
private:
};

} // namespace common

#endif // COMMON_NET_HTTP_SERVER_HANDLER_H
