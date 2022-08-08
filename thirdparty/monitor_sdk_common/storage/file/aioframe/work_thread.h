// Copyright (c) 2015, Vivo Inc. All rights reserved.
// Description: workthread to wrap all AIO operations.

#ifndef  COMMON_FILE_AIOFRAME_WORK_THREAD_H
#define  COMMON_FILE_AIOFRAME_WORK_THREAD_H

#include "thirdparty/monitor_sdk_common/base/closure2.h"
#include "thirdparty/monitor_sdk_common/base/object_pool.h"
#include "thirdparty/monitor_sdk_common/base/scoped_ptr.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/base_thread.h"

#include "thirdparty/monitor_sdk_common/storage/file/aioframe/aioframe.h"

#ifndef WIN32

namespace common {
namespace aioframe_internal {

// Reduce numbers to support more concurrent process.
static const int kMaxAIORequest = 128;
static const int kMaxBatchNumber = 32;

class WorkThread: public BaseThread {
public:
    WorkThread();
    virtual ~WorkThread();

    virtual void Entry();

    bool Start();

    bool Stop();

    bool SubmitRequest(
        io_iocb_cmd_t command,
        int fd,
        void* buffer,
        int64_t size,
        int64_t start_position,
        Closure<void, int64_t, uint32_t>* callback,
        AIOFrame::StatusCode* error_code);

private:
    void PrepareRequest(
        struct iocb* cb,
        int fd,
        io_iocb_cmd_t command,
        void* buffer,
        int64_t size,
        int64_t start_position,
        Closure<void, int64_t, uint32_t>* callback) {
        memset(cb, 0, sizeof(*cb));
        cb->aio_fildes = fd;
        cb->aio_lio_opcode = command;
        cb->aio_reqprio = 0;
        cb->u.c.buf = buffer;
        cb->u.c.nbytes = size;
        cb->u.c.offset = start_position;
        cb->data = callback;
    }

    bool DoAIOSubmit(int fd,
                    io_iocb_cmd_t command,
                    void* buffer,
                    int64_t size,
                    int64_t start_position,
                    Closure<void, int64_t, uint32_t>* callback,
                    AIOFrame::StatusCode* error_code);

    void ProcessCompletedRequests(int request_number);

private:
    void SetErrorCode(AIOFrame::StatusCode* ptr_error_code,
                      const AIOFrame::StatusCode error_code) {
        if (ptr_error_code) {
            *ptr_error_code = error_code;
        }
    }

    io_context_t m_context;
    io_event m_events[kMaxBatchNumber];

    scoped_ptr<FixedObjectPool<iocb, kMaxAIORequest> > m_iocb_pool;
};

} // namespace aioframe_internal
} // namespace common

#endif

#endif  // COMMON_FILE_AIOFRAME_WORK_THREAD_H
