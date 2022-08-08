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

    /// @brief ��һ��Fd�������¼�
    /// @param fd Fd���ļ�������
    /// @param event_mask ������¼�����
    /// @return �ɹ�����ʧ��
    virtual bool RequestEvent(int fd, unsigned int event_mask) = 0;

    /// @brief ��һ��Socket�����������¼�
    /// @param fd Fd���ļ�������
    /// @param event_mask ������¼�����
    /// @return �ɹ�����ʧ��
    virtual bool RerequestEvent(int fd, unsigned int event_mask) = 0;

    /// @brief ���һ��Fd�ϵ������¼�����
    /// @param fd �������Fd
    virtual bool ClearEventRequest(int fd) = 0;

    /// @brief �ȴ��¼��Ĵ������ú�������
    /// @param events �Ѿ������˵��¼��б�
    /// @retval false ����
    virtual bool PollEvents(EventHandler* event_handler) = 0;

    /// ��ϵȴ�
    virtual bool Interrupt() = 0;
};

} // end namespace netframe
} // namespace common

#endif // COMMON_NETFRAME_EVENT_POLLER_H
