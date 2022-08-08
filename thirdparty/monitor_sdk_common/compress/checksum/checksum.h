/**
 * @file checksum.h
 * @brief
 * @date 2010-04-29
 */

#ifndef COMMON_COMPRESS_CHECKSUM_H
#define COMMON_COMPRESS_CHECKSUM_H

#include <stdlib.h>
#include <stddef.h>
#include "thirdparty/monitor_sdk_common/base/stdint.h"

namespace common
{

/** Compute fletcher32 checksum for arbitary data
 *
 * @param data - input data
 * @param len - input data length in bytes
 */
extern uint32_t
fletcher32(const void *data, size_t len);

/** Compute fletcher32 checksum for 16-bit aligned and padded data
 *  slightly faster than fletcher32
 *
 * @param data - input data
 * @param len - input data length in bytes
 */
extern uint32_t
fletcher32a(const void *data, size_t len);

/** Compute adler32 checksum
 *
 * @param data - input data
 * @param len - input data length in bytes
 */
extern uint32_t
adler32(const void *data, size_t len);

/** Update adler32 checksum incrementally
 *
 * @param adler - current adler32 checksum
 * @param data - input data
 * @param len - input data length in bytes
 */
extern uint32_t
adler32_update(uint32_t adler, const void *data, size_t len);

/** Compute crc32 checksum
 *
 * @param data - input data
 * @param len - input data length in bytes
 */
extern uint32_t
crc32(const void *data, size_t len);

/** Update crc32 checksum incrementally
 *
 * @param crc - current crc32 checksum
 * @param data - input data
 * @param len - input data length in bytes
 */
extern uint32_t
crc32_update(uint32_t crc, const void *data, size_t len);

} // namespace common

namespace intern {
using common::fletcher32;
using common::fletcher32a;
using common::adler32;
using common::adler32_update;
using common::crc32;
using common::crc32_update;
}

#endif // COMMON_COMPRESS_CHECKSUM_H
