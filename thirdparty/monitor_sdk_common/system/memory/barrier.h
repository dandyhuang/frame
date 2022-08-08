// Copyright (c) 2012, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#ifndef COMMON_SYSTEM_MEMORY_BARRIER_H
#define COMMON_SYSTEM_MEMORY_BARRIER_H

namespace common {

inline void CompilerBarrier() { __asm__ __volatile__("": : :"memory"); }
#if defined __i386__ || defined __x86_64__
inline void MemoryBarrier() { __asm__ __volatile__("mfence": : :"memory"); }
inline void MemoryReadBarrier() { __asm__ __volatile__("lfence" ::: "memory"); }
inline void MemoryWriteBarrier() { __asm__ __volatile__("sfence" ::: "memory"); }
#else
#error Unsupportted platform.
#endif

} // namespace common

#endif // COMMON_SYSTEM_MEMORY_BARRIER_H

