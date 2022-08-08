// Copyright 2010, Vivo Inc.
//
// Defines checksum functions base on CRC algorithms.
// Refer to: http://en.wikipedia.org/wiki/Cyclic_redundancy_check

#ifndef COMMON_CRYPTO_HASH_CRC_H_
#define COMMON_CRYPTO_HASH_CRC_H_

#include <limits.h>
#include <string.h>
#include "thirdparty/monitor_sdk_common/base/stdint.h"

namespace common {

// CRC32�㷨
// ����:
//       PKZip��WinZip �� Ethernet �е�CRC�㷨,�����۽��Ľ�,�Ľ�������
// ��ע:
//       �����ڽ��в������ۼӵĳ���
//       ʵ�ֵ���CRC-32-IEEE 802.3�㷨������ʽΪ0x04C11DB7

static const uint32_t kCRC32InitValue = 0xffffffff;

uint32_t UpdateCRC32(const void* data, size_t size, uint32_t old_crc);

inline uint32_t CRC32Hash32(const void* data, size_t size) {
    return UpdateCRC32(data, size, kCRC32InitValue);
}

uint8_t  CRC16Hash8(const void* ptr, size_t size);

} // namespace common

#endif // COMMON_CRYPTO_HASH_CRC_H_
