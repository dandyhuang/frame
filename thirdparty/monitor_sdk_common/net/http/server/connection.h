// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#ifndef COMMON_NET_HTTP_SERVER_CONNECTION_H
#define COMMON_NET_HTTP_SERVER_CONNECTION_H
#pragma once

#include <list>
#include <string>
#include <vector>
#include <map>
#include "thirdparty/monitor_sdk_common/base/string/string_piece.h"
#include "thirdparty/monitor_sdk_common/system/event_dispatcher/event_dispatcher.h"
#include "thirdparty/monitor_sdk_common/system/net/socket.h"
#include "thirdparty/monitor_sdk_common/net/http/server/handler.h"

namespace common {

class HttpConnection {
    COMMON_DECLARE_UNCOPYABLE(HttpConnection);

public:
    HttpConnection(EventDispatcher* dispatcher, int fd, 
                   std::map<std::string, HttpHandler*>& handle_map);
    void Send(const StringPiece& data);
    void Close();

private:
    void OnIoEvents(int events);
    void OnError();
    bool OnReadable();
    bool OnWriteable();
    void OnClosed();

private:
    StreamSocket m_socket;
    IoEventWatcher m_watcher;
    std::string m_receive_buffer;
    std::list<std::string> m_send_queue;
    size_t m_sent_size;
    std::map<std::string, HttpHandler*>& m_handle_map;
};

} // namespace common

#endif // COMMON_NET_HTTP_SERVER_CONNECTION_H
