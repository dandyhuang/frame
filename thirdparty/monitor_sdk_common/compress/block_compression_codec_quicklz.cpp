/**
 * @file BlockCompressionCodecQuicklz.cc
 * @brief
 * @date 2010-05-05
 */

#include "thirdparty/monitor_sdk_common/compress/quicklz/quicklz.h"
#include "thirdparty/monitor_sdk_common/compress/block_compression_codec_quicklz.h"

namespace common {

BlockCompressionCodecQuicklz::BlockCompressionCodecQuicklz() {
    size_t amount = ((QLZ_SCRATCH_DECOMPRESS) < (QLZ_SCRATCH_COMPRESS))
        ? (QLZ_SCRATCH_COMPRESS) : (QLZ_SCRATCH_DECOMPRESS);
    m_workmem = new uint8_t[amount];
}


BlockCompressionCodecQuicklz::~BlockCompressionCodecQuicklz() {
    delete [] m_workmem;
}

int BlockCompressionCodecQuicklz::DoDeflate(const char *input,
                                            size_t input_size,
                                            char *output,
                                            size_t *output_size) {
    *output_size = qlz_compress((char *)input,
                                output,
                                input_size,
                                (char *)m_workmem);

    return COMPRESSION_E_OK;
}

int BlockCompressionCodecQuicklz::DoInflate(const char *input,
                                            size_t input_size,
                                            char *output,
                                            size_t *output_size) {
    size_t len = qlz_decompress((char *)input,
                                output,
                                (char *)m_workmem);

    if (len > *output_size) {
        return COMPRESSION_E_UNPACK_LEN_ERROR;
    }

    *output_size = len;
    return COMPRESSION_E_OK;
}

} // namespace common


