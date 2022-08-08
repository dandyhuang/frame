// // Copyright (c) 2011, Vivo Inc.
// All rights reserved.
//
// Created: 07/12/11
// Description:

#include <stdlib.h>
#include <zlib.h>
#include "thirdparty/monitor_sdk_common/compress/block_compression_codec_gzip.h"
namespace common
{

int BlockCompressionCodecGzip::DoDeflate(
        const char *input,
        size_t input_size,
        char *output,
        size_t *output_size)
{
    z_stream c_stream = {0};
    int err = 0;

    c_stream.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(input));
    c_stream.avail_in  = input_size;
    c_stream.next_out = reinterpret_cast<Bytef*>(output);
    c_stream.avail_out  = *output_size;

    if (input_size <= 0 || *output_size <= 0) {
        return COMPRESSION_E_PACK_ERROR;
    }

    if (deflateInit2(&c_stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED,
                ZLIB_MAX_WINDOW_BITS, 8, Z_DEFAULT_STRATEGY) != Z_OK) {
        return COMPRESSION_E_PACK_ERROR;
    }

    err = deflate(&c_stream, Z_FINISH);
    *output_size = c_stream.total_out;

    BlockCompressionCodec::compression_error compr_error;
    if (err == Z_STREAM_END) {
        compr_error = COMPRESSION_E_OK;
    } else if (c_stream.avail_out == 0) {
        compr_error = COMPRESSION_E_PACK_LEN_ERROR;
    } else {
        compr_error = COMPRESSION_E_PACK_ERROR;
    }
    deflateEnd(&c_stream);
    return compr_error;
}

int BlockCompressionCodecGzip::DoInflate(const char *input,
        size_t input_size,
        char *output,
        size_t *output_size)
{
    int err = 0;
    z_stream d_stream = {0}; /* decompression stream */

    d_stream.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(input));
    d_stream.avail_in  = input_size;
    d_stream.next_out = reinterpret_cast<Bytef*>(output);
    d_stream.avail_out  = *output_size;

    // using maximu window bits and auto header detect
    if (inflateInit2(&d_stream, ZLIB_MAX_WINDOW_BITS + ZLIB_AUTO_HEADER_DETECT) != Z_OK) {
        return COMPRESSION_E_UNPACK_ERROR;
    }

    err = inflate(&d_stream, Z_FINISH);
    *output_size = d_stream.total_out;

    BlockCompressionCodec::compression_error uncompr_error;
    if (err == Z_STREAM_END) {
        uncompr_error = COMPRESSION_E_OK;
    } else if (d_stream.avail_out == 0) {
        uncompr_error = COMPRESSION_E_UNPACK_LEN_ERROR;
    } else {
        uncompr_error = COMPRESSION_E_UNPACK_ERROR;
    }
    inflateEnd(&d_stream);
    return uncompr_error;
}

} // namespace common



