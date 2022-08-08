//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-05-14 10:01
//  @file:      common_file.cpp
//  @author:
//  @brief:
//
//********************************************************************


#ifndef _CRT_NONSTDC_NO_WARNINGS
#define _CRT_NONSTDC_NO_WARNINGS
#endif

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <string>
#include <stdlib.h>
#include <fstream>
#include "thirdparty/monitor_sdk_common/base/stdint.h"
#include "thirdparty/monitor_sdk_common/system/io/directory.h"
#include "thirdparty/monitor_sdk_common/system/io/file.h"

#ifdef __unix__
#include <unistd.h>
#include <fcntl.h>
#include <utime.h>
#include <sys/time.h>
#else
#include <sys/utime.h>
#include <io.h>
#include <fcntl.h>
#endif

using namespace std;

#ifdef _WIN32
#define R_OK    0x02
#define W_OK    0x04
#define utimbuf _utimbuf
#endif

namespace common {

namespace io
{

namespace file
{

bool Exists(const char* filename)
{
    return IsRegular(filename);
}

bool Exists(const string& filename)
{
    return Exists(filename.c_str());
}

bool IsRegular(const char* filename)
{
    struct stat buf;
    if (!(stat(filename, &buf) == 0))
    {
        return false;
    }
    return (buf.st_mode & S_IFREG) != 0;
}

bool IsRegular(const string& filename)
{
    return IsRegular(filename.c_str());
}

bool GetSize(const char* filename, long long* size)
{
    struct stat buf;
    int ret = stat(filename, &buf);
    if (ret < 0)
    {
        return false;
    }

    *size = buf.st_size;
    return true;
}

bool GetSize(const std::string& filename, long long* size)
{
    return GetSize(filename.c_str(), size);
}

long long GetSize(const char* filename)
{
    struct stat buf;
    int ret = stat(filename, &buf);
    if (ret < 0)
    {
        return -1;
    }
    return buf.st_size;
}

long long GetSize(const string& filename)
{
    return GetSize(filename.c_str());
}

bool Copy(const char *src, const char* dest, bool overwrite)
{
#ifdef _WIN32
    return CopyFileA(src, dest, !overwrite) != FALSE;
#else
    FILE*  fp_src;
    FILE*  fp_dest;
    char   buffer[4096];

    if (!overwrite) //check if dest file exits when no over writing
    {
        if (access(dest, F_OK) == 0)  // file exists
        {
            return false;
        }
        else if (errno != ENOENT) // file doesn't exist, go on only when the file name is invaild
        {
            return false;
        }
    }

    if ((fp_src = fopen(src, "rb")) == NULL)
    {
        return false;
    }
    if ((fp_dest = fopen(dest, "wb+")) == NULL)
    {
        fclose(fp_src);
        return false;
    }

    size_t ret = 0;
    while ((ret = fread(buffer, 1, sizeof(buffer), fp_src)) > 0)
    {
        if (fwrite(buffer, 1, ret, fp_dest) != ret)
        {
            fclose(fp_src);
            fclose(fp_dest);
            return false;
        }
    }
    fclose(fp_src);
    fclose(fp_dest);

    return true;
#endif
}

bool Copy(const string& src, const string& dest, bool overwrite)
{
    return Copy(src.c_str(), dest.c_str(), overwrite);
}

bool Delete(const char* filename)
{
    return remove(filename) == 0;
}

bool Delete(const string& filename)
{
    return Delete(filename.c_str());
}

bool Rename(const char* oldname, const char* newname)
{
    return rename(oldname, newname) == 0;
}

bool Rename(const string& oldname, const string& newname)
{
    return Rename(oldname.c_str(), newname.c_str());
}

bool GetTime(const std::string& filename, FileTime* times)
{
    struct stat buf;
    int ret = stat(filename.c_str(), &buf);
    if (ret < 0)
        return false;
    times->create_time = buf.st_ctime;
    times->access_time = buf.st_atime;
    times->modify_time = buf.st_mtime;

    return true;
}

bool SetTime(const std::string& filename, const FileTime& times)
{
    utimbuf utimes = {0, 0};
    utimes.actime = times.access_time;
    utimes.modtime = times.modify_time;
#ifdef __unix__
    return utime(filename.c_str(), &utimes) == 0;
#elif _WIN32
    return _utime(filename.c_str(), &utimes) == 0;
#endif
}

bool Touch(const std::string& filename)
{
#ifdef __unix__
    int fd = open(filename.c_str(), O_WRONLY|O_NONBLOCK|O_CREAT|O_NOCTTY, 0666);
    if (fd >= 0)
    {
        close(fd);
        return utimes(filename.c_str(), NULL) == 0;
    }
#endif
    return false;
}
time_t GetAccessTime(const char *filename)
{
    if (!IsRegular(filename))
    {
        return -1;
    }
    struct stat buf;
    int ret = stat(filename, &buf);
    if (ret < 0)
    {
        return -1;
    }
    return buf.st_atime;
}

time_t GetAccessTime(const string& filename)
{
    return GetAccessTime(filename.c_str());
}

time_t GetCreateTime(const char *filename)
{
    if (!IsRegular(filename))
    {
        return -1;
    }
    struct stat buf;
    int ret = stat(filename, &buf);
    if (ret < 0)
    {
        return -1;
    }
    return buf.st_ctime;
}

time_t GetCreateTime(const string& filename)
{
    return GetCreateTime(filename.c_str());
}

time_t GetLastModifyTime(const char *filename)
{
    if (!IsRegular(filename))
    {
        return -1;
    }
    struct stat buf;
    int ret = stat(filename, &buf);
    if (ret < 0)
    {
        return -1;
    }
    return buf.st_mtime;
}

time_t GetLastModifyTime(const string& filename)
{
    return GetLastModifyTime(filename.c_str());
}

bool IsReadable(const char* filename)
{
    if (!Exists(filename))
    {
        return false;
    }
    return access(filename, R_OK) == 0;
}

bool IsReadable(const string& filename)
{
    return IsReadable(filename.c_str());
}

bool IsWritable(const char* filename)
{
    if (!Exists(filename))
    {
        return false;
    }
    return access(filename, W_OK) == 0;
}

bool IsWritable(const std::string& filename)
{
    return IsWritable(filename.c_str());
}

bool ReadAll(const std::string& filename, void* buffer, size_t buffer_size, size_t* read_size)
{
    long long size = GetSize(filename);
    if (size < 0)
        return false;

    if ((unsigned long long)size > buffer_size) // file too large
        return false;

    int fd = open(filename.c_str(), O_RDONLY);
    if (fd < 0)
        return false;

    int read_length = read(fd, buffer, buffer_size);
    if (read_length < 0)
    {
        close(fd);
        return false;
    }
    *read_size = read_length;
    close(fd);
    return true;
}

template <typename T>
static bool ReadFileIntoContailer(const std::string& filename, T* content)
{
    long long size = GetSize(filename);
    if (size < 0)
    {
        return false;
    }

    if ((unsigned long long)size > SIZE_MAX)
    {
        // file too large
        return false;
    }

    int fd = open(filename.c_str(), O_RDONLY);
    if (fd < 0)
    {
        return false;
    }

    try
    {
        content->resize(size);
    }
    catch (std::bad_alloc& e)
    {
        close(fd);
        // file too large to fit into memory
        return false;
    }
    long long read_length = read(fd, &*content->begin(), size);
    if (read_length < 0)
    {
        close(fd);
        return false;
    }
    content->resize(read_length);
    close(fd);
    return true;
}

bool ReadAll(const std::string& filename, std::string* content)
{
    return ReadFileIntoContailer(filename, content);
}

bool ReadAll(const std::string& filename, std::vector<char>* content)
{
    return ReadFileIntoContailer(filename, content);
}

} // namespace io::file

} // namespace io

} // end of namespace common
