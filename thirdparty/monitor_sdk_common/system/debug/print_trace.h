// Copyright 2011, Vivo Inc.

#ifndef COMMON_SYSTEM_DEBUG_PRINT_TRACE_H
#define COMMON_SYSTEM_DEBUG_PRINT_TRACE_H

#ifdef __GNUC__

#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "thirdparty/monitor_sdk_common/base/cxx_demangle.h"

namespace common {

// Obtain a backtrace and print it to stderr. It's used to assist debugging.
// Add -rdynamic option when linking to see the function names.

inline void PrintStackTrace()
{
    void *array[16];
    size_t frames = backtrace(array, 16);
    char** symbols = backtrace_symbols(array, frames);

    fprintf(stderr, "Obtained %zd stack frames.\n", frames);
    for (size_t i = 0; i < frames; i++)
    {
        std::string stack = symbols[i];
        size_t pos1 = stack.find('(');
        size_t pos2 = stack.find('+', pos1);
        if (pos1 == std::string::npos || pos2 == std::string::npos)
        {
            fprintf(stderr, "%s\n", stack.c_str());
        }
        else
        {
            stack = stack.substr(0, pos1 + 1) +
                CxxDemangle(stack.substr(pos1 + 1, pos2 - pos1 - 1).c_str()) +
                stack.substr(pos2);
            fprintf(stderr, "%s\n", stack.c_str());
        }
    }
    free(symbols);
}

} // namespace common

#endif // __GNUC

#endif // COMMON_SYSTEM_DEBUG_PRINT_TRACE_H
