#ifndef COMMON_CRYPTO_HASH_STRING_HASHES_H
#define COMMON_CRYPTO_HASH_STRING_HASHES_H

namespace common {

unsigned int ELFhash(const char* url, unsigned int hash_size);
unsigned int HfIp(const char* url, unsigned int hash_size);
unsigned int hf(const char* url, unsigned int hash_size);

} // namespace common

#endif // COMMON_CRYPTO_HASH_STRING_HASHES_H
