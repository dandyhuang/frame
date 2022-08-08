// // Copyright (c) 2011, Vivo Inc.
// All rights reserved.
//
// Created: 08/12/11
// Description:

#ifndef COMMON_COMPRESS_BLOCK_COMPRESSION_CODEC_GZIP_H_
#define COMMON_COMPRESS_BLOCK_COMPRESSION_CODEC_GZIP_H_

#include "thirdparty/monitor_sdk_common/compress/block_compression_codec.h"

namespace common
{

class BlockCompressionCodecGzip : public BlockCompressionCodec
{
public:
    virtual int GetType() { return GZIP; }

private:
    // see document of inflateInit2 in /usr/include/zlib.h
    static const int ZLIB_MAX_WINDOW_BITS = 15;
    static const int ZLIB_AUTO_HEADER_DETECT = 32;

    virtual int DoDeflate(const char *input,
            size_t input_size,
            char* output,
            size_t *output_size);

    virtual int DoInflate(const char *input,
            size_t input_size,
            char* output,
            size_t *output_size);
};

} // namespace common



namespace intern {
DEPRECATED_BY(::common::BlockCompressionCodecGzip)
typedef ::common::BlockCompressionCodecGzip BlockCompressionCodecGzip;
}

#endif // COMMON_COMPRESS_BLOCK_COMPRESSION_CODEC_GZIP_H_
