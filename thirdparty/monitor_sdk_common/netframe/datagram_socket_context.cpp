// Copyright (c) 2010.
// All rights reserved.
//
//

#include "thirdparty/monitor_sdk_common/netframe/datagram_socket_context.h"

#include "thirdparty/monitor_sdk_common/netframe/command_event.h"
#include "thirdparty/monitor_sdk_common/netframe/event_poller.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/this_thread.h"

namespace common {
namespace netframe {

#ifndef EWOULDBLOCK
#define EWOULDBLOCK EAGAIN
#endif

DatagramSocketContext::DatagramSocketContext(
    NetFrame* netframe,
    const SocketAddress* local_address,
    const SocketAddress* remote_address,
    SocketId sock_id,
    DatagramSocketHandler* handler,
    size_t max_packet_size,
    const NetFrame::EndPointOptions& options
) : SocketContext(netframe, local_address, remote_address, sock_id,
    max_packet_size, options)
{
    SetEventHandler(handler);
    m_ReceiveBuffer = static_cast<char*>(MemPool_Allocate(max_packet_size));
    IncreaseReceiveBufferedLength(max_packet_size);
}

DatagramSocketContext::~DatagramSocketContext()
{
    MemPool_Free(m_ReceiveBuffer);
    DecreaseReceiveBufferedLength(m_MaxPacketLength);
}

/// ���ո�Socket�ϵ�������ݰ����ҽ�����Ӧ�Ĵ���
int DatagramSocketContext::HandleInput()
{
    // ��������
    SocketAddressStorage address;
    socklen_t address_length = address.Capacity();
    char* buffer = m_ReceiveBuffer;
    int received_length = (int) recvfrom(GetFd(), buffer, m_MaxPacketLength, 0,
            address.Address(), &address_length);
    if (received_length > 0)
    {
        address.SetLength(address_length);

        // �������ݱ������ҽ����ݱ����ŵ�Socket��RecvBuffer����
        m_Packet.SetContentPtr(m_ReceiveBuffer, received_length);
        m_Packet.SetLocalAddress(m_LocalAddress);
        m_Packet.SetRemoteAddress(address);
        GetEventHandler()->OnReceived(m_Packet);
    }
    else
    {
        int error = ThisThread::GetLastErrorCode();
        if (error != EAGAIN && error != EWOULDBLOCK && error != EINTR)
            return -1;
    }
    return 1;
}

int DatagramSocketContext::HandleOutput()
{
    CommandEvent event;
    while (m_CommandQueue.GetFront(&event))
    {
        if (event.Type == CommandEvent_CloseSocket)
        {
            return false;  // �˿ڹرգ�ɾ��socket context
        }
        Packet* packet = event.Cookie.CastTo<Packet*>();
        int n = SendPacket(packet);
        if (n != 0)
        {
            m_CommandQueue.PopFront();
            if (n > 0)
            {
                DecreaseSendBufferedPacket();
                DecreaseSendBufferedLength(packet->Length());
                if (GetEventHandler()->OnSent(packet))
                    delete packet;
            }
            else
            {
                HandleSendingFailed(packet, m_LastError);
                return n;
            }
        }
        else
        {
            break;
        }
    }
    return 1;
}

// ��Socket�Ϸ������ݱ���Ϣ
int DatagramSocketContext::SendPacket(Packet* packet)
{
    assert(packet != NULL);

    SocketAddressStorage address;
    packet->GetRemoteAddress(address);

    int sent_length = (int) ::sendto(
        GetFd(),
        (char*) packet->Content(), packet->Length(), 0,
        address.Address(), address.Length()
    );

    if (sent_length < 0)
    {
        int error_code = ThisThread::GetLastErrorCode();
        if (error_code == EAGAIN || error_code == EWOULDBLOCK || error_code == EINTR)
            sent_length = 0;
        m_LastError = error_code;
    }

    return sent_length;
}

int DatagramSocketContext::HandleIoEvent(const IoEvent& event)
{
    int result = 0;
    if (event.Mask & IoEvent_Readable)
    {
        result = HandleInput();
        if (result < 0)
            return result;
    }

    if (event.Mask & IoEvent_Writeable)
    {
        result = HandleOutput();
    }

    return result;
}

int DatagramSocketContext::HandleCommandEvent(const CommandEvent& event)
{
    switch (event.Type)
    {
    case CommandEvent_SendUrgentPacket:
        m_CommandQueue.EnqueueUrgent(event);
        break;
    case CommandEvent_SendPacket:
        if (!m_CommandQueue.Enqueue(event))
        {
            Packet* packet = event.Cookie.CastTo<Packet*>();
            HandleSendingFailed(packet, ENOBUFS);
        }
        break;
    case CommandEvent_CloseSocket:
        m_CommandQueue.Enqueue(event, true); // �ӻ�close����ǿ�����
        break;
    case CommandEvent_CloseSocket_Now: // �����ر���������ִ��
        SocketContext::HandleClose();
        break;
    default:
        break;
    }
    return 1;
}

} // namespace netframe
} // namespace common
