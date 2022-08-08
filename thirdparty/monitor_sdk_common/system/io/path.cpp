//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 19:44
//  @file:      path.cpp
//  @author:    
//  @brief:     
//
//********************************************************************


#include "thirdparty/monitor_sdk_common/system/io/path.h"

#include <stdlib.h>
#include <cstring>

#include "thirdparty/monitor_sdk_common/base/array_size.h"
#include "thirdparty/monitor_sdk_common/base/string/algorithm.h"
#include "thirdparty/monitor_sdk_common/system/io/directory.h"
#include "thirdparty/monitor_sdk_common/system/io/file.h"

namespace common {

namespace io {
namespace path {

using namespace std;

bool IsSeparator(char ch)
{
#ifdef _WIN32
    return ch == '\\' || ch == '/';
#else
    return ch == '/';
#endif
}

string GetBaseName(const char* filepath)
{
    size_t len = strlen(filepath);
    int i = static_cast<int>(len - 1);
    for ( ; i >= 0; i--)
    {
        if (IsSeparator(filepath[i]))
            break;
    }
    return string(filepath + i + 1, filepath + len);
}

string GetBaseName(const string& filepath)
{
    return GetBaseName(filepath.c_str());
}

string GetExtension(const char* filepath)
{
    size_t len = strlen(filepath);
    int i = static_cast<int>(len - 1);
    for ( ; i >= 0; i--)
    {
        if (filepath[i] == '.')
            return string(filepath + i, filepath + len);
        if (IsSeparator(filepath[i]))
            return "";
    }
    return "";
}

string GetExtension(const string& filepath)
{
    return GetExtension(filepath.c_str());
}

string GetDirectory(const char* filepath)
{
    size_t len = strlen(filepath);
    int i = static_cast<int>(len - 1);
    for ( ; i >= 0; i--)
    {
        if (IsSeparator(filepath[i]))
            break;
    }
    if (i >= 0)
        return string(filepath, filepath + i + 1);
    return "";
}

string GetDirectory(const string& filepath)
{
    return GetDirectory(filepath.c_str());
}

string GetFullPath(const char* filepath)
{
    std::string path = IsSeparator(*filepath) ?
            filepath : Join(directory::GetCurrentDir(), filepath);
    return NormalizePath(path);
}

string GetFullPath(const string& filepath)
{
    return GetFullPath(filepath.c_str());
}

// Normalize path, eliminating double slashes, etc.
::std::string NormalizePath(const ::std::string& path)
{
    if (path.empty())
        return ".";

    int initial_slashes = StringStartsWith(path, "/");
    // POSIX allows one or two initial slashes, but treats three or more
    // as single slash.
    if (initial_slashes &&
        StringStartsWith(path, "//") && !StringStartsWith(path, "///"))
    {
        initial_slashes = 2;
    }

    ::std::vector< ::std::string> comps;
    SplitStringKeepEmpty(path, "/", &comps);

    ::std::vector< ::std::string> new_comps;
    for (::std::vector< ::std::string>::iterator i = comps.begin(); i != comps.end(); ++i)
    {
        const ::std::string& comp = *i;
        if (comp.empty() || comp == ".")
            continue;
        if (comp != ".." || (!initial_slashes && new_comps.empty()) ||
            (!new_comps.empty() && new_comps.back() == ".."))
            new_comps.push_back(comp);
        else if (!new_comps.empty())
            new_comps.pop_back();
    }

    ::std::swap(comps, new_comps);
    ::std::string new_path = JoinStrings(comps, "/");
    if (initial_slashes)
        new_path = ::std::string(initial_slashes, '/') + new_path;

    return new_path.empty() ? "." : new_path;
}

static std::string DoJoin(const std::string** paths, size_t size)
{
    std::string result = *paths[0];
    for (size_t i = 1; i < size; ++i) {
        const std::string& path = *paths[i];
        if (!path.empty()) {
            if (path[0] == '/') {
                result = path;
            } else {
                if (!result.empty() && result[result.size() - 1] != '/')
                    result += '/';
                result += path;
            }
        }
    }
    return result;
}

std::string Join(const std::string& p1, const std::string& p2)
{
    const std::string* paths[] = {&p1, &p2};
    return DoJoin(paths, COMMON_ARRAY_SIZE(paths));
}

std::string Join(const std::string& p1, const std::string& p2,
                 const std::string& p3)
{
    const std::string* paths[] = {&p1, &p2, &p3};
    return DoJoin(paths, COMMON_ARRAY_SIZE(paths));
}

std::string Join(const std::string& p1, const std::string& p2,
                 const std::string& p3, const std::string& p4)
{
    const std::string* paths[] = {&p1, &p2, &p3, &p4};
    return DoJoin(paths, COMMON_ARRAY_SIZE(paths));
}

std::string Join(const std::string& p1, const std::string& p2,
                 const std::string& p3, const std::string& p4,
                 const std::string& p5)
{
    const std::string* paths[] = {&p1, &p2, &p3, &p4, &p5};
    return DoJoin(paths, COMMON_ARRAY_SIZE(paths));
}

std::string Join(const std::string& p1, const std::string& p2,
                 const std::string& p3, const std::string& p4,
                 const std::string& p5, const std::string& p6)
{
    const std::string* paths[] = {&p1, &p2, &p3, &p4, &p5, &p6};
    return DoJoin(paths, COMMON_ARRAY_SIZE(paths));
}

} // end of namespace path
} // end of namespace io
} // end of namespace common
