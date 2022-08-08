// Copyright (c) 2010.
// All rights reserved.
//
//

#ifndef COMMON_NETFRAME_LISTEN_SOCKET_CONTEXT_H
#define COMMON_NETFRAME_LISTEN_SOCKET_CONTEXT_H

#include "thirdparty/monitor_sdk_common/netframe/socket_context.h"

namespace common {
namespace netframe {

class NetFrame;

class ListenSocketContext : public SocketContext {
public:
    ListenSocketContext(
        NetFrame* netframe,
        const SocketAddress* local_address,
        SocketId sock_id,
        ListenSocketHandler* handler,
        size_t max_packet_size,
        const NetFrame::EndPointOptions& options
    ) : SocketContext(netframe, local_address, NULL, sock_id, max_packet_size, options)
    {
        SetEventHandler(handler);
    }
    virtual int HandleIoEvent(const IoEvent& event); // override
    virtual int HandleCommandEvent(const CommandEvent& event); // override

private:
    /// 在监听端口上接收新的连接请求事件
    int HandleAccept();
    virtual unsigned int GetWantedEventMask() const; // override

    virtual ListenSocketHandler* GetEventHandler() const
    {
        return static_cast<ListenSocketHandler*>(
            SocketContext::GetEventHandler());
    }
};

} // namespace netframe
} // namespace common

#endif // COMMON_NETFRAME_LISTEN_SOCKET_CONTEXT_H
