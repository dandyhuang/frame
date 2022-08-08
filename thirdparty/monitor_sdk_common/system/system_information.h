//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 18:54
//  @file:      system_information.h
//  @author:    
//  @brief:     
//
//********************************************************************


#ifndef COMMON_SYSTEM_SYSTEM_INFORMATION_H
#define COMMON_SYSTEM_SYSTEM_INFORMATION_H

// GLOBAL_NOLINT(runtime/int)

#include <string>

namespace common {

unsigned int GetLogicalCpuNumber();
unsigned long long GetPhysicalMemorySize();
unsigned long long GetTotalPhysicalMemorySize();
bool GetOsKernelInfo(::std::string* kernerl_info);
bool GetMachineArchitecture(::std::string* arch_info);
::std::string GetUserName();

} // end of namespace common

#endif // COMMON_SYSTEM_SYSTEM_INFORMATION_H
