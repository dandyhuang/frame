// Copyright (c) 2010.
// All rights reserved.
//
//

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX 1
#endif
#endif

#include "thirdparty/monitor_sdk_common/netframe/stream_socket_context.h"

#include "thirdparty/monitor_sdk_common/netframe/command_event.h"
#include "thirdparty/monitor_sdk_common/netframe/event_poller.h"
#include "thirdparty/monitor_sdk_common/netframe/netframe.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/this_thread.h"

namespace common {
namespace netframe {

#ifndef EWOULDBLOCK
#define EWOULDBLOCK EAGAIN
#endif

#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL 0
#endif

const size_t DefaultBufferSize = 0x4000;

StreamSocketContext::StreamSocketContext(
    NetFrame* netframe,
    const SocketAddress* local_address,
    const SocketAddress* remote_address,
    SocketId sock_id,
    StreamSocketHandler* handler,
    size_t max_packet_size,
    const NetFrame::EndPointOptions& options,
    bool connected
) : SocketContext(netframe, local_address, remote_address, sock_id,
    max_packet_size, options),
    m_Connected(connected),
    m_SentLength(0),
    m_MinBufferLength(std::min(max_packet_size, DefaultBufferSize)),
    m_ReceiveBuffer(NULL),
    m_ReceivedLength(0),
    m_DetectedPacketLength(0)
{
    SetEventHandler(handler);
    m_ReceiveBufferLength = m_MinBufferLength;
    m_ReceiveBuffer = static_cast<char*>(MemPool_Allocate(m_ReceiveBufferLength));
    IncreaseReceiveBufferedLength(m_ReceiveBufferLength);
    m_ReceivePacket.SetLocalAddress(m_LocalAddress);
    m_ReceivePacket.SetRemoteAddress(m_RemoteAddress);
}

StreamSocketContext::~StreamSocketContext()
{
    DecreaseReceiveBufferedLength(m_ReceiveBufferLength);
    MemPool_Free(m_ReceiveBuffer);
}

unsigned int StreamSocketContext::GetWantedEventMask() const
{
    unsigned int result = IoEvent_Readable;
    if (m_Connected)
    {
        if (!m_CommandQueue.IsEmpty())
        {
            result |= IoEvent_Writeable;
        }
    }
    else
    {
        result |= IoEvent_Writeable | IoEvent_Connected;
    }
    return result;
}

void StreamSocketContext::ResizeReceiveBuffer(size_t new_size)
{
    char* new_buffer = (char*) MemPool_Allocate(new_size);
    memcpy(new_buffer, m_ReceiveBuffer, m_ReceivedLength);
    MemPool_Free(m_ReceiveBuffer);
    m_ReceiveBuffer = new_buffer;
    if (m_ReceiveBufferLength > new_size) {
        DecreaseReceiveBufferedLength(m_ReceiveBufferLength - new_size);
    } else {
        IncreaseReceiveBufferedLength(new_size - m_ReceiveBufferLength);
    }
    m_ReceiveBufferLength = new_size;
}

void StreamSocketContext::AdjustReceiveBufferSize()
{
    // �����µĺ��ʵĻ�������С
    size_t new_size = m_ReceiveBufferLength;
    if (m_DetectedPacketLength > 0)
    {
        // ����������֪���������˳���
        new_size = m_DetectedPacketLength;
    }
    else
    {
        // ���ջ�����������һ������
        if (m_ReceivedLength > m_ReceiveBufferLength / 2)
        {
            new_size = 2 * m_ReceiveBufferLength;
        }
    }

    if (new_size > m_MaxPacketLength)
        new_size = m_MaxPacketLength;

    // ���ܵ�����̫С���Ⲩ��̫��Ӱ������
    if (new_size < m_MinBufferLength)
        new_size = m_MinBufferLength;

    if (new_size > m_ReceiveBufferLength)
    {
        // �����������������
        ResizeReceiveBuffer(new_size);
    }
    else
    {
        // �������С��ֻ�в��ϴ�ʱ�ŵ���������Ƶ����������̫��
        if (m_ReceiveBufferLength / 2 > new_size)
        {
            ResizeReceiveBuffer(new_size);
        }
    }
}

bool StreamSocketContext::HandleInput()
{
    size_t total_num_packets = 0;
    size_t total_received_length = 0;

    for (;;)
    {
        AdjustReceiveBufferSize();

        char* buffer = m_ReceiveBuffer + m_ReceivedLength;
        size_t buffer_length = m_ReceiveBufferLength - m_ReceivedLength;

        int received_length = Receive(buffer, buffer_length);
        if (received_length < 0) {
            return false;
        }

        if (received_length > 0)
        {
            total_received_length += received_length;
            m_ReceivedLength += received_length;

            int num_packets = SplitAndIndicatePackets();
            if (num_packets < 0) {
                return false;
            }

            if (num_packets == 0 && m_ReceivedLength == m_MaxPacketLength) {
                return false;
            }

            total_num_packets += num_packets;
        }
        // ������δ�����������ٳ��ԣ��˳���
        if ((size_t)received_length < buffer_length)
            break;

        // ÿ�δ���϶����ϳ�����֮��� Socket �ϵĶ��¼���Ҫ��������,
        // ������ һ��Socket �����ܵ� Rush ����������ִ����������
        if (total_num_packets > 1000 || total_received_length > 0x10000)
        {
            break;
        }
    }

    return true;
}

int StreamSocketContext::CheckAndIndicatePacket(char*& buffer, int& left_length)
{
    if (m_DetectedPacketLength <= 0) {
        m_DetectedPacketLength = GetEventHandler()->DetectPacketSize(buffer, left_length);
        if (m_DetectedPacketLength == 0) {
            return 0;
        }

        if (m_DetectedPacketLength > static_cast<int>(m_MaxPacketLength))
        {
            // ���ݰ���С��������
            m_LastError = EMSGSIZE;
            return -1;
        }

        if (m_DetectedPacketLength < 0) {
            // ��⵽��������ݰ�
            m_LastError = EBADMSG;
            return -1;
        }
    }

    if (m_DetectedPacketLength <= left_length)
    {
        // ���յ����������ݰ�
        m_ReceivePacket.SetContentPtr(buffer, m_DetectedPacketLength);
        GetEventHandler()->OnReceived(m_ReceivePacket);

        // �Ƶ���һ����ͷ
        buffer += m_DetectedPacketLength;
        left_length -= m_DetectedPacketLength;
        m_DetectedPacketLength = 0;
        return 1;
    }
    else
    {
        return 0;
    }
}

int StreamSocketContext::SplitAndIndicatePackets()
{
    char* begin = m_ReceiveBuffer;
    int left_length = static_cast<int>(m_ReceivedLength);

    int num_packets = 0;
    while (left_length > 0) {
        int result = CheckAndIndicatePacket(begin, left_length);
        if (result < 0)
            return -1;

        if (result == 0) {
            break;
        }

        ++num_packets;
    }

    // ʣ�ಿ�ֿ�������ͷ
    if (m_ReceiveBuffer != begin && left_length > 0)
        ::memmove(m_ReceiveBuffer, begin, left_length);
    m_ReceivedLength = left_length;

    return num_packets;
}

// ���ܵ�ǰSocket�ϵ������������
int StreamSocketContext::Receive(void* buffer, size_t buffer_length)
{
    assert(buffer != NULL);
    assert(buffer_length > 0);

    int received_length = -1;
    int error_code = 0;

    do
    {
        received_length = (int) ::recv(GetFd(), (char*) buffer, buffer_length, 0);
        error_code = ThisThread::GetLastErrorCode();
    } while ((received_length == -1) && (error_code == EINTR));

    if (received_length == -1)
    {
        if ((error_code == EAGAIN) || (error_code == EWOULDBLOCK))
        {
            received_length = 0;
        }
        else
        {
            m_LastError = error_code;
            // XXX: need more handle
            return -1;
        }
    }
    else if (received_length == 0)
    {
        received_length = -1;
    }

    return  received_length;
}

// �ڸ�Socket�ϴ���д�¼�
bool StreamSocketContext::HandleOutput()
{
    for (;;)
    {
        CommandEvent event;
        if (m_CommandQueue.GetFront(&event))
        {
            if (event.Type == CommandEvent_CloseSocket)
            {
                return false;  // �ѹرգ�ɾ��socket context
            }
            // �ǹر��¼����������ݰ��¼�
            Packet* packet = event.Cookie.CastTo<Packet*>();
            assert(packet);
            if (packet->Length() <= m_MaxPacketLength)
            {
                int sent_result = SendPacket(packet);
                if (sent_result > 0)
                {
                    m_CommandQueue.PopFront();
                    DecreaseSendBufferedLength(packet->Length());
                    DecreaseSendBufferedPacket();

                    GetEventHandler()->SetCommandQueueLength(m_CommandQueue.Size());
                    if (GetEventHandler()->OnSent(packet))
                        delete packet;

                    m_SentLength = 0;
                }
                else
                {
                    return sent_result == 0;
                }
            }
            else // packet too long
            {
                m_CommandQueue.PopFront();
                HandleSendingFailed(packet, EMSGSIZE);
            }
        }
        else
        {
            break;
        }
    }

    return true;
}

int StreamSocketContext::SendPacket(Packet* packet)
{
    for (;;)
    {
        unsigned char* sending_buffer = packet->Content() + m_SentLength;
        size_t sending_length = packet->Length() - m_SentLength;

        assert(sending_length > 0);
        int sent_length = SendBuffer(sending_buffer, sending_length);
        if (sent_length > 0)
        {
            m_SentLength +=  sent_length;
            if (m_SentLength == packet->Length())
                return 1;
        }
        else
        {
            return sent_length;
        }
    }
}

int StreamSocketContext::SendBuffer(const void* buffer, size_t size)
{
    assert(size > 0);

    int flags = 0;

    flags |= MSG_NOSIGNAL;

#ifdef MSG_MORE
    // ��Ϊ������ tcp nodelay, Ĭ�ϻ�����������
    // ��������������������� MSG_MORE �Ա�һ���͡�
    if (m_CommandQueue.HasMore())
    {
        flags |= MSG_MORE;
    }
#endif

    for (;;)
    {
        int sent_length = (int) ::send(GetFd(), (char*) buffer, size, flags);
        if (sent_length > 0)
            return sent_length;

        int error_code = ThisThread::GetLastErrorCode();

        if (error_code == EAGAIN)
        {
            return 0;
        }
        else if (error_code != EINTR)
        {
            m_LastError = error_code;
            return -1;
        }
    }
}

int StreamSocketContext::HandleCommandEvent(const CommandEvent& event)
{
    switch (event.Type)
    {
    case CommandEvent_AddSocket:
        return 1;
    case CommandEvent_CloseSocket_Now:
        SocketContext::HandleClose();
        return 1;
    case CommandEvent_CloseSocket:
    {
        m_CommandQueue.Enqueue(event, true); // �ر��¼�ǿ�����
        if (!HandleOutput())
            return -1;
    }
    break;
    case CommandEvent_SendPacket:
    {
        if (!m_CommandQueue.Enqueue(event)) // ������, ����ʧ��
        {
            Packet* packet = event.Cookie.CastTo<Packet*>();
            HandleSendingFailed(packet, ENOBUFS);
        }
        if (!HandleOutput())
            return -1;
    }
    break;
    case CommandEvent_SendUrgentPacket:
    {
        m_CommandQueue.EnqueueUrgent(event);
        if (!HandleOutput())
            return -1;
    }
    break;
    default:
        assert(!"unexpacted event");
        return 0;
    }
    return 1;
}

int StreamSocketContext::HandleIoEvent(const IoEvent& event)
{
    if (event.Mask & IoEvent_Closed)
    {
        if (event.ErrorCode != 0)
            m_LastError = event.ErrorCode;
        else
            HandleInput();
        return -1;
    }

    if (m_Connected)
    {
        if (event.Mask & IoEvent_Writeable)
        {
            if (!HandleOutput())
                return -1;
        }

        if (event.Mask & IoEvent_Readable)
        {
            if (!HandleInput())
                return -1;
        }
    }
    else
    {
        if (event.Mask & (IoEvent_Connected | IoEvent_Readable | IoEvent_Writeable))
        {
            m_Connected = true;
            GetEventHandler()->OnConnected();
        }
    }

    return 1;
}

} // namespace netframe
} // namespace common
