// Copyright (c) 2010.
// All rights reserved.
//
//

#ifndef COMMON_NETFRAME_DATAGRAM_SOCKET_CONTEXT_H
#define COMMON_NETFRAME_DATAGRAM_SOCKET_CONTEXT_H

#include "thirdparty/monitor_sdk_common/netframe/socket_context.h"

namespace common {
namespace netframe {

class DatagramSocketContext : public SocketContext
{
public:
    DatagramSocketContext(
        NetFrame* netframe,
        const SocketAddress* local_address,
        const SocketAddress* remote_address,
        SocketId sock_id,
        DatagramSocketHandler* handler,
        size_t max_packet_size,
        const NetFrame::EndPointOptions& options
    );
    virtual ~DatagramSocketContext();

    virtual int HandleIoEvent(const IoEvent& event);
    virtual int HandleCommandEvent(const CommandEvent& event);

private:
    /// ���ո�Socket�ϵ�������ݰ����ҽ�����Ӧ�Ĵ���
    int HandleInput();

    /// �ڸ�Socket�ϴ���д�¼�
    int HandleOutput();

    /// ��Socket�Ϸ������ݱ���Ϣ
    int SendPacket(Packet* packet);
    virtual DatagramSocketHandler* GetEventHandler() const
    {
        return static_cast<DatagramSocketHandler*>(SocketContext::GetEventHandler());
    }
private:
    char* m_ReceiveBuffer;
    Packet m_Packet;
};

} // namespace netframe
} // namespace common

#endif // COMMON_NETFRAME_DATAGRAM_SOCKET_CONTEXT_H
