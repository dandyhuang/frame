//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 19:41
//  @file:      path.h
//  @author:    
//  @brief:     
//
//********************************************************************

#ifndef COMMON_SYSTEM_IO_PATH_H
#define COMMON_SYSTEM_IO_PATH_H

#include <string>

namespace common {

namespace io {
namespace path {

bool IsSeparator(char ch);

::std::string GetBaseName(const char* filepath);
::std::string GetBaseName(const ::std::string& filepath);

::std::string GetExtension(const char* filepath);
::std::string GetExtension(const ::std::string& filepath);

::std::string GetDirectory(const char* filepath);
::std::string GetDirectory(const ::std::string& filepath);

::std::string GetFullPath(const char* filepath);
::std::string GetFullPath(const ::std::string& filepath);

// Normalize a path, e.g. A//B, A/./B and A/foo/../B all become A/B.
// Take the same behavior as os.path.normpath of python.
::std::string NormalizePath(const ::std::string& filepath);

// Join two or more pathname components, inserting '/' as needed.
// If any component is an absolute path, all previous path components
// will be discarded.
std::string Join(const std::string& p1, const std::string& p2);
std::string Join(const std::string& p1, const std::string& p2,
                 const std::string& p3);
std::string Join(const std::string& p1, const std::string& p2,
                 const std::string& p3, const std::string& p4);
std::string Join(const std::string& p1, const std::string& p2,
                 const std::string& p3, const std::string& p4,
                 const std::string& p5);
std::string Join(const std::string& p1, const std::string& p2,
                 const std::string& p3, const std::string& p4,
                 const std::string& p5, const std::string& p6);

} // namespace io::path
} // namespace io
} // end of namespace common

#endif // COMMON_SYSTEM_IO_PATH_H
