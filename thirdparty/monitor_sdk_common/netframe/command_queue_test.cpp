// Copyright (c) 2012.
// All rights reserved.
//
//
// Created: 05/29/12

#include "thirdparty/monitor_sdk_common/netframe/command_queue.h"

#include "thirdparty/monitor_sdk_common/netframe/command_event.h"
#include "thirdparty/monitor_sdk_common/netframe/packet.h"
#include "thirdparty/gtest/gtest.h"

namespace common{

TEST(CommandQueue, Enqueue)
{
    netframe::CommandQueue queue(5);
    EXPECT_EQ(0U, queue.Size());
    netframe::SocketId id(10);
    netframe::Packet* packet = new netframe::Packet();
    netframe::CommandEvent event(netframe::CommandEvent_AddSocket, id, packet);
    for (int i = 0; i < 5; ++i) {
        EXPECT_TRUE(queue.Enqueue(event, false));
    }
    EXPECT_FALSE(queue.Enqueue(event, false));
    EXPECT_TRUE(queue.Enqueue(event, true));
    netframe::CommandEvent event2;
    EXPECT_TRUE(queue.GetFront(&event2));
    EXPECT_EQ(netframe::CommandEvent_AddSocket, event2.Type);
    EXPECT_EQ(id, event2.SockId);
    EXPECT_EQ(packet, event2.Cookie.CastTo<netframe::Packet*>());
    // as we push an urgent event
    for (int i = 0; i < 6; ++i) {
        EXPECT_TRUE(queue.PopFront());
    }
    EXPECT_FALSE(queue.PopFront());
    EXPECT_FALSE(queue.HasMore());
    delete packet;
}

TEST(CommandQueue, EnqueueUrgent)
{
    netframe::CommandQueue queue(5);
    EXPECT_EQ(0U, queue.Size());
    netframe::SocketId id1(10), id2(20);
    netframe::Packet* packet = new netframe::Packet();
    netframe::CommandEvent event_normal(netframe::CommandEvent_AddSocket, id1);
    netframe::CommandEvent event_urgent(netframe::CommandEvent_SendUrgentPacket,
                                        id2,
                                        packet);
    for (int i = 0; i < 3; ++i) {
        EXPECT_TRUE(queue.Enqueue(event_normal, false));
    }
    queue.EnqueueUrgent(event_urgent);
    EXPECT_TRUE(queue.PopFront());
    netframe::CommandEvent event_result;
    EXPECT_TRUE(queue.GetFront(&event_result));
    EXPECT_EQ(event_urgent.Type, event_result.Type);
    EXPECT_EQ(event_urgent.SockId, event_result.SockId);
    EXPECT_EQ(packet, event_result.Cookie.CastTo<netframe::Packet*>());
}

} // namespace common
