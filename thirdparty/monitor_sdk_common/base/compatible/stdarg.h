//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 15:53
//  @file:      stdarg.h
//  @brief:     
//
//********************************************************************


#ifndef COMMON_BASE_COMPATIBLE_STDARG_H
#define COMMON_BASE_COMPATIBLE_STDARG_H

#include <stdarg.h>

#ifndef va_copy
/// @macro va_copy
/// @brief copy va_list object,
/// void va_copy(va_list dest, va_list src);
/// @param src source va_list
/// @param dest dest va_list
/// @details
/// An obvious implementation would have a va_list  be  a  pointer  to  the
/// stack frame of the variadic function.  In such a setup (by far the most
/// common) there seems nothing against an assignment
///            va_list aq = ap;
/// Unfortunately, there are also systems that make it an array of pointers
/// (of length 1), and there one needs
///            va_list aq;
///            *aq = *ap;
/// Finally, on systems where parameters are passed in registers, it may be
/// necessary for va_start()  to  allocate  memory,  store  the  parameters
/// there,  and  also  an  indication  of  which parameter is next, so that
/// va_arg() can step through the list. Now va_end() can free the allocated
/// memory  again.   To  accommodate  this  situation,  C99  adds  a  macro
/// va_copy(), so that the above assignment can be replaced by
///            va_list aq;
///            va_copy(aq, ap);
///            ...
///            va_end(aq);
/// Each invocation of va_copy() must be matched by a corresponding invoca-
/// tion of va_end() in the same function.  Some systems that do not supply
/// va_copy() have __va_copy instead, since that was the name used  in  the
/// draft proposal.

#if 0
#ifdef __cplusplus
// if va_list is an object, match this
template <typename T>
void guess_va_copy_helper(T* dest, T* src) { *dest = *src; }

// if va_list is a pointer, match this
template <typename T>
void guess_va_copy_helper(T** dest, T** src) { *dest = *src; }

// if va_list is an array[1], match this
template <typename T>
void guess_va_copy_helper(T (*dest)[1] dest, T (*src[]))
{
    (*dest)[0] = (*src)[0];
}

inline void guess_va_copy(va_list& dest, va_list& src)
{
    guess_va_copy_helper(&dest, &src);
}
#define va_copy(dest, src) guess_va_copy(dest, src)
#endif
#endif

# ifdef __va_copy
#  define va_copy __va_copy
# else
#  ifdef _MSC_VER
#   define va_copy(dest, src) ((dest) = (src)) // it does work!
#  else
#   error "Don't known how to define va_copy"
#  endif // _MSC_VER
# endif // __va_copy
#endif // va_copy

#endif // COMMON_BASE_COMPATIBLE_STDARG_H
