// Copyright (C) 2012, Vivo Inc.
// Author: An Qin (qinan@baidu.com)
//
// Description:
//

#ifndef COMMON_FILE_FILE_TYPES_H
#define COMMON_FILE_FILE_TYPES_H

#include "thirdparty/monitor_sdk_common/base/stdint.h"

namespace common {

enum FileOpenMode {
    FILE_READ = 0x01,
    FILE_WRITE = 0x02,
    FILE_APPEND = 0x04
};

enum FileErrorCode {
    kFileSuccess,
    kFileErrParameter,
    kFileErrOpenFail,
    kFileErrNotOpen,
    kFileErrWrite,
    kFileErrRead,
    kFileErrClose,
    kFileErrNotExit,
    kFileFail,
};

} // end of namespace common

#endif // COMMON_FILE_FILE_TYPES_H
