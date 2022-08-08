// Copyright (c) 2015, Vivo Inc. All rights reserved.
// Description: WorkThread implementation.

#include <unistd.h>
#include <sys/syscall.h>

#include <cstring>

#include "thirdparty/monitor_sdk_common/storage/file/aioframe/work_thread.h"

#include "thirdparty/glog/logging.h"

#ifndef Win32

namespace common {
namespace aioframe_internal {

WorkThread::WorkThread()
    : m_context(0), m_iocb_pool(new FixedObjectPool<iocb, kMaxAIORequest>()) {
}

WorkThread::~WorkThread() {
}

bool WorkThread::Start() {
    if (m_context != 0)
        return true;

    const int32_t kMaxRetryCount = 3;
    int32_t retry_count = 0;
    while (true) {
        ++retry_count;

        m_context = 0;
        int ret = syscall(SYS_io_setup, kMaxAIORequest, &m_context);
        if (ret == 0) {
            if (retry_count > 1) {
                LOG(INFO) << "SYS_io_setup finally success at retry count " << retry_count;
            }
            break; // success
        }

        char msg[1024] = { 0 };
        LOG(ERROR) << "fail to call SYS_io_setup. " << strerror_r(errno, msg, sizeof(msg))
            << " at retry count " << retry_count;
        // retry to mask errors like EAGAIN
        if ((errno != EAGAIN && errno != ENOMEM) || retry_count >= kMaxRetryCount) {
            return false;
        }
    }
    if (!BaseThread::Start()) {
        LOG(ERROR) << "fail to start this work thread.";
        return false;
    }

    return true;
}

bool WorkThread::Stop() {
    if (m_context == 0) return true;

    SendStopRequest();
    Join();

    syscall(SYS_io_destroy, m_context);
    m_context = 0;

    return true;
}

bool WorkThread::SubmitRequest(
    io_iocb_cmd_t command,
    int fd,
    void* buffer,
    int64_t size,
    int64_t start_position,
    Closure<void, int64_t, uint32_t>* callback,
    AIOFrame::StatusCode* error_code) {

    return DoAIOSubmit(fd, command, buffer, size, start_position, callback, error_code);
}

bool WorkThread::DoAIOSubmit(int fd,
                            io_iocb_cmd_t command,
                            void* buffer,
                            int64_t size,
                            int64_t start_position,
                            Closure<void, int64_t, uint32_t>* callback,
                            AIOFrame::StatusCode* error_code) {
    iocb* cb = m_iocb_pool->Acquire();
    PrepareRequest(cb, fd, command, buffer, size, start_position, callback);

    int ret = syscall(SYS_io_submit, m_context, 1, &cb);
    if (ret >= 0) {
        SetErrorCode(error_code, 0);
        return true;
    }

    switch (errno) {
        case EAGAIN:
        case ENOMEM:
            // Too busy.
            // Should retry? No. Let the AIOFrame application to decide retry policy.
            // Must avoid sleep to blocking possible network thread.
            SetErrorCode(error_code, errno);
            break;
        default:
            SetErrorCode(error_code, errno);
            break;
    }

    return false;
}

void WorkThread::Entry() {
    timespec timeout = {0, 1000000 * 20};
    while (!IsStopRequested()) {
        long n = // NOLINT (runtime/int)
            syscall(SYS_io_getevents, m_context, 1, kMaxBatchNumber, m_events, &timeout);
        if (n > 0)
            ProcessCompletedRequests(n);
    }
}

void WorkThread::ProcessCompletedRequests(int request_number) {
    for (int i = 0; i < request_number; ++i) {
        iocb* cb = static_cast<iocb*>(m_events[i].obj);

        Closure<void, int64_t, uint32_t>* callback =
            static_cast<Closure<void, int64_t, uint32_t>*>(cb->data);
        if (callback != NULL) {
            callback->Run(m_events[i].res, m_events[i].res2);
        }

        m_iocb_pool->Release(cb);
    }  // for
}

} // namespace aioframe_internal
} // namespace common

#endif
