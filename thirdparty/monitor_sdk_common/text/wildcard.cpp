// Copyright (c) 2010, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#include "thirdparty/monitor_sdk_common/text/wildcard.h"

#include <fnmatch.h>

namespace common {

bool Wildcard::Match(const char* pattern, const char* string, int flags)
{
    // fnmatch was defined by ISO/IEC 9945-2: 1993 (POSIX.2)
    return ::fnmatch(pattern, string, flags) == 0;
}

} // namespace common
