// Copyright (c) 2008, Vivo Inc.
// All rights reserved.
//

#include "thirdparty/monitor_sdk_common/crypto/hash/md4.h"

#ifdef _WIN32

#include <thirdparty/monitor_sdk_common/base/common_windows.h>


namespace common {

HMODULE hModule = LoadLibraryA("advapi32");

typedef void WINAPI MD4InitType(MD4_CTX* context);
MD4InitType* MD4Init = reinterpret_cast<MD4InitType*>(GetProcAddress(hModule, "MD4Init"));

typedef void WINAPI MD4UpdateType(
    MD4_CTX* context, const unsigned char* input, unsigned int inlen
);
MD4UpdateType* MD4Update = reinterpret_cast<MD4UpdateType*>(GetProcAddress(hModule, "MD4Update"));

typedef void WINAPI MD4FinalType(MD4_CTX* context);
MD4FinalType* MD4Final = reinterpret_cast<MD4FinalType*>(GetProcAddress(hModule, "MD4Final"));


void MD4Implement::DoInit()
{
    MD4Init(&m_ctx);
}

void MD4Implement::DoUpdate(const void* data, size_t size)
{
    MD4Update(&m_ctx, reinterpret_cast<const unsigned char*>(data), size);
}

void MD4Implement::DoFinal(void* digest)
{
    MD4Final(&m_ctx);
    memcpy(digest, m_ctx.digest, sizeof(m_ctx.digest));
}

void MD4Implement::DoDigest(const void* data, size_t size, void* digest)
{
    MD4_CTX ctx;
    MD4Init(&ctx);
    MD4Update(&ctx, reinterpret_cast<const unsigned char*>(data), size);
    MD4Final(&ctx);
    memcpy(digest, ctx.digest, sizeof(ctx.digest));
}

} // namespace common
#endif

