// Copyright (c) 2011, Vivo Inc. All rights reserved.
// ��������װ
// �㷨˵����
//          ����ժҪ(MD��MessageDigest�������ǽ��ɱ䳤�ȵı���M��Ϊ����ɢ�к������룬Ȼ���
//          ��һ���̶����ȵı�־H(M)��H(M)ͨ����Ϊ����ժҪ(MD)������Ҫ�������������
//          ͨ��˫������һ���������Կ�����Ͷ��Ƚ�����M�����ɢ�к���H�������H��M����MD��
//          ���ó������Կ��MD���м��ܣ������ܵ�MD׷���ڱ���M�ĺ��棬���͵����ܶˡ����ն���
//          ��ȥ׷���ڱ���M������ܵ�MD������֪��ɢ�к�������H(M)�������Լ�ӵ�е���ԿK�Լ���
//          ��MD���ܶ��ó���ʵ��MD���Ƚϼ������H(M)��MD����һ�£����յ��ı���M����ġ�

// GLOBAL_NOLINT(runtime/int)
// GLOBAL_NOLINT(runtime/references)
// GLOBAL_NOLINT(runtime/printf)

#include <stdio.h>
#include <string.h>

#include "thirdparty/monitor_sdk_common/base/inttypes.h"
#include "thirdparty/monitor_sdk_common/crypto/hash/md5_2.h"

#ifdef _WIN32
#include <thirdparty/monitor_sdk_common/base/common_windows.h>
#endif

namespace common {

#ifdef _WIN32

namespace {

static HMODULE hDLL = LoadLibraryA("advapi32.dll");

typedef void (WINAPI* MD5InitType)(MD5_CTX* context);

typedef void (WINAPI* MD5UpdateType)(
    MD5_CTX* context,
    const unsigned char* input,
    unsigned int inlen);

typedef void (WINAPI* MD5FinalType)(MD5_CTX* context);

static MD5InitType MD5Init = reinterpret_cast<MD5InitType>(GetProcAddress(hDLL, "MD5Init"));
static MD5UpdateType MD5Update = reinterpret_cast<MD5UpdateType>(GetProcAddress(hDLL, "MD5Update"));
static MD5FinalType MD5Final = reinterpret_cast<MD5FinalType>(GetProcAddress(hDLL, "MD5Final"));

int MD5_Init(MD5_CTX *c)
{
    MD5Init(c);
    return 0;
}

int MD5_Update(MD5_CTX *c, const void *data, size_t len)
{
    MD5Update(c, (const unsigned char*) data, (unsigned int) len);
    return 0;
}

int MD5_Final(unsigned char *md, MD5_CTX *c)
{
    MD5Final(c);
    memcpy(md, c->digest, sizeof(c->digest));
    return 0;
}

} // namespace

#endif

void MD5::DoInit()
{
    MD5_Init(&m_ctx);
}

void MD5::DoUpdate(const void* data, size_t size)
{
    MD5_Update(&m_ctx, data, size);
}

void MD5::DoFinal(void* digest)
{
    MD5_Final(reinterpret_cast<unsigned char*>(digest), &m_ctx);
}

void MD5::DoDigest(const void *data, size_t size, void *digest)
{
    MD5 md5;
    md5.Update(data, size);
    md5.Final(digest);
}

// ���ⲿʹ�õĹ�ϣ����������16bytes��hashֵ
void MD5::Hash(const char* str, void* hash_value)
{
    Digest(str, ::strlen(str), hash_value);
}

size_t MD5::Hash64String(const char* str, char* hash_value)
{
    uint64_t digest = Digest64(str);
    return sprintf(hash_value, "%" PRIu64, digest);
}

std::string MD5::GetHash64String(const char* str)
{
    uint64_t digest = Digest64(str);
    char buffer[sizeof("18446744073709551615")]; // UINT64_MAX
    int length = sprintf(buffer, "%" PRIu64, digest);
    return std::string(buffer, length);
}

size_t MD5::Hash64HexString(const char* str, char* hash_value)
{
    uint64_t digest = Digest64(str);
    return sprintf(hash_value, "%" PRIx64, digest);
}

std::string MD5::GetHash64HexString(const char* str)
{
    char buffer[sizeof("FFFFFFFFFFFFFFFF")]; // UINT64_MAX
    uint64_t digest = Digest64(str);
    return std::string(buffer, sprintf(buffer, "%" PRIx64, digest));
}

// ���ⲿʹ�õ�64λ��ϣ����������long long��hashֵ
void MD5::Hash64(const char* str, long long& hash_value)
{
    hash_value = Digest64(str);
}

// ���ⲿʹ�õ�64λ��ϣ����������unsigned long long��hashֵ
void MD5::Hash64(const char* str, unsigned long long& hash_value)
{
    hash_value = Digest64(str);
}

// ���ⲿʹ�õ�64λ��ϣ����������16bytes��hashֵ
void MD5::Hash(const void* data, size_t size, void* hash_value)
{
    Digest(data, size, hash_value);
}

// ���ⲿʹ�õ�64λ��ϣ����������long long��hashֵ
void MD5::Hash64(const void* data, size_t size, long long& hash_value)
{
    hash_value = Digest64(data, size);
}

// ���ⲿʹ�õ�64λ��ϣ����������unsigned long long��hashֵ
void MD5::Hash64(const void* data, size_t size, unsigned long long& hash_value)
{
    unsigned long long digest[kDigestLength / sizeof(hash_value)];
    Digest(data, size, digest);
    hash_value = digest[0];
}

unsigned long long MD5::GetHash64(const char* str)
{
    return Digest64(str);
}

unsigned long long MD5::GetHash64(const void* data, size_t size)
{
    return Digest64(data, size);
}

#ifdef _LP64 // 64 λ��long �� 64 λ
// ���ⲿʹ�õ�64λ��ϣ����������long ��hashֵ
void MD5::Hash64(const char* str, unsigned long & hash_value)
{
    hash_value = Digest64(str);
}

// ���ⲿʹ�õ�64λ��ϣ����������unsigned long��hashֵ
void MD5::Hash64(const void* data, size_t size, unsigned long& hash_value)
{
    hash_value = Digest64(data, size);
}
#endif

} // namespace common
