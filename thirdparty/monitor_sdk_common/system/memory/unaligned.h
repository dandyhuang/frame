// Copyright (c) 2012, The COMMON Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#ifndef COMMON_SYSTEM_MEMORY_UNALIGNED_H
#define COMMON_SYSTEM_MEMORY_UNALIGNED_H

#include <stddef.h>

namespace common {

//////////////////////////////////////////////////////////////////////////////
// interface declaration

/// @brief get value from unaligned address
/// @tparam T type to get
/// @param p pointer to get value from
/// @return get result
/// @details usage: uint32_t n = GetUnaligned<uint32_t>(p);
template <typename T>
T GetUnaligned(const void* p);

/// @brief put value into unaligned address
/// @tparam T type to get
/// @tparam U introduce U make T must be given explicitly
/// @param p pointer to get value
/// @param value value to put into p
/// @details usage: PutUnaligned<uint32_t>(p, 100);
template <typename T, typename U>
void PutUnaligned(void* p, const U& value);

} // namespace common

//////////////////////////////////////////////////////////////////////////////
// implementation

/// known alignment insensitive platforms
#if defined(__i386__) || \
    defined(__x86_64__) || \
    defined(_M_IX86) || \
    defined(_M_X64)
#define COMMON_ALIGNMENT_INSENSITIVE_PLATFORM 1
#endif

#if defined COMMON_ALIGNMENT_INSENSITIVE_PLATFORM
#include "thirdparty/monitor_sdk_common/system/memory/unaligned/align_insensitive.h"
#else
# if defined __GNUC__
#include "thirdparty/monitor_sdk_common/system/memory/unaligned/gcc.h"
# elif defined _MSC_VER
#include "thirdparty/monitor_sdk_common/system/memory/unaligned/msc.h"
# else
#include "thirdparty/monitor_sdk_common/system/memory/unaligned/generic.h"
# endif // compiler detect
#endif // arch detect

namespace common {

/// @brief round up pointer to next nearest aligned address
/// @param p the pointer
/// @param align alignment, must be power if 2
template <typename T>
T* RoundUpPtr(T* p, size_t align)
{
    size_t address = reinterpret_cast<size_t>(p);
    return reinterpret_cast<T*>((address + align - 1) & ~(align - 1U));
}

/// @brief round down pointer to previous nearest aligned address
/// @param p the pointer
/// @param align alignment, must be power if 2
template <typename T>
T* RoundDownPtr(T* p, size_t align)
{
    size_t address = reinterpret_cast<size_t>(p);
    return reinterpret_cast<T*>(address & ~(align - 1U));
}

} // namespace common

#endif // COMMON_SYSTEM_MEMORY_UNALIGNED_H
