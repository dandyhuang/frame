// Copyright (c) 2010.
// All rights reserved.
//
//

#ifndef COMMON_NETFRAME_STREAM_SOCKET_CONTEXT_H
#define COMMON_NETFRAME_STREAM_SOCKET_CONTEXT_H

#include "thirdparty/monitor_sdk_common/netframe/socket_context.h"

namespace common {
namespace netframe {

class StreamSocketContext : public SocketContext
{
public:
    StreamSocketContext(
        NetFrame* netframe,
        const SocketAddress* local_address,
        const SocketAddress* remote_address,
        SocketId sock_id,
        StreamSocketHandler* handler,
        size_t max_packet_size,
        const NetFrame::EndPointOptions& options,
        bool connected
    );
    ~StreamSocketContext();

private:
    virtual int HandleIoEvent(const IoEvent& event); // override
    virtual int HandleCommandEvent(const CommandEvent& event); // override
    virtual unsigned int GetWantedEventMask() const; // override

    /// �ڸ�Socket�ϴ�����¼�
    /// @retval true �ɹ�
    /// @retval false ���ִ���
    bool HandleInput();

    /// �ڸ�Socket�ϴ���д�¼�
    /// @retval true �ɹ�
    /// @retval false ���ִ���
    bool HandleOutput();

    /// ���Է���һ������ֱ���������ߴ���
    /// @param packet Ҫ���͵İ�
    /// @retval >0 ��ȫ������
    /// @retval =0 ����
    /// @retval <0 ���ִ���
    int SendPacket(Packet* packet);

    /// ���Է��ͻ�������ֱ���������ߴ���
    /// @retval >=0 ����ȥ�ĳ���
    /// @retval =0 ����
    /// @retval <0 ���ִ���
    int SendBuffer(const void* buffer, size_t size);

    /// ��ȡSocket���Ѿ����������
    int Receive(void* buffer, size_t buffer_length);

    /// ���ѽ��ܵĻ�������̽����߽磬ʶ��֪ͨ���ϲ�
    /// @return ʶ�𵽵İ��ĸ���
    /// @retval <0 ���ִ���
    int SplitAndIndicatePackets();

    int CheckAndIndicatePacket(char*& buffer, int& left_length);

    void AdjustReceiveBufferSize();
    void ResizeReceiveBuffer(size_t size);
private:
    virtual StreamSocketHandler* GetEventHandler() const // override
    {
        return static_cast<StreamSocketHandler*>(SocketContext::GetEventHandler());
    }
private:
    bool   m_Connected;            ///< �Ƿ�������
    size_t m_SentLength;           ///< �Ѿ����ͳ�ȥ�˵İ��ĳ���
    size_t m_MinBufferLength;      ///< ��С����������
    char*  m_ReceiveBuffer;        ///< ���յ������ݰ�
    size_t m_ReceiveBufferLength;  ///< ��ǰ���ջ������ĳ���
    size_t m_ReceivedLength;       ///< �Ѿ����ܵ��˵����ݳ���
    int    m_DetectedPacketLength; ///< �������֪�İ�����
    Packet m_ReceivePacket;        ///< ��ǰ�����յ������ݰ�
};

} // namespace netframe
} // namespace common

#endif // COMMON_NETFRAME_STREAM_SOCKET_CONTEXT_H
