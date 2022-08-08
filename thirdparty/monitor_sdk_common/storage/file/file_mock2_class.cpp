// Copyright (c) 2012, Vivo Inc.
// All rights reserved.
//
// Author: CHEN Feng
// Created: 2012-07-26
// Description: Register mock file class to the File class registry.

#include "thirdparty/monitor_sdk_common/storage/file/file_mock2.h"

namespace common {

// Put this to a standalone source file to avoid link_all_symbols apply to all
// symbols of file mock unnecessaryly.
REGISTER_FILE_IMPL(FileMock2::kPrefix, FileMock2);

} // namespace common
