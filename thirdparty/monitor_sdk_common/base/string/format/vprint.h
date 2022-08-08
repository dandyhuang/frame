// Copyright (c) 2013, The COMMON Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 2013-02-07

#ifndef COMMON_BASE_STRING_FORMAT_VPRINT_H
#define COMMON_BASE_STRING_FORMAT_VPRINT_H
#pragma once

#include <string>

#include "thirdparty/monitor_sdk_common/base/string/format/print_arg.h"

namespace common {

int VFormatPrint(FormatPrintTarget* target, const char* format,
                 const FormatPrintArg** args, int nargs);

int StringVPrintAppend(std::string* out, const char* format,
                       const FormatPrintArg** args, int argc);

int StringVPrintTo(std::string* out, const char* format,
                   const FormatPrintArg** args, int argc);

std::string StringVPrint(const char* format,
                         const FormatPrintArg** args, int argc);

} // namespace common

#endif // COMMON_BASE_STRING_FORMAT_VPRINT_H
