// Copyright (c) 2010.
// All rights reserved.
//
//

#ifndef COMMON_NETFRAME_SOCKET_HANDLER_H
#define COMMON_NETFRAME_SOCKET_HANDLER_H

#include <string>
#include <vector>
#include "thirdparty/monitor_sdk_common/netframe/netframe.h"
#include "thirdparty/monitor_sdk_common/netframe/packet.h"

namespace common {
namespace netframe {

/// �¼� Handler ����
class SocketHandler
{
protected:
    explicit SocketHandler(NetFrame& netframe): m_NetFrame(netframe),
        m_CommandQueueLength(0)
    {
    }
    NetFrame::EndPoint m_EndPoint;

public:
    virtual ~SocketHandler() {}

    void SetEndPointId(SocketId id)
    {
        m_EndPoint.SetEndPointId(id);
    }
    virtual NetFrame::EndPoint& GetEndPoint()
    {
        return m_EndPoint;
    }
    NetFrame& GetNetFrame() const
    {
        return m_NetFrame;
    }

    size_t GetCommandQueueLength()
    {
        return m_CommandQueueLength;
    }
    void SetCommandQueueLength(size_t length)
    {
        m_CommandQueueLength = length;
    }

    /// ���ӹر�ǰִ�еĺ���
    /// @param error_code ʧ��ԭ��ϵͳ������
    /// error_code �Ŀ���ֵ:
    /// EMSGSIZE ��Ϣ̫����������max_packet_size
    /// ����Ϊ����ϵͳ������
    virtual void OnClose(int error_code) = 0;

    /// ����ʧ��ʱ����Ϣ֪ͨ
    /// @param packet ʧ�ܵİ�
    /// @param error_code ʧ��ԭ��ϵͳ������
    /// @return �Ƿ�ɾ����
    /// @retval true ����Ҫɾ��
    /// @retval false ���ѱ��û�����netframe��Ҫɾ����
    /// error_code �Ŀ���ֵ
    /// EMSGSIZE ��Ϣ̫��
    /// ENOBUFS ���ﱾ���Ӷ��г�������
    /// ENOMEN ���۵Ĵ�����Ϣ�ܳ��Գ�������
    /// ESHUTDOWN �û��Ѿ����� Close
    /// ����Ϊ����ϵͳ������
    virtual bool OnSendingFailed(Packet* packet, int error_code)
    {
        return true;
    }

private:
    NetFrame& m_NetFrame;
    size_t m_CommandQueueLength;
};

class StreamSocketHandler : public SocketHandler
{
public:
    explicit StreamSocketHandler(NetFrame& netframe):
        SocketHandler(netframe)
    {
    }

    /// ���ӳɹ�ʱִ�еĺ����������������Ӻͱ�������ʱ�������
    virtual void OnConnected() = 0;

    /// �������յ�������̽����Ĵ�С������0��ʾδ֪������0����ʶ����İ��Ĵ�С
    /// С��0��ʾ̽�ⷢ��������Э�����
    /// ��������������޶����Ȼ�̽����̷�������ʱ�����ӽ����ر�
    virtual int DetectPacketSize(const void* data, size_t length) = 0;

    NetFrame::StreamEndPoint& GetEndPoint() /// override
    {
        return static_cast<NetFrame::StreamEndPoint&>(m_EndPoint);
    }

    /// ���ܵ����ݰ�֮��ִ�еĺ���;
    virtual void OnReceived(const Packet& packet) = 0;

    /// ���ݰ��������֮��ִ�еĺ���;
    /// @param packet �ѱ����͵İ�
    /// @return �Ƿ���netframeɾ����
    /// @retval true ����Ҫɾ��
    /// @retval false ���ѱ��û�����netframe��Ҫɾ����
    virtual bool OnSent(Packet* packet)
    {
        return true;
    }

    /// Set/Get remote network address
    void SetRemoteAddress(const SocketAddress& address) {
        m_remote_address = address;
    }

    const SocketAddress& GetRemoteAddress() const {
        return m_remote_address;
    }

private:
    SocketAddressStorage m_remote_address;
};

class DatagramSocketHandler : public SocketHandler
{
public:
    explicit DatagramSocketHandler(NetFrame& netframe):
        SocketHandler(netframe)
    {
    }

    virtual NetFrame::DatagramEndPoint& GetEndPoint() /// override
    {
        return static_cast<NetFrame::DatagramEndPoint&>(m_EndPoint);
    }

    /// ���ܵ����ݰ�֮��ִ�еĺ���;
    virtual void OnReceived(const Packet& packet) = 0;

    /// ���ݰ��������֮��ִ�еĺ���;
    virtual bool OnSent(Packet* packet)
    {
        return true;
    }
};

class ListenSocketHandler : public SocketHandler
{
public:
    explicit ListenSocketHandler(NetFrame& netframe):
        SocketHandler(netframe)
    {
    }

    /// ����Listen�˿�;�������ӳɹ�ʱִ�еĺ���.
    /// �ú�����Ҫ�ں������ڽ����������ϴ��������½�
    virtual StreamSocketHandler* OnAccepted(SocketId id) = 0;

    NetFrame::ListenEndPoint& GetEndPoint() /// override
    {
        return static_cast<NetFrame::ListenEndPoint&>(m_EndPoint);
    }
};

/// ÿ���ı���ʶ��Ϊһ����Ϣ
class LineStreamSocketHandler : public StreamSocketHandler
{
public:
    explicit LineStreamSocketHandler(NetFrame& netframe): StreamSocketHandler(netframe)
    {
    }
    int DetectPacketSize(const void* header, size_t size)
    {
        const char* p = (const char*) memchr(header, '\n', size);
        if (p)
            return p - (const char*)header + 1;
        else
            return 0;
    }
};

} // namespace netframe
} // namespace common

#endif // COMMON_NETFRAME_SOCKET_HANDLER_H
