// Copyright (c) 2010, Vivo Inc.
// All rights reserved.
//

#ifndef COMMON_BASE_STDEXT_HASH_FUNCTION_H
#define COMMON_BASE_STDEXT_HASH_FUNCTION_H

#ifdef __GNUC__

#if __GNUC__ < 4 || __GNUC__ == 4 && __GNUC_MINOR__ < 2
#include <ext/hash_fun.h>
#else
#include <backward/hash_fun.h>
#endif // __GNUC__ version

#include <stddef.h>

namespace std {
template <typename Char, typename Traits, typename Allocator> class basic_string;
}

namespace __gnu_cxx {

template<>
struct hash<long long>
{
    size_t operator()(long long __x) const
    {
        if (sizeof(__x) == sizeof(size_t))
            return __x;
        else
            return (__x >> 32) ^ (__x & 0xFFFFFFFF);
    }
};

template<>
struct hash<unsigned long long>
{
    size_t operator()(unsigned long long __x) const
    {
        if (sizeof(__x) == sizeof(size_t))
            return __x;
        else
            return (__x >> 32) ^ (__x & 0xFFFFFFFF);
    }
};

template<typename Traits, typename Allocator>
struct hash<std::basic_string<char, Traits, Allocator> >
{
    size_t operator()(const std::basic_string<char, Traits, Allocator>& s) const
    {
        return __stl_hash_string(s.c_str());
    }
};

} // namespace __gnu_cxx

#endif // __GNUC__

#endif // COMMON_BASE_STDEXT_HASH_FUNCTION_H

