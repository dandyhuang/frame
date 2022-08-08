// Copyright (c) 2010.
// All rights reserved.
//
//

#ifndef COMMON_NETFRAME_EPOLL_EVENT_POLLER_H
#define COMMON_NETFRAME_EPOLL_EVENT_POLLER_H

#include <sys/epoll.h>
#include <vector>
#include "thirdparty/monitor_sdk_common/base/unique_ptr2.h"
#include "thirdparty/monitor_sdk_common/netframe/event_poller.h"
#include "thirdparty/monitor_sdk_common/netframe/eventfd_interrupter.h"
#include "thirdparty/monitor_sdk_common/netframe/pipe_interrupter.h"

namespace common {
namespace netframe {

/// EventPoller using linux epoll
class EpollEventPoller : public EventPoller
{
public:
    explicit EpollEventPoller(unsigned int max_fds = 0x10000);
    virtual ~EpollEventPoller();

    virtual bool RequestEvent(int fd, unsigned int event_mask);
    virtual bool RerequestEvent(int fd, unsigned int event_mask);
    virtual bool ClearEventRequest(int fd);
    virtual bool PollEvents(EventPoller::EventHandler* event_handler);
    virtual bool Interrupt();
private:
    int m_EpollFd;
    ::std::vector< ::epoll_event> m_EpollEvents;  ///< epoll event array
    unique_ptr<Interrupter> m_Interrupter;
};

} // namespace netframe
} // namespace common

#endif // COMMON_NETFRAME_EPOLL_EVENT_POLLER_H
