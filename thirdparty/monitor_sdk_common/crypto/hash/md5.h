// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#ifndef COMMON_CRYPTO_HASH_MD5_H
#define COMMON_CRYPTO_HASH_MD5_H

#include <string>

#include "thirdparty/monitor_sdk_common/base/int128.h"
#include "thirdparty/monitor_sdk_common/base/string/string_piece.h"
#include "thirdparty/monitor_sdk_common/base/uncopyable.h"

// MD5 stands for Message Digest algorithm 5.
// MD5 is a robust hash function, designed for cyptography, but often used
// for file checksums.  The code is complex and slow, but has few
// collisions.
// See Also:
//   http://en.wikipedia.org/wiki/MD5

namespace common {

struct Context;

class MD5 {
    COMMON_DECLARE_UNCOPYABLE(MD5);

public:
    MD5();
    ~MD5();

    //  Init is called in constructor, but if you want to use the same object
    //  for many times, you SHOULD call Init before computing md5 of new data.
    void Init();

    // For the given buffer of data, updates the given MD5 context with the sum of
    // the data. You can call this any number of times during the computation,
    // except that MD5Init() must have been called first.
    void Update(StringPiece sp);

    // Finalizes the MD5 operation and fills the buffer with the digest.
    UInt128 Final();
    // Finalizes the MD5 operation and fills the buffer with the digest.
    void Final(void* digest);
    //  Hex encoding for result
    std::string HexFinal();

    static UInt128 Digest(StringPiece sp);

    static uint64_t Digest64(StringPiece sp);

    static std::string HexDigest(StringPiece sp);

private:
    struct Context {
        uint32_t buf[4];
        uint32_t bits[2];
        unsigned char in[64];
    };

    void FinalInternal();

    Context context_;
};
}  // namespace common
#endif  // COMMON_CRYPTO_HASH_MD5_H

