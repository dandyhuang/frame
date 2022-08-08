// Copyright (c) 2010, Vivo Inc. All rights reserved.
// Created: 2010-06-12

// GLOBAL_NOLINT(runtime/int)

#ifndef COMMON_CRYPTO_HASH_FNV_H
#define COMMON_CRYPTO_HASH_FNV_H

#include <stddef.h>
#include <string>

namespace common {

struct FnvHasher32
{
    typedef unsigned int ResultType;
    ResultType operator()(const void* data, size_t size, ResultType seed) const;
    ResultType operator()(const char* str, ResultType seed) const;

    template <typename Char, typename Traits, typename Allocator>
    ResultType operator()(const std::basic_string<Char, Traits, Allocator>& str,
                          ResultType seed) const
    {
        return this->operator()(str.data(), str.length(), seed);
    }
};

struct FnvHasher64
{
    typedef unsigned long long ResultType;
    ResultType operator()(const void* data, size_t size, ResultType seed) const;
    ResultType operator()(const char* str, ResultType seed) const;
    template <typename Char, typename Traits, typename Allocator>
    ResultType operator()(const std::basic_string<Char, Traits, Allocator>& str,
                          ResultType seed) const
    {
        return this->operator()(str.data(), str.length(), seed);
    }
};

struct FnvaHasher32
{
    typedef unsigned int ResultType;
    ResultType operator()(const void* data, size_t size, ResultType seed) const;
    ResultType operator()(const char* str, ResultType seed) const;

    template <typename Char, typename Traits, typename Allocator>
    ResultType operator()(const std::basic_string<Char, Traits, Allocator>& str,
                          ResultType seed) const
    {
        return this->operator()(str.data(), str.length(), seed);
    }
};

struct FnvaHasher64
{
    typedef unsigned long long ResultType;
    ResultType operator()(const void* data, size_t size, ResultType seed) const;
    ResultType operator()(const char* str, ResultType seed) const;
    template <typename Char, typename Traits, typename Allocator>
    ResultType operator()(const std::basic_string<Char, Traits, Allocator>& str,
                          ResultType seed) const
    {
        return this->operator()(str.data(), str.length(), seed);
    }
};

#ifndef linux
#include <limits.h>
#elif _WIN64
#define __WORDSIZE 64
#endif

#if __WORDSIZE == 64
typedef FnvHasher32 FnvHasher;
typedef FnvaHasher32 FnvaHasher;
#else
typedef FnvHasher64 FnvHasher;
typedef FnvaHasher64 FnvaHasher;
#endif

} // namespace common

#endif // COMMON_CRYPTO_HASH_FNV_H

