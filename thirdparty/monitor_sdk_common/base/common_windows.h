//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 17:36
//  @file:      common_windows.h
//  @author:    
//  @brief:     
//
//********************************************************************


#ifndef COMMON_BASE_COMMON_WINDOWS_H
#define COMMON_BASE_COMMON_WINDOWS_H

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501 // windows xp
#endif

#ifndef NOMINMAX
#define NOMINMAX 1
#endif

#include <windows.h>

#endif // COMMON_BASE_COMMON_WINDOWS_H
