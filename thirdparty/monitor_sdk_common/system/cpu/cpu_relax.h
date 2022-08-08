//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-14 15:17
//  @file:      cpu_relax.h
//  @author:    
//  @brief:     
//
//********************************************************************


#ifndef COMMON_SYSTEM_CPU_CPU_RELAX_H
#define COMMON_SYSTEM_CPU_CPU_RELAX_H
#pragma once

namespace common {

// Relax CPU to improves the performance of spin-wait loops.
inline void CpuRelax()
{
    // From Intel Manual:
    //
    // When executing a 'spin-wait loop', a Pentium 4 or Intel Xeon processor
    // suffers a severe performance penalty when exiting the loop because it
    // detects a possible memory order violation. The PAUSE instruction
    // provides a hint to the processor that the code sequence is a spin-wait
    // loop. The processor uses this hint to avoid the memory order violation
    // in most situations, which greatly improves processor performance. For
    // this reason, it is recommended that a PAUSE instruction be placed in
    // all spin-wait loops.
    //
    // An additional fucntion of the PAUSE instruction is to reduce the power
    // consumed by a Pentium 4 processor while executing a spin loop.
    //
    // The "rep;nop" is same as "pause", but it is recognized by any old assembiler.
#if defined __GNUC__ && (defined __i386__ || defined __x86_64__)
    __asm__ __volatile__("rep;nop": : :"memory");
#elif defined _WIN32
    __asm rep nop;
#endif

}

} // end of namespace common

#endif // COMMON_SYSTEM_CPU_CPU_RELAX_H
