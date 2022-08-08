// Copyright 2011, Vivo Inc.

#include "thirdparty/monitor_sdk_common/system/debug/print_trace.h"

namespace common {

/* A dummy function to make the backtrace more interesting. */
void dummy_function()
{
    PrintStackTrace();
}
} // namespace common

int main()
{
    common::dummy_function();
    return 0;
}

