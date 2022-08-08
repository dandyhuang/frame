// Copyright (c) 2013, Vivo Inc.
// All rights reserved.
//
// Created: 2013-03-22

#ifndef COMMON_CRYPTO_HASH_CRYPTO_HASH_BASE_H
#define COMMON_CRYPTO_HASH_CRYPTO_HASH_BASE_H
#pragma once

#include <string.h>
#include <string>
#include "thirdparty/monitor_sdk_common/base/static_assert.h"
#include "thirdparty/monitor_sdk_common/base/stdint.h"
#include "thirdparty/monitor_sdk_common/encoding/hex.h"

namespace common {

// This class template defined all public methods for MD4, MD5, SHA1, etc.
// All crypto hash algorithm share the same interface.
template <typename Algorithm>
class CryptoHashBase
{
public:
    // Init or Reset the digester
    void Init()
    {
        static_cast<Algorithm*>(this)->DoInit();
    }

    // Accumulate data into the digester
    void Update(const void* data, size_t size)
    {
        static_cast<Algorithm*>(this)->DoUpdate(data, size);
    }

    void Update(const char* str)
    {
        Update(str, strlen(str));
    }

    // Overloading for std::string
    void Update(const std::string& str)
    {
        Update(str.data(), str.size());
    }

    // Obtain the result
    void Final(void* result)
    {
        static_cast<Algorithm*>(this)->DoFinal(result);
    }

    //////////////////////////////////////////////////////////////////////////
    // Overloading for capture buffer size error at compile time.
    template <typename ResultElementType, size_t Size>
    void Final(ResultElementType (*buffer)[Size])
    {
        COMMON_STATIC_ASSERT(sizeof(*buffer) >= Algorithm::kDigestLength);
        Final(static_cast<void*>(buffer));
    }

    // Need non-const reference here to catpure the following bad case
    // unsigned char digest[19];
    // sha1.Final(digest); // implicit array to pointer cast
    template <typename ResultElementType, size_t Size>
    void Final(ResultElementType (&buffer)[Size])
    {
        COMMON_STATIC_ASSERT(sizeof(buffer) >= Algorithm::kDigestLength);
        Final(static_cast<void*>(&buffer[0]));
    }

    // Return the digest as hex string
    std::string HexFinal()
    {
        unsigned char digest[Algorithm::kDigestLength];
        Final(digest);
        return Hex::EncodeBuffer(digest, sizeof(digest));
    }

    ///////////////////////////////////////////////////////////////////////////
    // Static methods

    static void Digest(const void* data, size_t size, void* result)
    {
        Algorithm::DoDigest(data, size, result);
    }

    // Overloading for compile time buffer size check
    template <typename ResultElementType, size_t ResultSize>
    static void Digest(
        const void* buffer,
        size_t size,
        ResultElementType (*result)[ResultSize]
        )
    {
        COMMON_STATIC_ASSERT(sizeof(*result) >= Algorithm::kDigestLength);
        Digest(buffer, size, static_cast<void*>(result));
    }

    // Overloading for compile time buffer size check
    template <typename ResultElementType, size_t ResultSize>
    static void Digest(
        const void* buffer,
        size_t size,
        ResultElementType (&result)[ResultSize]
        )
    {
        COMMON_STATIC_ASSERT(sizeof(result) >= Algorithm::kDigestLength);
        Digest(buffer, size, static_cast<void*>(result));
    }

    template <typename ResultType>
    static void Digest(const char* str, ResultType result)
    {
        Digest(str, strlen(str), result);
    }

    template <typename ResultType>
    static void Digest(const std::string& str, ResultType result)
    {
        Digest(str.data(), str.size(), result);
    }

    // Digest to be uint32 for bytes
    static uint32_t Digest32(const void* data, size_t size)
    {
        return DigestToInteger<uint32_t>(data, size);
    }

    // Digest to be uint32 for '\0' terminated string
    static uint32_t Digest32(const char* str)
    {
        return DigestToInteger<uint32_t>(str, strlen(str));
    }

    // Digest to be uint32 for std::string
    static uint32_t Digest32(const std::string& str)
    {
        return DigestToInteger<uint32_t>(str.data(), str.size());
    }

    // Digest to be uint64 for bytes
    static uint64_t Digest64(const void* data, size_t size)
    {
        return DigestToInteger<uint64_t>(data, size);
    }

    // Digest to be uint64 for '\0' terminated string
    static uint64_t Digest64(const char* str)
    {
        return DigestToInteger<uint64_t>(str, strlen(str));
    }

    // Digest to be uint64 for std::string
    static uint64_t Digest64(const std::string& str)
    {
        return DigestToInteger<uint64_t>(str.data(), str.size());
    }

    // Digest to be hex string for bytes
    static std::string HexDigest(const void* data, size_t size)
    {
        char digest[Algorithm::kDigestLength];
        Digest(data, size, digest);
        return Hex::EncodeBuffer(digest, sizeof(digest));
    }

    // Digest to be hex string for '\0' terminated string
    static std::string HexDigest(const char* data)
    {
        return HexDigest(data, strlen(data));
    }

    // Digest to be hex string for std::string
    static std::string HexDigest(const std::string& str)
    {
        return HexDigest(str.data(), str.size());
    }

private:
    template <typename T>
    static T DigestToInteger(const void* data, size_t size)
    {
        T digest[Algorithm::kDigestLength/sizeof(T)];
        Digest(data, size, digest);
        return digest[0];
    }
};

} // namespace common

#endif // COMMON_CRYPTO_HASH_CRYPTO_HASH_BASE_H
