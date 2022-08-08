// Copyright (c) 2010.
// All rights reserved.
//
//

#ifndef COMMON_NETFRAME_SOCKET_CONTEXT_H
#define COMMON_NETFRAME_SOCKET_CONTEXT_H

#include <deque>
#include <string>
#include <vector>
#include "thirdparty/monitor_sdk_common/base/uncopyable.h"
#include "thirdparty/monitor_sdk_common/netframe/command_queue.h"
#include "thirdparty/monitor_sdk_common/netframe/netframe.h"
#include "thirdparty/monitor_sdk_common/netframe/packet.h"
#include "thirdparty/monitor_sdk_common/netframe/socket_handler.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/mutex.h"
#include "thirdparty/monitor_sdk_common/system/memory/mempool.h"

namespace common {
namespace netframe {

struct IoEvent;
struct CommandEvent;
class  EventPoller;

class SocketContext
{
public:
    SocketContext(
        NetFrame* netframe,
        const SocketAddress* local_address,
        const SocketAddress* remote_address,
        SocketId sock_id,
        size_t  max_packet_size,
        const NetFrame::EndPointOptions& options
    );
    virtual ~SocketContext();

    void SetEventPoller(EventPoller* event_poller)
    {
        m_EventPoller = event_poller;
    }

    /// �������Socket�¼�
    /// @retval >0 �¼��ѱ�����
    /// @retval =0 �¼�δ������
    /// @retval <0 ��������
    int ProcessIoEvent(const IoEvent& event);

    /// @brief ��������
    /// @retval >0 �¼��ѱ�����
    /// @retval =0 �¼�δ������
    /// @retval <0 ��������
    int ProcessCommandEvent(const CommandEvent& event);

    void UpdateEventRequest(); ///< �¼��������֮��Ĳ���

    SocketId GetSockId() const
    {
        return m_SockId;
    }

    int GetFd() const
    {
        return m_SockId.SockFd;
    }

    void Close();           ///< �ر�Socket�������˳�

protected: // ����������ص�
    /// �������Socket�¼�
    /// @param event_mask �������¼�����
    /// @retval >0 �¼��ѱ�����
    /// @retval =0 �¼�δ������
    /// @retval <0 ��������
    virtual int HandleIoEvent(const IoEvent& event) = 0;

    /// @brief ��������
    /// @param event_mask �������¼�����
    /// @retval >0 �¼��ѱ�����
    /// @retval =0 �¼�δ������
    /// @retval <0 ��������
    virtual int HandleCommandEvent(const CommandEvent& event) = 0;

    virtual unsigned int GetWantedEventMask() const;

    void SetEventHandler(SocketHandler* handler)
    {
        m_EventHandler = handler;
    }

    virtual SocketHandler* GetEventHandler() const ///< ��Socket�ϵ��¼�������
    {
        return m_EventHandler;
    }

    void DecreaseSendBufferedLength(size_t size)
    {
        m_NetFrame->DecreaseSendBufferedLength(size);
    }

    void DecreaseSendBufferedPacket()
    {
        m_NetFrame->DecreaseSendBufferedPacket();
    }

    void IncreaseReceiveBufferedLength(size_t size)
    {
        m_NetFrame->IncreaseReceiveBufferedLength(size);
    }

    void DecreaseReceiveBufferedLength(size_t size)
    {
        m_NetFrame->DecreaseReceiveBufferedLength(size);
    }

    virtual void HandleClose();             ///< �ر�Socket���������

    void HandleSendingFailed(Packet* packet, int error_code);
    void ClearCommandQueue();

protected:
    NetFrame* m_NetFrame;
    EventPoller* m_EventPoller;
    SocketHandler* m_EventHandler;
    SocketId m_SockId;                      ///< socket��ʶ
    NetFrame::EndPointOptions m_EndPointOptions;
    size_t m_MaxPacketLength;

    SocketAddressStorage m_LocalAddress;    ///< Socket�ı��ص�ַ
    SocketAddressStorage m_RemoteAddress;   ///< Զ�˵ĵ�ַ

    unsigned int m_RequestedEvent;          ///< �ϴ��������Socket�¼�
    bool m_IsFirstRequested;                ///< �Ƿ��һ������Socket�¼�
    int m_LastError;                        ///< ����ʱ���Ĵ�����

    CommandQueue m_CommandQueue; ///< �������
    COMMON_DECLARE_UNCOPYABLE(SocketContext);
};

} // namespace netframe
} // namespace common

#endif // COMMON_NETFRAME_SOCKET_CONTEXT_H
