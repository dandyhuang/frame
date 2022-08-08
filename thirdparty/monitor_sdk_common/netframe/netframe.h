// Copyright (c) 2010.
// All rights reserved.
//
//

#ifndef COMMON_NETFRAME_NETFRAME_H
#define COMMON_NETFRAME_NETFRAME_H

#include <string>
#include <vector>
#include "thirdparty/monitor_sdk_common/base/annotation.h"
#include "thirdparty/monitor_sdk_common/netframe/command_event.h"
#include "thirdparty/monitor_sdk_common/netframe/packet.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/mutex.h"
#include "thirdparty/monitor_sdk_common/system/net/socket.h"

namespace common {
namespace netframe {

class WorkThread;
class SocketContext;
class ListenSocketContext;

class SocketHandler;
class ListenSocketHandler;
class DatagramSocketHandler;
class StreamSocketHandler;

/// �첽����ͨѶ��.
/// ֧��ͨ������ SendPacket ���������ͣ�ͨ�� handler �������ա�
class NetFrame
{
    friend class SocketContext;
    friend class WorkThread;
    friend class ListenSocketContext;

public:
    static const size_t DEFAULT_MAX_COMMAND_QUEUE_LENGTH = 100000;
    static const size_t MAX_BUFFERED_SIZE = 1 * 1024 * 1024 * 1024L;

    enum ErrorCode
    {
        ErrorCode_None = 0,
        ErrorCode_QueueFull,
        ErrorCode_SocketClosed,
        ErrorCode_SystemError
    };

    class EndPoint
    {
    public:
        friend class NetFrame;
        friend class SocketHandler;
        EndPoint() : m_SockId(-1)
        {
        }
        SocketId GetSockId() const
        {
            return m_SockId;
        }
        int64_t GetId() const
        {
            return m_SockId.Id;
        }
        int GetFd() const
        {
            return m_SockId.SockFd;
        }
        bool IsValid() const
        {
            return GetFd() >= 0;
        }
        bool operator == (const EndPoint& rhs)
        {
            return m_SockId == rhs.m_SockId;
        }
        bool operator != (const EndPoint& rhs)
        {
            return !(m_SockId == rhs.m_SockId);
        }

        explicit EndPoint(SocketId id): m_SockId(id) {}
        explicit EndPoint(int64_t id)
        {
            m_SockId.Id = id;
        }

    private:
        void SetEndPointId(SocketId id)
        {
            m_SockId = id;
        }
        SocketId m_SockId;
    };

    class ListenEndPoint : public EndPoint
    {
        friend class ListenSocketHandler;
    public:
        ListenEndPoint() {}
    private:
        ListenEndPoint(SocketId id) : EndPoint(id) {}
    };

    class StreamEndPoint : public EndPoint
    {
        friend class StreamSocketHandler;
    public:
        StreamEndPoint() {}
    private:
        StreamEndPoint(SocketId id) : EndPoint(id) {}
    };

    class DatagramEndPoint : public EndPoint
    {
        friend class DatagramSocketHandler;
    public:
        DatagramEndPoint() {}
    private:
        DatagramEndPoint(SocketId id) : EndPoint(id) {}
    };

    class EndPointOptions
    {
    public:
        EndPointOptions() :
            m_MaxCommandQueueLength(DEFAULT_MAX_COMMAND_QUEUE_LENGTH),
            m_SendBufferSize(32 * 1024),
            m_ReceiveBufferSize(32 * 1024),
            m_Priority(0)
        {
        }

        size_t MaxCommandQueueLength() const
        {
            return m_MaxCommandQueueLength;
        }
        EndPointOptions& MaxCommandQueueLength(size_t value)
        {
            m_MaxCommandQueueLength = value;
            return *this;
        }
        size_t SendBufferSize() const
        {
            return m_SendBufferSize;
        }
        EndPointOptions& SendBufferSize(size_t value)
        {
            m_SendBufferSize = value;
            return *this;
        }
        size_t ReceiveBufferSize() const
        {
            return m_ReceiveBufferSize;
        }
        EndPointOptions& ReceiveBufferSize(size_t value)
        {
            m_ReceiveBufferSize = value;
            return *this;
        }

        // QoS priority, should be 0~6
        int Priority() const
        {
            return m_Priority;
        }

        EndPointOptions& Priority(int value)
        {
            m_Priority = value;
            return *this;
        }
    private:
        size_t m_MaxCommandQueueLength;
        size_t m_SendBufferSize;
        size_t m_ReceiveBufferSize;
        int m_Priority;
    };

    /// @param num_workthreads �����߳���Ŀ, 0 ����ϵͳ�߼� CPU ��Ŀ
    /// @param max_bufferd_size ֧�ֵ��������͵İ������ֽڴ�С
    NetFrame(unsigned int num_workthreads = 0, size_t max_bufferd_size = MAX_BUFFERED_SIZE);
    ~NetFrame();

    /// @retval >0 socket id
    /// @retval <0 -errno
    int64_t AsyncListen(
        const SocketAddress& address,   ///< Ҫ�����ı��ص�ַ
        ListenSocketHandler* handler,   ///< Socket�ϵĴ�����
        size_t max_packet_size,         ///< Socket���������������ݰ��Ĵ�С
        const EndPointOptions& options = EndPointOptions()
    );

    /// @retval >0 socket id
    /// @retval <0 -errno
    int64_t AsyncConnect(
        const SocketAddress& local_address,     ///< ���ص�ַ
        const SocketAddress& remote_address,    ///< Զ�˵�ַ
        StreamSocketHandler* handler,           ///< Socket�ϵĴ�����
        size_t max_packet_size,                 ///< Socket���������������ݰ��Ĵ�С
        const EndPointOptions& options = EndPointOptions()
    );

