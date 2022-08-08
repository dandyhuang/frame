// Copyright (c) 2013, The COMMON Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 2013-02-05

#ifndef COMMON_BASE_STRING_FORMAT_VSCAN_H
#define COMMON_BASE_STRING_FORMAT_VSCAN_H
#pragma once

#include <string>

#include "thirdparty/monitor_sdk_common/base/string/format/scan_arg.h"

namespace common {

// Scan string with arguments vector
// Return: number filelds of success scanned.
int StringVScan(const char* string, const char* format, const FormatScanArg** args, int nargs);

} // namespace common

#endif // COMMON_BASE_STRING_FORMAT_VSCAN_H
