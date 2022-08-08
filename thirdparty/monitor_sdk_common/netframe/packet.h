// Copyright (c) 2010.
// All rights reserved.
//
//

#ifndef COMMON_NETFRAME_PACKET_H
#define COMMON_NETFRAME_PACKET_H

#include <stdlib.h>
#include <string.h>
#include "thirdparty/monitor_sdk_common/system/memory/mempool.h"
#include "thirdparty/monitor_sdk_common/system/net/socket.h"

namespace common {
namespace netframe {

class Packet
{
public:
    Packet():
        m_OwnBuffer(false),
        m_Content(NULL),
        m_Length(0)
    {
    }

    virtual ~Packet()
    {
        DeleteContent();
    }

    /// @brief ����packet���ݣ�packet�����ڴ棬������������
    void  SetContent(const void* content, size_t length)
    {
        DeleteContent();
        m_Content = MemPool_Allocate(length);
        ::memcpy(m_Content, content, length);
        m_Length = length;
        m_OwnBuffer = true;
    }

    /// @brief ����packet����ָ�룬packet���������ڴ�
    void  SetContentPtr(const void* content, size_t length)
    {
        DeleteContent();
        m_Content = const_cast<void*>(content);
        m_Length = length;
        m_OwnBuffer = false;
    }

    void  DeleteContent()
    {
        if (m_Content != NULL)
        {
            if (m_OwnBuffer)
                MemPool_Free(m_Content);
            m_Content = NULL;
        }
        m_OwnBuffer = false;
        m_Length = 0;
    }

    unsigned char* Content()
    {
        if (m_Content != NULL)
        {
            return (unsigned char*)(m_Content);
        }
        return NULL;
    }

    const unsigned char* Content() const
    {
        if (m_Content != NULL)
        {
            return (const unsigned char*)(m_Content);
        }
        return NULL;
    }

    size_t Length() const
    {
        return m_Length;
    }

    void SetLocalAddress(const SocketAddress& address)
    {
        m_LocalAddress = address;
    }

    void GetLocalAddress(SocketAddress& address) const
    {
        address = m_LocalAddress;
    }

    const SocketAddressStorage& GetLocalAddress() const
    {
        return m_LocalAddress;
    }

    void SetRemoteAddress(const SocketAddress& address)
    {
        m_RemoteAddress = address;
    }

    void GetRemoteAddress(SocketAddress& address) const
    {
        address = m_RemoteAddress;
    }

    const SocketAddressStorage& GetRemoteAddress() const
    {
        return m_RemoteAddress;
    }

private:
    Packet(const Packet&);
    Packet& operator=(const Packet&);
private:
    bool m_OwnBuffer; ///< �Ƿ���л�����
    void* m_Content;  ///< ����������ݵ�MemUnit
    size_t m_Length;  ///< ���ݰ����ܳ���
    SocketAddressStorage m_LocalAddress;
    SocketAddressStorage m_RemoteAddress;
};

} // namespace netframe
} // namespace common

#endif // COMMON_NETFRAME_PACKET_H

