// Copyright (c) 2010.
// All rights reserved.
//
//

#ifndef COMMON_NETFRAME_EVENT_POLLER_H
#define COMMON_NETFRAME_EVENT_POLLER_H

namespace common {
namespace netframe {

enum IoEventMask
{
    IoEvent_Connected = 1,
    IoEvent_Readable = 2,
    IoEvent_Writeable = 4,
    IoEvent_Acceptable = 8,
    IoEvent_Closed = 16
};

struct IoEvent
{
    int Fd;
    unsigned int Mask;
    int ErrorCode;
};

/// abstract event poller interface
class EventPoller
{
public:
    class EventHandler
    {
    public:
        virtual ~EventHandler() {}

        /// @retval false fatal error, want quit
        virtual bool HandleIoEvent(const IoEvent& event) = 0;

        /// @retval false fatal error, want quit
        virtual bool HandleInterrupt() = 0;
    };
public:
    virtual ~EventPoller() {}

    /// @brief 在一个Fd上请求事件
    /// @param fd Fd的文件描述符
    /// @param event_mask 请求的事件掩码
    /// @return 成功或者失败
    virtual bool RequestEvent(int fd, unsigned int event_mask) = 0;

    /// @brief 在一个Socket上重新请求事件
    /// @param fd Fd的文件描述符
    /// @param event_mask 请求的事件掩码
    /// @return 成功或者失败
    virtual bool RerequestEvent(int fd, unsigned int event_mask) = 0;

    /// @brief 清除一个Fd上的所有事件请求
    /// @param fd 被清除的Fd
    virtual bool ClearEventRequest(int fd) = 0;

    /// @brief 等待事件的触发，该函数诸塞
    /// @param events 已经触发了的事件列表
    /// @retval false 出粗
    virtual bool PollEvents(EventHandler* event_handler) = 0;

    /// 打断等待
    virtual bool Interrupt() = 0;
};

} // end namespace netframe
} // namespace common

#endif // COMMON_NETFRAME_EVENT_POLLER_H
