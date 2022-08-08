//
// Created by 王锦涛 on 2020-2-19.
//

#ifndef COMMON_LIBTRACE_TRACE_THRIFT_H
#define COMMON_LIBTRACE_TRACE_THRIFT_H

#include <memory>
#include <iostream>
#include <string>
#include <vector>

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/protocol/TCompactProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

#include "thirdparty/monitor_sdk_common/libtrace/utils/flume_constants.h"
#include "thirdparty/monitor_sdk_common/libtrace/utils/flume_types.h"
#include "thirdparty/monitor_sdk_common/libtrace/utils/ThriftSourceProtocol.h"
#include "thirdparty/monitor_sdk_common/libtrace/utils/span.pb.h"
#include "thirdparty/monitor_sdk_common/libtrace/utils/metric.pb.h"

namespace common {

class TraceThrift
{
public:
    TraceThrift();
    ~TraceThrift();

    void SendSpan();

    void SendMetric();

private:
    void SendEvents();
};

} //namespace common
#endif //COMMON_LIBTRACE_TRACE_THRIFT_H