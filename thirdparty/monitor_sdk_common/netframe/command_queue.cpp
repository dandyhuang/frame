// Copyright (c) 2010.
// All rights reserved.
//
//

#include "thirdparty/monitor_sdk_common/netframe/command_queue.h"

#include "thirdparty/monitor_sdk_common/netframe/command_event.h"
#include "thirdparty/monitor_sdk_common/netframe/packet.h"

namespace common {
namespace netframe {

CommandQueue::CommandQueue(size_t max_length):
    m_MaxLength(max_length)
{
}

CommandQueue::~CommandQueue()
{
    while (!m_Queue.empty())
    {
        CommandEvent event = m_Queue.front();
        // ���������¼��������ݰ�
        if (event.Type == CommandEvent_SendPacket ||
            event.Type == CommandEvent_SendUrgentPacket)
        {
            Packet* packet = event.Cookie.CastTo<Packet*>();
            delete packet;
        }
        m_Queue.pop_front();
    }
}

bool CommandQueue::GetFront(CommandEvent* event)
{
    if (!m_Queue.empty())
    {
        *event = m_Queue.front();
        return true;
    }
    return false;
}

bool CommandQueue::PopFront()
{
    if (!m_Queue.empty())
    {
        m_Queue.pop_front();
        return true;
    }
    return false;
}

bool CommandQueue::Enqueue(const CommandEvent& event, bool force)
{
    // �������г���,����
    if (!force && (m_Queue.size() + 1) > m_MaxLength)
        return false;
    m_Queue.push_back(event);
    return true;
}

void CommandQueue::EnqueueUrgent(const CommandEvent& event)
{
    if (!m_Queue.empty())
    {
        // ���뵽�ڶ�����һ���������ڴ���
        CommandEvent first = m_Queue.front();
        m_Queue.pop_front();
        m_Queue.push_front(event);
        m_Queue.push_front(first);
    }
    else
    {
        m_Queue.push_front(event);
    }
}

} // namespace netframe
} // namespace common
