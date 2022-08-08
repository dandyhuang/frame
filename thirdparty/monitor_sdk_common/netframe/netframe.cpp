// Copyright (c) 2010.
// All rights reserved.
//
//

#include "thirdparty/monitor_sdk_common/netframe/netframe.h"

#include "thirdparty/monitor_sdk_common/netframe/command_event.h"
#include "thirdparty/monitor_sdk_common/netframe/datagram_socket_context.h"
#include "thirdparty/monitor_sdk_common/netframe/listen_socket_context.h"
#include "thirdparty/monitor_sdk_common/netframe/stream_socket_context.h"
#include "thirdparty/monitor_sdk_common/netframe/work_thread.h"

#include "thirdparty/monitor_sdk_common/system/concurrency/atomic/atomic.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/thread.h"
#include "thirdparty/monitor_sdk_common/system/memory/mempool.h"
#include "thirdparty/monitor_sdk_common/system/net/socket.h"
#include "thirdparty/monitor_sdk_common/system/system_information.h"

#include "thirdparty/glog/logging.h"

namespace common {
namespace netframe {

using namespace std;

uint32_t NetFrame::m_SequenceId = 0;

NetFrame::NetFrame(unsigned int num_workthreads, size_t max_bufferd_size):
    m_CurrentSendBufferedMemorySize(0), m_SendBufferedPacketNumber(0),
    m_CurrentReceiveBufferedMemorySize(0)
{
#ifdef _WIN32
    WSADATA wsaData;
    ::WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif // _WIN32
    if (num_workthreads == 0)
    {
        num_workthreads = GetLogicalCpuNumber();
    }

    m_MaxSendBufferedMemorySize = max_bufferd_size;
    MemPool_Initialize(MemPool::MAX_UNIT_SIZE);

    for (size_t i = 0; i < num_workthreads; ++i)
    {
        WorkThread* work_thread = new WorkThread(this);
        m_WorkThreads.push_back(work_thread);
    }
    for (size_t i = 0; i < num_workthreads; ++i)
    {
        m_WorkThreads[i]->Start();
    }
}

NetFrame::~NetFrame()
{
    for (size_t i = 0; i < m_WorkThreads.size(); ++i)
    {
        delete m_WorkThreads[i];
    }
    m_WorkThreads.clear();

    // ������ϴ���
    m_MaxSendBufferedMemorySize = 0;
    m_CurrentSendBufferedMemorySize = 0;
    m_CurrentReceiveBufferedMemorySize = 0;
    m_SendBufferedPacketNumber = 0;
#ifdef _WIN32
    ::WSACleanup();
#endif // _WIN32
}

WorkThread* NetFrame::GetWorkThreadOfFd(int fd)
{
#ifdef _WIN32
    // socket on win32 is always multiple of 4
    size_t index = (fd / 4) % m_WorkThreads.size();
#else
    size_t index = fd % m_WorkThreads.size();
#endif
    return m_WorkThreads[index];
}

int64_t NetFrame::AsyncListen(
    const SocketAddress& address,
    ListenSocketHandler* handler,   // Socket�ϵĴ�����
    size_t max_packet_size,         // Socket���������������ݰ��Ĵ�С
    const EndPointOptions& options
)
{
    CHECK_NOTNULL(handler);

    ListenerSocket listener;
    if (!listener.Create(address.Family(), SOCK_STREAM))
    {
        return -ThisThread::GetLastErrorCode();
    }

    listener.SetCloexec();
    listener.SetBlocking(false);
    listener.SetReceiveBufferSize(options.ReceiveBufferSize());
    listener.SetSendBufferSize(options.SendBufferSize());
    listener.SetReuseAddress();
    int priority = options.Priority();
    if (priority > 0) {
#ifndef _WIN32
        listener.SetOption(SOL_IP, IP_TOS, static_cast<uint8_t>(priority));
        listener.SetOption(SOL_SOCKET, SO_PRIORITY, priority);
#endif
    }

    if (!listener.Bind(address))
        return -ThisThread::GetLastErrorCode();

    if (!listener.Listen())
        return -ThisThread::GetLastErrorCode();

    SocketId sock_id = GenerateSocketId(listener.Handle());
    ListenSocketContext* socket_context = new ListenSocketContext(
        this,
        &address,
        sock_id,
        handler,
        max_packet_size,
        options
    );

    listener.Detach();
    handler->SetEndPointId(sock_id);

    if (!AddCommandEvent(CommandEvent(CommandEvent_AddSocket, sock_id,
                    static_cast<SocketContext*>(socket_context))))
    {
        delete socket_context;
        return -EINVAL;
    }

    return sock_id.Id;
}

int64_t NetFrame::AsyncConnect(
    const SocketAddress* local_address,
    const SocketAddress& remote_address,
    StreamSocketHandler* handler,   ///< Socket�ϵĴ�����
    size_t max_packet_size,         ///< Socket���������������ݰ��Ĵ�С
    const EndPointOptions& options
)
{
    CHECK_NOTNULL(handler);

    StreamSocket socket;
    if (!socket.Create(remote_address.Family()))
        return -ThisThread::GetLastErrorCode();

    socket.SetCloexec();
    socket.SetBlocking(false);

    if (local_address && !socket.Bind(*local_address))
        return -ThisThread::GetLastErrorCode();

    socket.SetTcpNoDelay();
    socket.SetKeepAlive();

    if (!socket.Connect(remote_address))
    {
        int error = ThisThread::GetLastErrorCode();
        if ((error != EAGAIN) && (error != EINPROGRESS))
            return -error;
    }

    // ���÷��ͻ������ͽ��ջ������Ĵ�С
    socket.SetSendBufferSize(options.SendBufferSize());
    socket.SetReceiveBufferSize(options.ReceiveBufferSize());

    int priority = options.Priority();
    if (priority > 0)
    {
        // win32�²���Ҫ����,���������е�����
#ifndef _WIN32
        socket.SetOption(SOL_IP, IP_TOS, static_cast<uint8_t>(priority));
        socket.SetOption(SOL_SOCKET, SO_PRIORITY, priority);
#endif
    }

    SocketId sock_id = GenerateSocketId(socket.Handle());
    SocketContext* socket_context = new StreamSocketContext(
        this, local_address, &remote_address, sock_id,
        handler, max_packet_size, options,
        false // not connected
    );

    socket.Detach();
    handler->SetEndPointId(sock_id);

    handler->SetRemoteAddress(remote_address);

    if (!AddCommandEvent(CommandEvent(CommandEvent_AddSocket, sock_id, socket_context)))
    {
        delete socket_context;
        return -EINVAL;
    }
    return sock_id.Id;
}

int64_t NetFrame::AsyncConnect(
    const SocketAddress& local_address,
    const SocketAddress& remote_address,
    StreamSocketHandler* handler,   ///< Socket�ϵĴ�����
    size_t  max_packet_size,        ///< Socket���������������ݰ��Ĵ�С
    const EndPointOptions&  options
)
{
    return AsyncConnect(&local_address, remote_address, handler, max_packet_size, options);
}

int64_t NetFrame::AsyncConnect(
    const SocketAddress& remote_address,
    StreamSocketHandler* handler,       ///< Socket�ϵĴ�����
    size_t max_packet_size,             ///< Socket���������������ݰ��Ĵ�С
    const EndPointOptions& options
)
{
    return AsyncConnect(NULL, remote_address, handler, max_packet_size, options);
}

int64_t NetFrame::AsyncDatagramBind(
    const SocketAddress& address,
    DatagramSocketHandler* handler, ///< Socket�ϵĴ�����
    size_t max_packet_size,         ///< Socket���������������ݰ��Ĵ�С
    const EndPointOptions& options
)
{
    CHECK_NOTNULL(handler);

    DatagramSocket socket;
    if (!socket.Create(address.Family()))
        return -ThisThread::GetLastErrorCode();

    socket.SetCloexec();
    socket.SetBlocking(false);
    socket.SetSendBufferSize(options.SendBufferSize());
    socket.SetReceiveBufferSize(options.ReceiveBufferSize());
    int priority = options.Priority();
    if (priority > 0)
    {
#ifndef _WIN32
        socket.SetOption(SOL_IP, IP_TOS, static_cast<uint8_t>(priority));
        socket.SetOption(SOL_SOCKET, SO_PRIORITY, priority);
#endif
    }

    if (!socket.Bind(address))
        return -ThisThread::GetLastErrorCode();

    SocketId sock_id = GenerateSocketId(socket.Handle());
    SocketContext* socket_context = new DatagramSocketContext(
        this, &address, NULL, sock_id, handler, max_packet_size, options);

    socket.Detach(); // ��ֹfd����ʱ���ر�
    handler->SetEndPointId(sock_id);

    if (!AddCommandEvent(CommandEvent(CommandEvent_AddSocket, sock_id, socket_context)))
    {
        delete socket_context;
        return -EINVAL;
    }

    return sock_id.Id;
}

bool NetFrame::AddCommandEvent(const CommandEvent& event)
{
    WorkThread* work_thread = GetWorkThreadOfFd(event.GetFd());
    CHECK_NOTNULL(work_thread);
    work_thread->AddCommandEvent(event);
    return true;
}

int NetFrame::CloseEndPoint(EndPoint& endpoint, bool immidiate)
{
    CHECK(endpoint.IsValid());

    WorkThread* work_thread = GetWorkThreadOfFd(endpoint.GetFd());
    CHECK_NOTNULL(work_thread);
    if (immidiate) // ����ɾ��һ���˿�
    {
        work_thread->AddCommandEvent(CommandEvent(
                CommandEvent_CloseSocket_Now, endpoint.GetSockId()));
    }
    else // �ر�һ���˿ڣ��ȴ�֮ǰ�����ݰ�����
    {
        work_thread->AddCommandEvent(CommandEvent(CommandEvent_CloseSocket,
                                                  endpoint.GetSockId()));
    }
    return 0;
}

int NetFrame::SendPacket(const StreamEndPoint& endpoint, const void* data,
        size_t size, bool urgent)
{
    CHECK(endpoint.IsValid());

    Packet* packet = new Packet();
    packet->SetContent(data, size);
    int result = SendPacket(endpoint, packet, urgent);
    if (result < 0)
    {
        delete packet;
    }
    return result;
}

int NetFrame::SendPacket(const StreamEndPoint& endpoint, Packet* packet, bool urgent)
{
    CHECK(endpoint.IsValid());

    size_t total_buffered_length = GetCurrentSendBufferedLength();
    CommandEventType command_type = urgent ? CommandEvent_SendUrgentPacket :
                                             CommandEvent_SendPacket;
    // �ڴ����û�������Ϊ�������ݰ�
    if ((total_buffered_length + packet->Length()) < m_MaxSendBufferedMemorySize || urgent)
    {
        WorkThread* work_thread = GetWorkThreadOfFd(endpoint.GetFd());
        if (work_thread)
        {
            IncreaseSendBufferedLength(packet->Length());
            IncreaseSendBufferedPacket();
            work_thread->AddCommandEvent(CommandEvent(command_type, endpoint.GetSockId(), packet));
            return 0;
        }
    } else {
        LOG_EVERY_N(WARNING, 100) << "SendPacket failed, current buffer: " << total_buffered_length
                                  << ", packet length: " << packet->Length()
                                  << ", max buffer: " << m_MaxSendBufferedMemorySize;
    }
    return -1;
}

int NetFrame::SendPacket(const DatagramEndPoint& endpoint,
        const SocketAddress& address, Packet* packet)
{
    CHECK(endpoint.IsValid());

    packet->SetRemoteAddress(address);
    WorkThread* work_thread = GetWorkThreadOfFd(endpoint.GetFd());
    if (work_thread)
    {
        IncreaseSendBufferedLength(packet->Length());
        IncreaseSendBufferedPacket();
        work_thread->AddCommandEvent(CommandEvent(CommandEvent_SendPacket,
                    endpoint.GetSockId(), packet));
        return 0;
    }
    return -1;
}

int NetFrame::SendPacket(
    const DatagramEndPoint& endpoint,
    const SocketAddress& address,
    const void* data, size_t size
)
{
    CHECK(endpoint.IsValid());

    Packet* packet = new Packet;
    packet->SetContent(data, size);
    int result = SendPacket(endpoint, address, packet);
    if (result < 0)
    {
        delete packet;
    }
    return result;
}

/************************************************************************/
/* �ײ���ú���                                                         */
/************************************************************************/

int NetFrame::CloseFd(int fd)
{
#if _WIN32
    return closesocket(fd);
#elif __unix__
    return close(fd);
#endif
}

void NetFrame::IncreaseSendBufferedLength(size_t length)
{
    AtomicAdd(&m_CurrentSendBufferedMemorySize, length);
}

void NetFrame::DecreaseSendBufferedLength(size_t length)
{
    AtomicSub(&m_CurrentSendBufferedMemorySize, length);
}

void NetFrame::IncreaseReceiveBufferedLength(size_t length)
{
    AtomicAdd(&m_CurrentReceiveBufferedMemorySize, length);
}

void NetFrame::DecreaseReceiveBufferedLength(size_t length)
{
    AtomicSub(&m_CurrentReceiveBufferedMemorySize, length);
}

size_t NetFrame::GetCurrentSendBufferedLength() const
{
    return AtomicGet(&m_CurrentSendBufferedMemorySize);
}

size_t NetFrame::GetCurrentReceiveBufferedLength() const
{
    return AtomicGet(&m_CurrentReceiveBufferedMemorySize);
}

size_t NetFrame::GetMaxSendBufferedLength() const
{
    return  m_MaxSendBufferedMemorySize;
}

void NetFrame::IncreaseSendBufferedPacket()
{
    AtomicIncrement(&m_SendBufferedPacketNumber);
}

void NetFrame::DecreaseSendBufferedPacket()
{
    AtomicDecrement(&m_SendBufferedPacketNumber);
}

size_t NetFrame::GetSendBufferedPacketNumber()
{
    return AtomicGet(&m_SendBufferedPacketNumber);
}

bool NetFrame::WaitForSendingComplete(int timeout)
{
    int time_interval = 10;
    int total_wait_time = 0;
    while (GetSendBufferedPacketNumber() > 0)
    {
        ThisThread::Sleep(time_interval);
        total_wait_time += time_interval;
        if (total_wait_time >= timeout && timeout > 0)
            break;
    }
    if (GetSendBufferedPacketNumber() > 0)
        return false;
    return true;
}

/// ��sock fd�����к�ƴ�ӳ�64λ��id
SocketId NetFrame::GenerateSocketId(int32_t sock_fd)
{
    SocketId id;
    id.SequenceId = AtomicIncrement(&m_SequenceId);
    id.SockFd = sock_fd;
    return id;
}

} // namespace netframe
} // namespace common

#ifdef _MSC_VER
#pragma comment(lib, "ws2_32.lib")
#endif
