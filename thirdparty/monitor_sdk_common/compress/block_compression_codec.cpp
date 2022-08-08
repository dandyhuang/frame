/**
 * @file BlockCompressionCodec.cc
 * @brief
 * @date 2010-05-05
 */

#include "thirdparty/monitor_sdk_common/compress/block_compression_codec.h"
#include "thirdparty/monitor_sdk_common/compress/block_compression_codec_none.h"
#include "thirdparty/monitor_sdk_common/compress/block_compression_codec_bmz.h"
#include "thirdparty/monitor_sdk_common/compress/block_compression_codec_lzo.h"
#include "thirdparty/monitor_sdk_common/compress/block_compression_codec_quicklz.h"
#ifndef _WIN32
// disable on win32
#include "thirdparty/monitor_sdk_common/compress/block_compression_codec_gzip.h"
#include "thirdparty/monitor_sdk_common/compress/block_compression_codec_snappy.h"
#include "thirdparty/monitor_sdk_common/compress/block_compression_codec_zlib.h"
#endif
#include "thirdparty/monitor_sdk_common/compress/block_compression_codec_manager.h"
#include "thirdparty/monitor_sdk_common/compress/checksum/checksum.h"

namespace common {

BlockCompressionCodec *BlockCompressionCodec::CreateCodec(int type) {
    switch (type) {
        case BlockCompressionCodec::NONE:
            return new BlockCompressionCodecNone();

        case BlockCompressionCodec::BMZ:
            return new BlockCompressionCodecBmz();

        case BlockCompressionCodec::LZO:
            return new BlockCompressionCodecLzo();

        case BlockCompressionCodec::QUICKLZ:
            return new BlockCompressionCodecQuicklz();        
#ifndef _WIN32
            // disable these codec on win32
        case BlockCompressionCodec::ZLIB:
            return new BlockCompressionCodecZlib();

        case BlockCompressionCodec::GZIP:
            return new BlockCompressionCodecGzip();

        case BlockCompressionCodec::SNAPPY:
            return new BlockCompressionCodecSnappy();
#endif
        default:
            break;
    }

    return NULL;
}

int BlockCompressionCodec::Deflate(const char *input,
                                   size_t input_size,
                                   char *output,
                                   size_t &output_size,
                                   uint32_t &crc) {
    int ret = DoDeflate(input, input_size, output, &output_size);
    if (ret != COMPRESSION_E_OK) {
        return ret;
    }

#ifdef _WIN32
    // don't verify the checksum on win32
    crc = 0xffffffff;
#else
    crc = fletcher32(output, output_size);
#endif

    return COMPRESSION_E_OK;
}

int BlockCompressionCodec::Inflate(const char *input,
                                   size_t input_size,
                                   char *output,
                                   size_t &output_size,
                                   uint32_t crc) {
    // º∆À„–£—È¬Î
#ifndef _WIN32
    // don't verify the checksum on win32
    if (crc != fletcher32(input, input_size)) {
        return COMPRESSION_E_CHECKSUM_ERROR;
    }
#endif

    return DoInflate(input, input_size, output, &output_size);
}

int BlockCompressionCodec::Deflate(const char *input,
                                   size_t input_size,
                                   char *output,
                                   size_t *output_size) {
    return DoDeflate(input, input_size, output, output_size);
}

int BlockCompressionCodec::Inflate(const char *input,
                                   size_t input_size,
                                   char *output,
                                   size_t *output_size) {
    return DoInflate(input, input_size, output, output_size);
}

} // namespace common



