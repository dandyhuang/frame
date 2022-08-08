// Copyright (c) 2015, Vivo Inc. All rights reserved.
/**
 * @file file_fwd.h
 * @brief Forwarding declarations for common File related classes.
 * @author chenzheng05@baidu.com
 * @date 2015-01-20
 */

#ifndef COMMON_FILE_FILE_FWD_H
#define COMMON_FILE_FILE_FWD_H
#pragma once

namespace common {
class File;

struct OpenFileOptions;
class KeyValueInfoHelper;

struct AttrsMask;
struct AttrsInfo;
struct Attrs;
struct DataLocation;
struct FileContentSummary;
struct FileContentSummaryMask;

class AbstractStreamFile;

} // namespace common

#endif // COMMON_FILE_FILE_FWD_H
