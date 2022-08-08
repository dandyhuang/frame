// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#include "thirdparty/monitor_sdk_common/hash/fingerprint.h"
#include "thirdparty/monitor_sdk_common/hash/murmur.h"

#include "thirdparty/stringencoders/modp_b16.h"

namespace common {

namespace {
static const uint64_t kFingerPrintSeed = 19820125;
}

uint64_t Fingerprint64(const std::string& str) {
    return MurmurHash64A(str, kFingerPrintSeed);
}

std::string Fingerprint64ToString(uint64_t fp) {
    // NOLINT(runtime/sizeof)
    return modp::b16_encode(reinterpret_cast<char*>(&fp), sizeof(uint64_t));
}

uint64_t StringToFingerprint64(const std::string& str) {
    std::string tmp = modp::b16_decode(str);
    uint64_t value = 0;
    memcpy(&value, tmp.data(), sizeof(uint64_t)); // NOLINT(runtime/sizeof)
    return value;
}

}  // namespace common
