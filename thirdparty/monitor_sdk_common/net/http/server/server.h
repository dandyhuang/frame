// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#ifndef COMMON_NET_HTTP_SERVER_SERVER_H
#define COMMON_NET_HTTP_SERVER_SERVER_H
#pragma once

#include <map>
#include <string>
#include "thirdparty/monitor_sdk_common/base/scoped_ptr.h"
#include "thirdparty/monitor_sdk_common/base/uncopyable.h"
#include "thirdparty/monitor_sdk_common/system/net/socket_address.h"

namespace common {

class HttpHandler;

class HttpServer {
    COMMON_DECLARE_UNCOPYABLE(HttpServer);

public:
    HttpServer();
    virtual ~HttpServer();
    bool RegisterHttpHandler(const std::string& path, HttpHandler* handler);
    bool Bind(const SocketAddress& address, SocketAddress* real_address = NULL);
    bool Start();
    void Close();
    void Run();

private:
    struct Impl;
    scoped_ptr<Impl> m_impl;
};

} // namespace common

#endif // COMMON_NET_HTTP_SERVER_SERVER_H
