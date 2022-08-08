// Copyright (c) 2010, Vivo Inc.
// All rights reserved.
//

#ifndef COMMON_BASE_STDEXT_HASH_MAP_H
#define COMMON_BASE_STDEXT_HASH_MAP_H

#ifdef __GNUC__

#ifdef __DEPRECATED
#undef __DEPRECATED
#define __DEPRECATED_DEFINED
#endif

#include <ext/hash_map>

#ifdef __DEPRECATED_DEFINED
#define __DEPRECATED
#undef __DEPRECATED_DEFINED
#endif

#include "thirdparty/monitor_sdk_common/base/stdext/hash_function.h"

namespace stdext
{
using __gnu_cxx::hash_map;
using __gnu_cxx::hash_multimap;
} // namespace stdext

#elif defined _MSC_VER

#include <hash_map>

#endif // __GNUC__

namespace ext = stdext;

#endif // COMMON_BASE_STDEXT_HASH_MAP_H
