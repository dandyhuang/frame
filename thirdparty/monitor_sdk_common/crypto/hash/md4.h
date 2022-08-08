// Copyright (c) 2011, Vivo Inc.
// All rights reserved.
//

// GLOBAL_NOLINT(runtime/int)
// GLOBAL_NOLINT(runtime/references)

#ifndef COMMON_CRYPTO_HASH_MD4_H
#define COMMON_CRYPTO_HASH_MD4_H

#include <stddef.h>
#include <string.h>
#include <string>
#include "thirdparty/monitor_sdk_common/base/deprecate.h"
#include "thirdparty/monitor_sdk_common/crypto/hash/crypto_hash_base.h"

namespace common {
#ifdef _WIN32


///////////////////////////////////////////////////////////////////////////////
// Windows implementation, using advapi

typedef struct {
    unsigned long i[2];
    unsigned long buf[4];
    unsigned char in[64];
    unsigned char digest[16];
} MD4_CTX;

class MD4Implement
{
public:
    static const size_t kDigestLength = 16;
    DEPRECATED_BY(kDigestLength) static const size_t DigestLength = 16;
protected:
    MD4Implement() {}
    ~MD4Implement() {}
    void DoInit();
    void DoUpdate(const void* data, size_t size);
    void DoFinal(void* digest);
    static void DoDigest(const void* data, size_t size, void* digest);
private:
    MD4_CTX m_ctx;
};


#else

///////////////////////////////////////////////////////////////////////////////
// other platforms, using openssl

#include "thirdparty/monitor_sdk_common/crypto/hash/openssl/md4.h"


class MD4Implement
{
public:
    static const size_t kDigestLength = MD4_DIGEST_LENGTH;
    DEPRECATED_BY(kDigestLength) static const size_t DigestLength = kDigestLength;
protected:
    MD4Implement() {}
    ~MD4Implement() {}
    void DoInit()
    {
        MD4_Init(&m_ctx);
    }
    void DoUpdate(const void* data, size_t size)
    {
        MD4_Update(&m_ctx, data, size);
    }
    void DoFinal(void* digest)
    {
        MD4_Final(reinterpret_cast<unsigned char*>(digest), &m_ctx);
    }
    static void DoDigest(const void* data, size_t size, void* digest)
    {
        MD4Implement md4;
        md4.DoInit();
        md4.DoUpdate(data, size);
        md4.DoFinal(digest);
    }
private:
    MD4_CTX m_ctx;
};


#endif


/// See CryptoHashBase for more interfaces
class MD4 : public MD4Implement, public common::CryptoHashBase<class MD4>
{
public:
    MD4()
    {
        Init();
    }
public:
    // Introduce base class implementation
    using common::CryptoHashBase<MD4>::Digest;

    //////////////////////////////////////////////////////////////////////////
    // All following method are deprecated, you should call the methods defined
    // in CryptoHashBase.

    DEPRECATED_BY(result = Digest32(data, size))
    static void Digest(const void* data, size_t size, unsigned int& result)
    {
        result = Digest32(data, size);
    }

#ifdef _LP64
    DEPRECATED_BY(result = Digest64(data, size))
#else
    DEPRECATED_BY(result = Digest32(data, size))
#endif
    static void Digest(const void* data, size_t size, unsigned long& result)
    {
        if (sizeof(result) == 8)
            result = Digest64(data, size);
        else
            result = Digest32(data, size);
    }

    DEPRECATED_BY(result = Digest64(data, size))
    static void Digest(const void* data, size_t size, unsigned long long& result)
    {
        result = Digest64(data, size);
    }

    DEPRECATED_BY(result = Digest32(str))
    static void Digest(const char* str, unsigned int& result)
    {
        result = Digest32(str);
    }

#ifdef _LP64
    DEPRECATED_BY(result = Digest64(str))
#else
    DEPRECATED_BY(result = Digest32(str))
#endif
    static void Digest(const char* str, unsigned long& result)
    {
        if (sizeof(result) == 8)
            result = Digest64(str);
        else
            result = Digest32(str);
    }

    DEPRECATED_BY(result = Digest64(str))
    static void Digest(const char* str, unsigned long long& result)
    {
        result = Digest64(str);
    }

    DEPRECATED_BY(HexDigest)
    static std::string DigestAsHexString(const void* data, size_t size)
    {
        return HexDigest(data, size);
    }

    DEPRECATED_BY(HexDigest)
    static std::string DigestAsHexString(const char* str)
    {
        return HexDigest(str);
    }

private:
    // All CryptoHashBase call the following methods.
    friend class common::CryptoHashBase<MD4>;
    using MD4Implement::DoDigest;
    using MD4Implement::DoInit;
    using MD4Implement::DoUpdate;
    using MD4Implement::DoFinal;
};

} // namespace common

#endif // COMMON_CRYPTO_HASH_MD4_H
