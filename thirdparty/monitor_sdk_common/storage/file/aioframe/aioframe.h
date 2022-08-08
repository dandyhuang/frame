// Copyright (c) 2015, Vivo Inc. All rights reserved.
// Description: Encapsulate sync and async Read/Write inferface for native AIO.

#ifndef  COMMON_FILE_AIOFRAME_AIOFRAME_H
#define  COMMON_FILE_AIOFRAME_AIOFRAME_H

#ifndef WIN32
#include <libaio.h>
#include <stdint.h>
#endif

#include "thirdparty/monitor_sdk_common/base/closure2.h"
#include "thirdparty/monitor_sdk_common/base/deprecate.h"
#include "thirdparty/monitor_sdk_common/base/scoped_ptr.h"
#include "thirdparty/monitor_sdk_common/base/stdint.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/event.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/spinlock.h"

namespace common {

namespace aioframe_internal {
class WorkThread;
}

class AIOFrame {
public:
    typedef int32_t StatusCode;

public:
    AIOFrame();
    ~AIOFrame();

    // Cleanup internal threads and status.
    // AIOFrame destructor will Cleanup automatically.
    // Could be explicit called to release resources before AIOFrame destructor.
    void Cleanup();

    // Return true for success to submit AIO request.
    // Return false for errors and error_code for reason.
    // Real operation data size and error code are parameters in callback.
    bool AsyncRead(int fd,
                   void* buffer,
                   int64_t size,
                   int64_t start_position,
                   Closure<void, int64_t, uint32_t>* callback,
                   StatusCode* error_code = NULL);

    // Return true for success to submit AIO request.
    // Return false for errors and error_code for reason.
    // Real operation data size and error code are parameters in callback.
    bool AsyncWrite(int fd,
                    const void* buffer,
                    int64_t size,
                    int64_t start_position,
                    Closure<void, int64_t, uint32_t>* callback,
                    StatusCode* error_code = NULL);

    // Simuate sync read by async read. Return the read bytes, or less than 0 for error.
    int64_t Read(int fd,
                 void* buffer,
                 int64_t size,
                 int64_t start_position,
                 StatusCode* error_code = NULL);

    // Simuate sync write by async write. Return the write bytes, or less than 0 for error.
    int64_t Write(int fd,
                  const void* buffer,
                  int64_t size,
                  int64_t start_position,
                  StatusCode* error_code = NULL);

private:
    void ReadCallback(
        AutoResetEvent* sync_event,
        int64_t* read_size,
        StatusCode* read_code,
        int64_t size,
        uint32_t error_code);

    void WriteCallback(
        AutoResetEvent* sync_event,
        int64_t* write_size,
        StatusCode* read_code,
        int64_t size,
        uint32_t error_code);

    Spinlock m_lock;
    scoped_ptr<aioframe_internal::WorkThread>  m_worker;
};

} // namespace common

namespace aioframe {
DEPRECATED_BY(::common::AIOFrame)
typedef ::common::AIOFrame AIOFrame;

DEPRECATED_BY(::common::AIOFrame::StatusCode)
typedef ::common::AIOFrame::StatusCode StatusCode;
} // namespace aioframe

#endif  // COMMON_FILE_AIOFRAME_AIOFRAME_H
