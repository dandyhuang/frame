// Copyright (c) 2010, The Toft Authors. All rights reserved.
// Author: Chen Feng <chen3feng@gmail.com>

#ifndef COMMON_SYSTEM_INFO_INFO_H
#define COMMON_SYSTEM_INFO_INFO_H

// GLOBAL_NOLINT(runtime/int)

#include <string>

namespace common {

unsigned int GetLogicalCpuNumber();
unsigned long long GetPhysicalMemorySize();
unsigned long long GetTotalPhysicalMemorySize();
bool GetOsKernelInfo(std::string* kernerl_info);
bool GetMachineArchitecture(std::string* arch_info);
std::string GetUserName();

} // namespace common

#endif // COMMON_SYSTEM_INFO_INFO_H
