//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-14 10:04
//  @file:      directory.cpp
//  @author:
//  @brief:
//
//********************************************************************


#include "thirdparty/monitor_sdk_common/system/io/directory.h"

#include <stdlib.h>

#include <cstring>
#include <iostream>
#include <set>
#include <stack>

#include "thirdparty/monitor_sdk_common/base/string/concat.h"
#include "thirdparty/monitor_sdk_common/system/io/file.h"
#include "thirdparty/monitor_sdk_common/system/io/path.h"
#include "thirdparty/monitor_sdk_common/text/wildcard.h"

using namespace std;

namespace common {

namespace io
{

#ifdef  _WIN32 // _WIN32
#define R_OK    0x02
#define W_OK    0x04
#ifndef stat
#define stat    _stat
#endif
#define access  _access
#define S_IFREG _S_IFREG
#define S_IFDIR _S_IFDIR
const char kPathSep = '\\';
#else // Other platform
const char kPathSep = '/';
#endif

void DirectoryIterator::Initialize()
{
#ifdef _WIN32
    m_Handle = INVALID_HANDLE_VALUE;
#else
    m_Dir = NULL;
    m_Dirent = NULL;
#endif
    m_End = true;
    m_Flags = ALL;
    m_exclude_flags = INVALID;
}

DirectoryIterator::DirectoryIterator()
{
    Initialize();
}

DirectoryIterator::DirectoryIterator(const char* dir, int flags,
                                     const char* filter, int exclude_flags)
{
    Initialize();
    Open(dir, flags, filter, exclude_flags);
}

bool DirectoryIterator::SkipCurrent()
{
#ifdef _WIN32
    // skip "." and ".." directory
    if (strncmp(m_FindFileData.cFileName, ".", MAX_PATH) == 0  ||
        strncmp(m_FindFileData.cFileName, "..", MAX_PATH) == 0)
        return true;
    // required file but get a directory
    if (m_Flags == FILE && (m_FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
        return true;
    // required directory but get a file
    if (m_Flags == DIRECTORY && (m_FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
        return true;
#else
    if (m_Dirent == NULL)
        return false;
    // skip "." and ".." directory
    if (strncmp(m_Dirent->d_name, ".", MAX_PATH_LEN) == 0 ||
        strncmp(m_Dirent->d_name, "..", MAX_PATH_LEN) == 0)
        return true;
    if (!m_FilterPattern.empty() && !Wildcard::Match(m_FilterPattern, m_Dirent->d_name))
        return true;
    int type = GetType();
    if ((type == INVALID) ||
        ((type & m_Flags) == 0) ||
        ((type & m_exclude_flags) != 0)) {
        return true;
    }
#endif
    return false;
}

int DirectoryIterator::GetType() const
{
    if (m_End)
    {
        return INVALID;
    }
    int type = INVALID;
#ifdef _WIN32
    if ((m_FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
    {
        type |= DIRECTORY;
    }
    else
    {
        type |= FILE;
    }
#else
    string path = m_OpenDir;
    path += "/";
    path += m_Dirent->d_name;
    struct stat buf;
    int ret = lstat(path.c_str(), &buf);
    if (ret < 0)
    {
        return type;
    }
    if (S_ISDIR(buf.st_mode))
        type |= DIRECTORY;
    if (S_ISREG(buf.st_mode))
        type |= FILE;
    if (S_ISLNK(buf.st_mode))
        type |= SYMBOLLINK;
#endif
    return type;
}

bool DirectoryIterator::Open(const string& dir, int flags,
                             const std::string& filter, int exclude_flags)
{
    m_Flags = flags;
    m_exclude_flags = exclude_flags;
    m_OpenDir = dir;
    m_FilterPattern = filter;
#ifdef _WIN32
    if (m_Handle != INVALID_HANDLE_VALUE)
    {
        FindClose(m_Handle);
    }
    string path = dir;
    path += kPathSep;
    if (m_FilterPattern.empty())
    {
        path += "*";
    }
    else
    {
        path += filter;
    }
    m_Handle = FindFirstFile(path.c_str(), &m_FindFileData);
    if (m_Handle == INVALID_HANDLE_VALUE) // open failed
    {
        return false;
    }
    m_End = false;
    if (SkipCurrent())
    {
        Next();
    }
#else
    if (m_Dir)
    {
        closedir(m_Dir);
    }
    m_Dir = opendir(dir.c_str());
    if (!m_Dir) // open failed
    {
        return false;
    }
    m_End = false;
    Next();
#endif
    return true;
}

bool DirectoryIterator::IsEnd()
{
    return m_End;
}

string DirectoryIterator::Name() const
{
    if (m_End)
    {
        return "";
    }
#ifdef _WIN32
    return m_FindFileData.cFileName;
#else
    return m_Dirent->d_name;
#endif
}

string DirectoryIterator::FullPath()
{
    string filename = Name();
    if (filename.empty())
        return filename;
    string related_path = m_OpenDir;
    related_path += kPathSep;
    related_path += filename;
    filename = path::GetFullPath(related_path);
    return filename;
}

bool DirectoryIterator::Next()
{
#ifdef _WIN32
    if (m_Handle != INVALID_HANDLE_VALUE)
    {
        int ret = FindNextFile(m_Handle, &m_FindFileData);
        // success when nonzero returned
        while (ret != 0 && SkipCurrent())
        {
            ret = FindNextFile(m_Handle, &m_FindFileData);
        }
        if (ret == 0)
        {
            m_End = true;
        }
        return ret != 0;
    }
#else
    if (m_Dir)
    {
        m_Dirent = readdir(m_Dir);
        while (m_Dirent != NULL && SkipCurrent())
        {
            m_Dirent = readdir(m_Dir);
        }
        if (m_Dirent == NULL)
        {
            m_End = true;
        }
        return m_Dirent != NULL;
    }
#endif
    return false;
}

bool DirectoryIterator::Close()
{
#ifdef _WIN32
    if (m_Handle != INVALID_HANDLE_VALUE)
    {
        FindClose(m_Handle);
        m_Handle = INVALID_HANDLE_VALUE;
    }
#else
    if (m_Dir)
    {
        closedir(m_Dir);
        m_Dir = NULL;
    }
#endif
    m_End = true;
    return true;
}


namespace directory
{

bool IsDir(const char* dir)
{
    struct stat buf;
    if (!(stat(dir, &buf) == 0))
    {
        return false;
    }
    return (buf.st_mode & S_IFDIR) != 0;
}

bool IsDir(const string& dir)
{
    return Exists(dir.c_str());
}

bool Exists(const char* dir)
{
    return IsDir(dir);
}

bool Exists(const string& dir)
{
    return IsDir(dir);
}

bool IsReadable(const char* dir)
{
    if (!Exists(dir))
    {
        return false;
    }
    return access(dir, R_OK) == 0;
}

bool IsReadable(const string& dir)
{
    return IsReadable(dir.c_str());
}

bool IsWritable(const char* dir)
{
    if (!Exists(dir))
    {
        return false;
    }
    return access(dir, W_OK) == 0;
}

bool IsWritable(const string& dir)
{
    return IsWritable(dir.c_str());
}

bool Create(const char* dir, int mode)
{
#ifdef _WIN32
    return (_mkdir(dir) == 0);
#else
    return (mkdir(dir, mode) == 0);
#endif
}

bool Create(const string& dir, int mode)
{
    return Create(dir.c_str(), mode);
}


bool RecursiveCreate(const char* dir, int mode)
{
    bool ret = true;
    std::string dir_path = io::path::NormalizePath(dir);

    std::stack<std::string> paths;
    while (!Exists(dir_path)) {
        paths.push(io::path::GetBaseName(dir_path));
        dir_path = io::path::NormalizePath(
            io::path::GetDirectory(dir_path));
    }

    // 已经存在
    if (paths.empty()) {
        return false;
    }

    // 记录start_dir，便于后续删除到第一个不存在的目录
    std::string start_dir = StringConcat(dir_path, "/", paths.top());
    while (!paths.empty()) {
        dir_path = StringConcat(dir_path, "/", paths.top());
        paths.pop();
        if (!Create(dir_path, mode)) {
            ret = false;
            break;
        }
    }

    if (!ret) {
        RecursiveDelete(start_dir);
    }
    return ret;
}

bool RecursiveCreate(const string& dir, int mode)
{
    return RecursiveCreate(dir.c_str(), mode);
}

bool Delete(const char *dir)
{
    if (!Exists(dir))
    {
        return false;
    }
#ifdef _WIN32
    return _rmdir(dir) == 0;
#else
    return rmdir(dir) == 0;
#endif
}

bool Delete(const string& dir)
{
    return Delete(dir.c_str());
}

string ToWindowsFormat(const char* dir)
{
    string str(dir);
    size_t len = str.length();
    for (size_t idx = 0; idx < len; idx++)
    {
        if (str[idx] == '/')
        {
            str[idx] = '\\';
        }
    }
    return str;
}

string ToWindowsFormat(const string& dir)
{
    return ToWindowsFormat(dir.c_str());
}

string ToUnixFormat(const char* dir)
{
    string str(dir);
    size_t len = str.length();
    for (size_t idx = 0; idx < len; idx++)
    {
        if (str[idx] == '\\')
        {
            str[idx] = '/';
        }
    }
    return str;
}

string ToUnixFormat(const string& dir)
{
    return ToUnixFormat(dir.c_str());
}

bool SetCurrentDir(const char *dir)
{
    if (!Exists(dir))
    {
        return false;
    }

#ifdef _WIN32
    return (SetCurrentDirectoryA(dir) != 0);
#else
    return (chdir(dir) == 0);
#endif
}

bool SetCurrentDir(const string& dir)
{
    return SetCurrentDir(dir.c_str());
}

::std::string GetCurrentDir()
{
#ifdef _WIN32
    char path[MAX_PATH];
    int ret = GetCurrentDirectoryA(MAX_PATH, path);
    if (ret != 0)
    {
        return string(path, path + ret);
    }
    return "";
#else
    char path[MAX_PATH_LEN];
    char* p = getcwd(path, MAX_PATH_LEN);
    // p may be NULL in some bad condition, for example, dir unlinked
    return p ? p : "";
#endif
}

time_t GetAccessTime(const char* dir)
{
    if (!IsDir(dir))
    {
        return -1;
    }
    struct stat buf;
    int ret = stat(dir, &buf);
    if (ret < 0)
    {
        return -1;
    }
    return buf.st_atime;
}

time_t GetAccessTime(const string& dir)
{
    return GetAccessTime(dir.c_str());
}

time_t GetCreateTime(const char* dir)
{
    if (!IsDir(dir))
    {
        return -1;
    }
    struct stat buf;
    int ret = stat(dir, &buf);
    if (ret < 0)
    {
        return -1;
    }
    return buf.st_ctime;
}

time_t GetCreateTime(const string& dir)
{
    return GetCreateTime(dir.c_str());
}

time_t GetLastModifyTime(const char* dir)
{
    if (!IsDir(dir))
    {
        return -1;
    }
    struct stat buf;
    int ret = stat(dir, &buf);
    if (ret < 0)
    {
        return -1;
    }
    return buf.st_mtime;
}

time_t GetLastModifyTime(const string& dir)
{
    return GetLastModifyTime(dir.c_str());
}

bool GetFiles(const char* dir, ::std::vector< ::std::string>* files)
{
    files->clear();
    DirectoryIterator iter;
    if (!iter.Open(dir, DirectoryIterator::FILE))
    {
        return false;
    }
    while (!iter.IsEnd())
    {
        string filename = iter.Name();
        string path = dir;
        path += kPathSep;
        path += filename;
        files->push_back(path);
        iter.Next();
    }
    iter.Close();
#ifdef _WIN32
    for (size_t idx = 0; idx < files->size(); idx++)
    {
        files->at(idx) = ToWindowsFormat(files->at(idx));
    }
#endif
    return true;
}

bool GetFiles(const ::std::string& dir, ::std::vector< ::std::string>* files)
{
    return GetFiles(dir.c_str(), files);
}

bool GetAllFiles(const char* dir, ::std::vector< ::std::string>* files)
{
    files->clear();
    bool ret = GetFiles(dir, files);
    set<string> visited_dirs;
    string fullpath = path::GetFullPath(dir);
    visited_dirs.insert(fullpath);

    vector<string> subdirs;
    ret = ret && GetAllSubDirs(dir, &subdirs);
    vector<string> subfiles;
    for (size_t i = 0; i < subdirs.size(); i++)
    {
        fullpath = path::GetFullPath(subdirs[i].c_str());
        if (visited_dirs.find(fullpath) != visited_dirs.end())
        {
            continue;
        }
        visited_dirs.insert(fullpath);
        ret = ret && GetFiles(subdirs[i], &subfiles);
        files->insert(files->end(), subfiles.begin(), subfiles.end());
    }
#ifdef _WIN32
    for (size_t idx = 0; idx < files->size(); idx++)
    {
        files->at(idx) = ToWindowsFormat(files->at(idx));
    }
#endif
    return ret;
}

bool GetAllFiles(const ::std::string& dir, ::std::vector< ::std::string>* files)
{
    return GetAllFiles(dir.c_str(), files);
}

bool GetSubDirs(const char* dir, ::std::vector< ::std::string>* subdirs)
{
    subdirs->clear();
    DirectoryIterator iter;
    if (!iter.Open(dir, DirectoryIterator::DIRECTORY))
    {
        return false;
    }
    while (!iter.IsEnd())
    {
        string filename = iter.Name();
        string path = dir;
        path += kPathSep;
        path += filename;
        subdirs->push_back(path);
        iter.Next();
    }
    iter.Close();
#ifdef _WIN32
    for (size_t idx = 0; idx < subdirs->size(); idx++)
    {
        subdirs->at(idx) = ToWindowsFormat(subdirs->at(idx));
    }
#endif
    return true;
}

bool GetSubDirs(const ::std::string& dir, ::std::vector< ::std::string>* subdirs)
{
    return GetSubDirs(dir.c_str(), subdirs);
}

bool GetAllSubDirs(const char* dir, ::std::vector< ::std::string>* subdirs)
{
    subdirs->clear();
    bool ret = GetSubDirs(dir, subdirs);
    set<string> visited_dirs;
    string fullpath = path::GetFullPath(dir);
    visited_dirs.insert(fullpath);

    size_t idx = 0;
    while (idx < subdirs->size())
    {
        fullpath = path::GetFullPath(subdirs->at(idx).c_str());
        if (visited_dirs.find(fullpath) == visited_dirs.end())
        {
            vector<string> vdirs;
            ret = ret && GetSubDirs(subdirs->at(idx), &vdirs);
            subdirs->insert(subdirs->end(), vdirs.begin(), vdirs.end());
            visited_dirs.insert(fullpath);
        }
        idx++;
    }
#ifdef _WIN32
    for (idx = 0; idx < subdirs->size(); idx++)
    {
        subdirs->at(idx) = ToWindowsFormat(subdirs->at(idx));
    }
#endif
    return ret;
}

bool GetAllSubDirs(const ::std::string& dir, ::std::vector< ::std::string>* subdirs)
{
    return GetAllSubDirs(dir.c_str(), subdirs);
}

bool GetAll( const char* dir, ::std::vector< ::std::string>* files )
{
    files->clear();
    DirectoryIterator iter;
    if (!iter.Open(dir, DirectoryIterator::ALL))
    {
        return false;
    }
    while (!iter.IsEnd())
    {
        string filename = iter.Name();
        string path = dir;
        path += kPathSep;
        path += filename;
        files->push_back(path);
        iter.Next();
    }
    iter.Close();
#ifdef _WIN32
    for (size_t idx = 0; idx < files->size(); idx++)
    {
        files->at(idx) = ToWindowsFormat(files->at(idx));
    }
#endif
    return true;
}

bool GetAll( const ::std::string& dir, ::std::vector< ::std::string>* files )
{
    return GetAll(dir.c_str(), files);
}

bool RecursiveDelete(const char* dir)
{
    DirectoryIterator iter;
    bool ret = iter.Open(dir);
    if (!ret)
    {
        return false;
    }
    while (!iter.IsEnd())
    {
        string filepath = dir;
        filepath += kPathSep;
        filepath += iter.Name();
        if ((iter.GetType() & (DirectoryIterator::FILE | DirectoryIterator::SYMBOLLINK)) != 0)
        {
            ret = ret && file::Delete(filepath);
        }
        else
        {
            ret = ret && RecursiveDelete(filepath);
        }
        iter.Next();
    }
    directory::Delete(dir);
    iter.Close();
    return ret;
}

bool RecursiveDelete(const ::std::string& dir)
{
    return RecursiveDelete(dir.c_str());
}

} // namespace io::directory

} // namespace io

} // end of namespace common
