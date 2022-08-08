//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 19:47
//  @file:      textfile.cpp
//  @author:    
//  @brief:     
//
//********************************************************************


#include "thirdparty/monitor_sdk_common/system/io/textfile.h"
#include <string.h>
#include <errno.h>
#include <fstream>
#include "thirdparty/monitor_sdk_common/base/stdint.h"
#include "thirdparty/monitor_sdk_common/base/unique_ptr.h"
#include "thirdparty/monitor_sdk_common/base/stdext/string.h"
#include "thirdparty/monitor_sdk_common/base/string/algorithm.h"
#include "thirdparty/monitor_sdk_common/system/io/file.h"

namespace common {

namespace io { namespace textfile {

bool LoadToString(const ::std::string& filename, ::std::string* result, int64_t max_size)
{
    if (max_size <= 0)
        return false;

    long long file_size;
    if (!io::file::GetSize(filename, &file_size))
        return false;

    if (static_cast<uint64_t>(file_size) > SIZE_MAX)
        file_size = max_size;

    std::unique_ptr<FILE> fp(fopen(filename.c_str(), "rb"));
    if (!fp.get())
        return false;

    size_t total_read_size = 0;
    size_t size = static_cast<size_t>(file_size);
    if (size == 0) {
        // normal empty file, or files in folder : /proc/
        size = 32 * 1024;
    } else {
        // 减少一次下面的循环
        // 当普通文件很大的时候，比如3G，如果不将size加1，则：
        // 第二次循环result->resize()可能会抛bad_alloc异常
        size++;
    }
    while (true) {
        try {
            result->resize(total_read_size + size);
        } catch (const ::std::bad_alloc& e) {
            result->clear();
            return false;
        }

        char* buffer = string_as_array(result);
        buffer += total_read_size;
        size_t read_size = fread(buffer, 1, size, fp.get());
        total_read_size += read_size;
        if (read_size < size || total_read_size >= static_cast<size_t>(max_size))
            break;
    }

    if (ferror(fp.get())) {
        result->clear();
        return false;
    }
    result->resize(total_read_size);
    return true;
}

namespace {
template <typename Container>
bool ReadLinesT(const ::std::string& filename, Container* result)
{
    int64_t size = io::file::GetSize(filename);
    if (size < 0)
        return false;

    if (static_cast<uint64_t>(size) > SIZE_MAX) // file is larger than memory
    {
        errno = EFBIG;
        return false;
    }

    ::std::ifstream fs;
    fs.open(filename.c_str(), ::std::ios_base::in);

    if (fs.fail())
        return false;

    result->clear();

    ::std::string line;
    while (::std::getline(fs, line))
    {
        RemoveLineEnding(&line);

        // swap with empty object to reduce copying
        result->push_back(::std::string());
        ::std::swap(result->back(), line);
    }
    fs.close();
    return true;
}
} // anonymous namespace

bool ReadLines(const ::std::string& filename, ::std::vector< ::std::string>* result)
{
    return ReadLinesT(filename, result);
}

bool ReadLines(const ::std::string& filename, ::std::deque< ::std::string>* result)
{
    return ReadLinesT(filename, result);
}

bool ReadLines(const ::std::string& filename, ::std::list< ::std::string>* result)
{
    return ReadLinesT(filename, result);
}

} } // end namespace io.textfile

} // end of namespace common
