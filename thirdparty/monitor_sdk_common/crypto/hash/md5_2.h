// Copyright (c) 2008, Vivo Inc. All rights reserved.
// 版权信息：(C) 2003, Jerry.CS.HUST.China
// 作者：    Jerry

// GLOBAL_NOLINT(runtime/int)
// GLOBAL_NOLINT(runtime/references)

#ifndef COMMON_CRYPTO_HASH_MD5_H
#define COMMON_CRYPTO_HASH_MD5_H

#include <stddef.h>
#include <string.h>
#include <string>
#include "thirdparty/monitor_sdk_common/base/deprecate.h"
#include "thirdparty/monitor_sdk_common/base/static_assert.h"
#include "thirdparty/monitor_sdk_common/crypto/hash/crypto_hash_base.h"

#ifndef _WIN32
#include "thirdparty/monitor_sdk_common/crypto/hash/openssl/md5.h"
#else

// don't use windows type
namespace common {
typedef struct {
    unsigned long i[2];
    unsigned long buf[4];
    unsigned char in[64];
    unsigned char digest[16];
} MD5_CTX;
}
#endif

namespace common {

/// See CryptoHashBase for more interfaces
class MD5 : public CryptoHashBase<MD5>
{
public:
    static const size_t kDigestLength = 16;
    DEPRECATED_BY(kDigestLength) static const size_t DigestLength = 16;

public:
    MD5()
    {
        Init();
    }

public:
    //////////////////////////////////////////////////////////////////////////
    // All following method are deprecated, you should call the methods defined
    // in CryptoHashBase.

    /// 供外部使用的哈希函数：生成 16 bytes 的 hash 值
    DEPRECATED_BY(Digest) static void Hash(const char* str, void* hash_value);

    /// 供外部使用的64位哈希函数：生成 16 bytes 的 hash 值
    DEPRECATED_BY(Digest) static void Hash(const void* data, size_t size, void* hash_value);

    /// 供外部使用的 64 位哈希函数：生成 long long 的字符串形式 hash 值，缓冲区长度至少 21
    /// @return 字符串的长度，不包含末尾的 \0
    DEPRECATED_BY(NumberToString(MD5::Digest64(str), hash_value))
    static size_t Hash64String(const char* str, char* hash_value);

    /// 供外部使用的64位哈希函数：生成 unsigned long long 的字符串形式 hash 值，缓冲区长度至少 21
    DEPRECATED_BY(NumberToString(MD5::Digest64(str)))
    static std::string GetHash64String(const char* str);

    /// 供外部使用的64位哈希函数：生成 unsigned long long 的十六进制字符串形式 hash 值，
    /// 缓冲区长度至少 17
    /// @return 字符串的长度，不包含末尾的 \0
    DEPRECATED_BY(UInt64ToHexString(MD5::Digest64(str), hash_value))
    static size_t Hash64HexString(const char* str, char* hash_value);

    /// 供外部使用的64位哈希函数：生成 unsigned long long 的十六进制字符串形式 hash 值
    DEPRECATED_BY(UInt64ToHexString(MD5::Digest64(str)))
    static std::string GetHash64HexString(const char* str);

    /// 供外部使用的64位哈希函数：生成long long的hash值。
    /// 已废弃：不要用有符号数存 hash
    DEPRECATED_BY(hash_value = Digest64(str))
    static void Hash64(const char* str, long long& hash_value);

    /// 供外部使用的64位哈希函数：生成long long的hash值。
    /// 已废弃：不要用有符号数存 hash
    DEPRECATED_BY(hash_value = Digest64(data, size))
    static void Hash64(const void* data, size_t size, long long& hash_value);

    /// 供外部使用的64位哈希函数：生成unsigned long long的hash值
    DEPRECATED_BY(hash_value = Digest64(str))
    static void Hash64(const char* str, unsigned long long& hash_value);

    /// 供外部使用的64位哈希函数：生成 unsigned long long 的 hash 值
    DEPRECATED_BY(hash_value = Digest64(data, size))
    static void Hash64(const void* data, size_t size, unsigned long long& hash_value);
#ifdef _LP64 // Unix 64 位下，long 是 64 位的
    /// 供外部使用的64位哈希函数：生成 long 的 hash 值
    DEPRECATED_BY(hash_value = Digest64(str, size))
    static void Hash64(const char* str, unsigned long & hash_value);
    /// 供外部使用的64位哈希函数：生成 unsigned long 的 hash 值
    DEPRECATED_BY(hash_value = Digest64(data, size))
    static void Hash64(const void* data, size_t size, unsigned long& hash_value);
#endif
    /// 供外部使用的 64 位哈希函数：生成 long long 的 hash 值
    DEPRECATED_BY(Digest64)
    static unsigned long long GetHash64(const char* str);
    DEPRECATED_BY(Digest64)
    static unsigned long long GetHash64(const void* data, size_t size);

    DEPRECATED_BY(Digest64)
    static unsigned long long GetHash64(const std::string& str)
    {
        return Digest64(str);
    }

private:
    // All CryptoHashBase call the following methods.
    friend class CryptoHashBase<MD5>;
    void DoInit();
    void DoUpdate(const void* data, size_t size);
    void DoFinal(void* digest);
    static void DoDigest(const void *data, size_t size, void *digest);

private:
    MD5_CTX m_ctx;
};

} // end namespace common

#endif // COMMON_CRYPTO_HASH_MD5_H

