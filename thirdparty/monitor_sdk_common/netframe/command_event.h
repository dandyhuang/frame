// Copyright (c) 2010.
// All rights reserved.
//
//

#ifndef COMMON_NETFRAME_COMMAND_EVENT_H
#define COMMON_NETFRAME_COMMAND_EVENT_H

#include "thirdparty/monitor_sdk_common/base/any_ptr.h"
#include "thirdparty/monitor_sdk_common/base/stdint.h"
#include "thirdparty/monitor_sdk_common/base/uncopyable.h"

namespace common{
namespace netframe {

///< ����ÿ��������Ӧ��socket��ʶ
union SocketId
{
    explicit SocketId(int64_t i = -1)
    {
        Id = i;
    }
    struct
    {
        int32_t SockFd;     ///< 32λsock fd
        int32_t SequenceId; ///< 32λ���к�
    };
    bool operator == (const SocketId& sock_id) const
    {
        return Id == sock_id.Id;
    }
    int GetFd() const
    {
        return SockFd;
    }
    int64_t GetId() const
    {
        return Id;
    }
    int64_t Id;
};

enum CommandEventType
{
    ///< ռλ��
    CommandEvent_Null = 0,
    ///< ���һ��Socket�������¼���CookieΪSocketContextָ��
    CommandEvent_AddSocket,
    ///< �ر�һ�����ӣ��ȴ�֮ǰ�����ݰ�����
    CommandEvent_CloseSocket,
    ///< �����ر�һ��socket
    CommandEvent_CloseSocket_Now,
    ///< Socket���Ͱ������¼���Cookie��ΪPacketָ��
    CommandEvent_SendPacket,
    ///< Socket���ͽ����������¼���Cookie��ΪPacketָ��
    CommandEvent_SendUrgentPacket,
};

struct CommandEvent
{
public:
    CommandEventType Type;  ///< Event������
    SocketId SockId;        ///< �¼�Ҫ������Socket Id
    AnyPtr Cookie;          ///< �¼��ĸ�����Ϣ

    CommandEvent() : Type(CommandEvent_Null), SockId(-1), Cookie()
    {
    }

    template <typename T>
    CommandEvent(CommandEventType type, SocketId id, T* cookie) :
        Type(type), SockId(id), Cookie(cookie)
    {
    }

    CommandEvent(CommandEventType type, SocketId id) :
        Type(type), SockId(id)
    {
    }

    int32_t GetFd() const
    {
        return SockId.SockFd;
    }
};

} // namespace netframe
} // namespace common
#endif // COMMON_NETFRAME_COMMAND_EVENT_H
