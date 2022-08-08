/**
 * @brief
 * @date 2011/04/29
 */

#include "thirdparty/monitor_sdk_common/compress/block_compression_codec_snappy.h"
#include <string>
#include "thirdparty/snappy-1.0.2/snappy.h"

namespace common {

BlockCompressionCodecSnappy::BlockCompressionCodecSnappy() { }

int BlockCompressionCodecSnappy::DoDeflate(const char *input,
                                           size_t input_size,
                                           char *output,
                                           size_t *output_size) {
    /// judge whether the output buffer is sufficient by MaxCompressedLength
    size_t max_len_for_compress = snappy::MaxCompressedLength(input_size);
    if (*output_size < max_len_for_compress) {
        /// use deflate-safe interface to avoid buffer overflow
        std::string compress_result;
        snappy::Compress(input, input_size, &compress_result);

        if (compress_result.length() > *output_size) {
            /// return the buffer size needed by compression to user
            *output_size = compress_result.length();
            return COMPRESSION_E_OUTPUT_OVERRUN;
        } else {
            /// copy the compressed result to output
            *output_size = compress_result.length();
            compress_result.copy(output, compress_result.length());
            return COMPRESSION_E_OK;
        }
    } else {
        // if output is sufficient, call RawCompress directly
        snappy::RawCompress(input, input_size, output, output_size);
        return COMPRESSION_E_OK;
    }
}

int BlockCompressionCodecSnappy::DoInflate(const char *input,
                                           size_t input_size,
                                           char *output,
                                           size_t *output_size) {
    size_t output_size_in_need = 0;
    if (!snappy::GetUncompressedLength(input, input_size, &output_size_in_need)) {
        /// maybe the format error of input
        return COMPRESSION_E_UNPACK_ERROR;
    } else if (output_size_in_need > *output_size) {
        /// the output is not sufficient for uncompression
        /// return the buffer size needed for uncompression to user
        *output_size = output_size_in_need;
        return COMPRESSION_E_UNPACK_LEN_ERROR;
    }

    *output_size = output_size_in_need;
    snappy::RawUncompress(input, input_size, output);
    return COMPRESSION_E_OK;
}

} // namespace common



