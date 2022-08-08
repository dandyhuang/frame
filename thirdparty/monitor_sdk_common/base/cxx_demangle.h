// Copyright (c) 2009, Vivo Inc.
// All rights reserved.
//

#ifndef COMMON_BASE_CXX_DEMANGLE_H
#define COMMON_BASE_CXX_DEMANGLE_H

/// @file CxxDemangle.hpp
/// @author phongchen
/// brief do C++ demangling
/// convert mangled name to human readable form
/// support gcc/MSVC C++ ABI
///
/// example:
/// _ZNKSt13basic_fstreamIcSt11char_traitsIcEE7is_openEv
/// ->
/// std::basic_fstream<char, std::char_traits<char> >::is_open() const

#include <string>
#include <typeinfo>

#include <cxxabi.h>

inline std::string CxxDemangle(const char* name)
{
    char buffer[4096];
    size_t size = sizeof(buffer);
    int status;
    if (abi::__cxa_demangle(name, buffer, &size, &status))
        return std::string(buffer);
    else
        return name;
}

#endif // COMMON_BASE_CXX_DEMANGLE_H

