//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-05-14 9:59
//  @file:      file.h
//  @author:    
//  @brief:     file operation interfaces.
//
//********************************************************************

#ifndef COMMON_SYSTEM_IO_FILE_H
#define COMMON_SYSTEM_IO_FILE_H



#include <string>
#include <vector>
#include <thirdparty/monitor_sdk_common/base/stdint.h>

namespace common {

namespace io
{

namespace file
{

bool Copy(const char* src, const char* dest, bool overwrite = false);
bool Copy(const ::std::string& src, const ::std::string& dest, bool overwrite = false);
bool Delete(const char* filename);
bool Delete(const ::std::string& filename);
bool Rename(const char* oldname, const char* newname);
bool Rename(const ::std::string& oldname, const ::std::string& newname);

bool IsRegular(const char* filename);
bool IsRegular(const ::std::string& filename);
bool Exists(const char* filename);
bool Exists(const ::std::string& filename);

bool IsReadable(const char* filename);
bool IsReadable(const ::std::string& filename);
bool IsWritable(const char* filename);
bool IsWritable(const ::std::string& filename);

bool GetSize(const char* filename, long long* size);
bool GetSize(const ::std::string& filename, long long* size);
long long GetSize(const char* filename);
long long GetSize(const ::std::string& filename);

struct FileTime
{
    int64_t create_time;
    int64_t access_time;
    int64_t modify_time;
};

bool GetTime(const ::std::string& filename, FileTime* times);
bool SetTime(const ::std::string& filename, const FileTime& times);

bool Touch(const ::std::string& filename);

time_t GetAccessTime(const char* filename);
time_t GetAccessTime(const ::std::string& filename);
time_t GetCreateTime(const char* filename);
time_t GetCreateTime(const ::std::string& filename);
time_t GetLastModifyTime(const char* filename);
time_t GetLastModifyTime(const ::std::string& filename);

/// real all content into container
bool ReadAll(const ::std::string& filename, void* buffer, size_t buffer_size, size_t* read_size);
bool ReadAll(const ::std::string& filename, ::std::string* content);
bool ReadAll(const ::std::string& filename, ::std::vector<char>* content);

} // namespace io::file

} // namespace io

} // end of namespace common

#endif // COMMON_SYSTEM_IO_FILE_H
