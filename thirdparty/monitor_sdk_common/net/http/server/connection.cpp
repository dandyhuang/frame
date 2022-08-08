// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#include "thirdparty/monitor_sdk_common/net/http/server/connection.h"
#include "thirdparty/glog/logging.h"
#include "thirdparty/monitor_sdk_common/base/string/format.h"
#include "thirdparty/monitor_sdk_common/base/string/number.h"
#include "thirdparty/monitor_sdk_common/net/http/request.h"
#include "thirdparty/monitor_sdk_common/net/http/response.h"
#include "thirdparty/monitor_sdk_common/net/uri/uri.h"

namespace common {

HttpConnection::HttpConnection(EventDispatcher* dispatcher, int fd, 
                               std::map<std::string, HttpHandler*>& handle_map)
    : m_watcher(dispatcher, std::bind(&HttpConnection::OnIoEvents, this,
                                      std::placeholders::_1),
                fd, EventMask_Read),
      m_sent_size(0), 
      m_handle_map(handle_map) {
    m_socket.Attach(fd);
    m_watcher.Start();
}

void HttpConnection::Send(const StringPiece& data) {
    m_send_queue.push_back(data.as_string());
}

void HttpConnection::Close() {
}

void HttpConnection::OnIoEvents(int events) {
    LOG(INFO) << "HttpConnection::OnIoEvents";
    if (events & EventMask_Error) {
        LOG(INFO) << "Error";
        m_watcher.Stop();
        return;
    }
    if (events & EventMask_Read) {
        LOG(INFO) << "Read";
        if (!OnReadable())
            return;
    }
    if (events & EventMask_Write) {
        LOG(INFO) << "Write";
        if (!OnWriteable())
            return;
    }

    int new_events = EventMask_Read;
    if (!m_send_queue.empty())
        new_events |= EventMask_Write;
    m_watcher.Set(new_events);
}

bool HttpConnection::OnReadable() {
    size_t kBufferSize = 65536;
    size_t received_size = m_receive_buffer.size();
    m_receive_buffer.resize(received_size + kBufferSize);
    char* buf = &m_receive_buffer[received_size];
    size_t new_received_size;
    if (!m_socket.Receive(buf, kBufferSize, &new_received_size)) {
        OnClosed();
        return false;
    }
    m_receive_buffer.resize(received_size + new_received_size);
    LOG(INFO) << m_receive_buffer;
    HttpRequest request;
    if (request.ParseHeaders(m_receive_buffer)) {
        LOG(INFO) << "Request URI: " << request.Uri();
        HttpResponse response;
        URI uri;
        uri.Parse(request.Uri());
        std::map<std::string, HttpHandler*>::iterator it = m_handle_map.find(uri.Path());
        if (it != m_handle_map.end()) {
            it->second->HandleRequest(&request, &response);
        }
        else {
            response.SetStatus(HttpResponse::Status_NotFound);
            std::string body = StringPrint("not found");
            response.SetBody(body);
            response.SetHeader("Content-Length", NumberToString(body.size()));
        }
        Send(response.ToString());
        m_receive_buffer.clear();
    }

    return true;
}

bool HttpConnection::OnWriteable() {
    while (!m_send_queue.empty()) {
        const std::string& data = m_send_queue.front();
        size_t data_size = data.size() - m_sent_size;
        size_t sent_size;
        if (m_socket.Send(data.data() + m_sent_size, data_size, &sent_size)) {
            if (sent_size == data_size) {
                m_send_queue.pop_front();
                m_sent_size = 0;
            } else {
                m_sent_size += sent_size;
                break;
            }
        } else {
            break;
        }
    }
    return true;
}

void HttpConnection::OnClosed() {
    m_watcher.Stop();
}

} // namespace common
