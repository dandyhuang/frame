/**
 * @file BlockCompressionCodecZlib.cc
 * @brief
 * @date 2010-04-29
 */

#include <string.h>

#include "thirdparty/monitor_sdk_common/base/stdint.h"
#include "thirdparty/monitor_sdk_common/compress/checksum/checksum.h"
#include "thirdparty/monitor_sdk_common/compress/block_compression_codec_zlib.h"

namespace common {

BlockCompressionCodecZlib::BlockCompressionCodecZlib() : m_level(Z_BEST_SPEED) {
}

BlockCompressionCodecZlib::~BlockCompressionCodecZlib() {}

bool BlockCompressionCodecZlib::SetArgs(const char *name, const char *value) {
    bool set_level = false;
    if ((0 == strcasecmp(name, "base") || 0 == strcasecmp(name, "9"))
        && (m_level != Z_BEST_SPEED)) {
        set_level = true;
        m_level = Z_BEST_SPEED;
    } else if (0 == strcasecmp(name, "normal") &&
               m_level != Z_DEFAULT_COMPRESSION) {
        set_level = true;
        m_level = Z_DEFAULT_COMPRESSION;
    }
    return set_level;
}


int BlockCompressionCodecZlib::DoDeflate(const char *input,
                                         size_t input_size,
                                         char *output,
                                         size_t *output_size) {
    // zlib docs in http://www.zlib.net/zlib_tech.html
    int ret;
    z_stream c_stream;
    memset(&c_stream, 0, sizeof(c_stream));

    ret = deflateInit(&c_stream, m_level);
    if(ret != Z_OK){
        return COMPRESSION_E_PACK_ERROR;
    }

    c_stream.avail_in = input_size;
    c_stream.next_in = (Bytef*)input;

    c_stream.avail_out = (*output_size);
    c_stream.next_out = (Bytef*)(output);

    ret = ::deflate(&c_stream, Z_FINISH);
    *output_size = c_stream.total_out;
    deflateEnd(&c_stream);

    if (ret != Z_STREAM_END) {
        return COMPRESSION_E_PACK_ERROR;
    }

    return COMPRESSION_E_OK;
}

int BlockCompressionCodecZlib::DoInflate(const char *input,
                                         size_t input_size,
                                         char *output,
                                         size_t *output_size) {
    int ret;
    z_stream d_stream;
    memset(&d_stream, 0, sizeof(d_stream));

    ret = inflateInit(&d_stream);
    if (ret != Z_OK) {
        return COMPRESSION_E_UNPACK_ERROR;
    }

    d_stream.avail_in = input_size;
    d_stream.next_in = (Bytef *)input;

    d_stream.avail_out = *output_size;
    d_stream.next_out = (Bytef *)output;

    ret = ::inflate(&d_stream, Z_NO_FLUSH);
    *output_size = d_stream.total_out;
    inflateEnd(&d_stream);

    if (ret != Z_STREAM_END) {
        return COMPRESSION_E_UNPACK_ERROR;
    }

    return COMPRESSION_E_OK;
}

} // namespace common


