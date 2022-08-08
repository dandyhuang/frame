/**
 * @file BlockCompressionCodecNone.cc
 * @brief
 * @date 2010-04-29
 */

#include "string.h"
#include "thirdparty/monitor_sdk_common/compress/block_compression_codec_none.h"

namespace common {

BlockCompressionCodecNone::BlockCompressionCodecNone() { }

int BlockCompressionCodecNone::DoDeflate(const char *input,
                                         size_t input_size,
                                         char *output,
                                         size_t *output_size) {
    if (input_size > *output_size) {
        return COMPRESSION_E_OUTPUT_OVERRUN;
    }

    memcpy(output, input, input_size);

    *output_size = input_size;

    return COMPRESSION_E_OK;
}

int BlockCompressionCodecNone::DoInflate(const char *input,
                                         size_t input_size,
                                         char *output,
                                         size_t *output_size) {
    return DoDeflate(input, input_size, output, output_size);
}

} // namespace common