    /// @retval >0 socket id
    /// @retval <0 -errno
    int64_t AsyncConnect(
        const SocketAddress& remote_address,    ///< Զ�˵�ַ
        StreamSocketHandler* handler,           ///< Socket�ϵĴ�����
        size_t max_packet_size,                 ///< Socket���������������ݰ��Ĵ�С
        const EndPointOptions& options = EndPointOptions()
    );

    /// @retval >0 socket id
    /// @retval <0 -errno
    int64_t AsyncDatagramBind(
        const SocketAddress& address,   ///< Ҫ�󶨵ı��ص�ַ
        DatagramSocketHandler* handler, ///< Socket�ϵĴ�����
        size_t max_packet_size,         ///< Socket���������������ݰ��Ĵ�С
        const EndPointOptions& options = EndPointOptions()
    );

    /// @brief �ر�һ��Socket
    /// @param endpoint  Socket �Ķ˵��
    /// @param immediate Ĭ��Ϊ�����رգ���Ϊ����ȴ�֮ǰ�����ݰ������ر�
    /// @retval 0 �ɹ�������ֵʧ��
    int CloseEndPoint(EndPoint& endpoint, bool immediate = true);

    /// @brief ����һ����ʽ�����ϵİ�
    /// @param endpoint  Socket �Ķ˵��
    /// @param packet   ָ�򱻷��Ͱ���ָ�룬�����ڴ���netframe��������
    /// @param urgent   �Ƿ��ǽ��������ݰ�
    /// @retval 0 �ɹ�������ֵʧ��
    WARN_UNUSED_RESULT int SendPacket(const StreamEndPoint& endpoint,
                                      Packet* packet,
                                      bool urgent = false);

    /// @brief ����һ����ʽ�����ϵİ�
    /// @param endpoint  Socket �Ķ˵��
    /// @param data      Ҫ���͵����ݵĻ������������������ݽ������������к���
    /// @param size      Ҫ���͵����ݵĳ���
    /// @param urgent    �Ƿ��ǽ��������ݰ�
    /// @retval 0 �ɹ�������ֵʧ��
    int SendPacket(const StreamEndPoint& endpoint, const void* data, size_t size,
                   bool urgent = false);

    /// @brief ����UDP���ݰ�
    /// @param endpoint  Socket �Ķ˵��
    /// @param address Զ�˵ĵ�ַ
    /// @param packet ���Ͱ���ָ�룬�����ڴ���netframe��������
    /// @retval 0 �ɹ�
    WARN_UNUSED_RESULT int SendPacket(const DatagramEndPoint& endpoint,
                                      const SocketAddress& address,
                                      Packet* packet);

    /// @brief ����UDP���ݰ�
    /// @param endpoint Socket �Ķ˵��
    /// @param address  Զ�˵ĵ�ַ
    /// @param data     Ҫ���͵����ݵĻ������������������ݽ������������к���
    /// @param size     Ҫ���͵����ݵĳ���
    /// @retval 0 �ɹ�
    int SendPacket(const DatagramEndPoint& endpoint, const SocketAddress& address,
                   const void* data, size_t size);

    /// @brief �ȴ����е����ݰ��������
    /// @param timeout ָ��ʱ�䣬��λ������
    /// @retval false �ȴ�ָ��ʱ������δ����
    /// @retval true �ѷ��꣬��������
    bool WaitForSendingComplete(int timeout = 10000);

    /// @brief �õ���ǰ���ͻ������Ĵ�С
    /// @retval size ��ǰ���ͻ������ĳ���
    size_t GetCurrentSendBufferedLength() const;

    /// @brief �õ���ǰ���ջ������Ĵ�С
    /// @retval size ��ǰ���ջ������ĳ���
    size_t GetCurrentReceiveBufferedLength() const;

private:
    ///< @param local_address ���ص�ַ��NULL ��ʾ��ָ��
    ///< @param remote_address Զ�˵�ַ
    ///< @param handler Socket�ϵĴ�����
    ///< @param max_packet_size Socket���������������ݰ���
    ///< ��С���������������ޣ�
    ///< OnClose �� error_code Ϊ EMSGSSIZE
    int64_t AsyncConnect(
        const SocketAddress* local_address,
        const SocketAddress& remote_address,
        StreamSocketHandler* handler,
        size_t max_packet_size,
        const EndPointOptions& options = EndPointOptions()
    );
    WorkThread* GetWorkThreadOfFd(int fd);

    /// add a new command to system
    bool AddCommandEvent(const CommandEvent& event);

    static int CloseFd(int fd);

    void IncreaseSendBufferedLength(size_t length);
    void DecreaseSendBufferedLength(size_t length);
    size_t GetMaxSendBufferedLength() const;

    void IncreaseSendBufferedPacket();
    void DecreaseSendBufferedPacket();
    size_t GetSendBufferedPacketNumber();

    void IncreaseReceiveBufferedLength(size_t length);
    void DecreaseReceiveBufferedLength(size_t length);

    /// ��sock fd�õ��µ�SocketId
    static SocketId GenerateSocketId(int32_t sock_fd);

private:
    static uint32_t m_SequenceId;
    size_t m_MaxSendBufferedMemorySize;
    size_t m_CurrentSendBufferedMemorySize;
    size_t m_SendBufferedPacketNumber;
    size_t m_CurrentReceiveBufferedMemorySize;
    std::vector<WorkThread*> m_WorkThreads;
};

} // namespace netframe
} // namespace common

#endif // COMMON_NETFRAME_NETFRAME_H
