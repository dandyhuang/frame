// Copyright (c) 2010.
// All rights reserved.
//
//

#include "thirdparty/monitor_sdk_common/netframe/listen_socket_context.h"

#include "thirdparty/monitor_sdk_common/netframe/command_event.h"
#include "thirdparty/monitor_sdk_common/netframe/event_poller.h"
#include "thirdparty/monitor_sdk_common/netframe/netframe.h"
#include "thirdparty/monitor_sdk_common/netframe/stream_socket_context.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/this_thread.h"
#include "thirdparty/monitor_sdk_common/system/net/socket.h"

namespace common {
namespace netframe {

unsigned int ListenSocketContext::GetWantedEventMask() const
{
    return IoEvent_Acceptable;
}

int ListenSocketContext::HandleIoEvent(const IoEvent& event)
{
    // ����ʲôʵ�ʶ��� acceptable �¼�
    return HandleAccept();
}

int ListenSocketContext::HandleCommandEvent(const CommandEvent& event)
{
    switch (event.Type)
    {
    case CommandEvent_AddSocket: // ����ʵ�ʴ���
        return 1;
    case CommandEvent_CloseSocket:
        return -1;
    default:
        break;
    }
    return 0;
}

// �ڼ����˿��Ͻ����µ����������¼�
int ListenSocketContext::HandleAccept()
{
    SocketAddressStorage address;

    for (;;)
    {
        int new_fd = -1;

        // ��������
        socklen_t address_length = address.Capacity();
        do
        {
            new_fd = static_cast<int>(::accept(GetFd(), address.Address(), &address_length));
        } while ((new_fd < 0) && (ThisThread::GetLastErrorCode() == EINTR));

        if (new_fd >= 0)
        {
            StreamSocket socket;
            socket.Attach(new_fd);

            address.SetLength(address_length);
            ListenSocketHandler* listen_handler = GetEventHandler();
            SocketId id = NetFrame::GenerateSocketId(new_fd);
            StreamSocketHandler* new_handler = listen_handler->OnAccepted(id);

            if (new_handler != NULL)
            {
                // ���÷�����,����Socket�������д�¼�
                Socket::SetCloexec(new_fd, true);
                socket.SetBlocking(false);
                socket.SetTcpNoDelay();
                socket.SetKeepAlive();
                socket.SetSendBufferSize(m_EndPointOptions.SendBufferSize());
                socket.SetReceiveBufferSize(m_EndPointOptions.ReceiveBufferSize());
                int priority = m_EndPointOptions.Priority();
                if (priority > 0)
                {
#ifndef _WIN32
                    socket.SetOption(SOL_IP, IP_TOS, static_cast<uint8_t>(priority));
                    socket.SetOption(SOL_SOCKET, SO_PRIORITY, priority);
#endif
                }

                new_handler->SetEndPointId(id);
                new_handler->SetRemoteAddress(address);
                new_handler->OnConnected();
                StreamSocketContext* new_socket_context = new StreamSocketContext(
                    m_NetFrame, &m_LocalAddress, &address, id, new_handler,
                    m_MaxPacketLength, m_EndPointOptions,
                    true // connected
                );

                socket.Detach();
                m_NetFrame->AddCommandEvent(
                    CommandEvent(CommandEvent_AddSocket, id,
                        static_cast<SocketContext*>(new_socket_context))
                );
            }
        }
        else
        {
            // TODO(phongchen): handle fatal and nonfatal errors here
            break;
        }
    }
    return 1;
}

} // namespace netframe
} // namespace common
