/**
 * @file BlockCompressionCodecNone.h
 * @brief
 * @date 2010-04-29
 */

#ifndef BLOCK_COMPRESSION_CODEC_NONE_H__
#define BLOCK_COMPRESSION_CODEC_NONE_H__

#include "thirdparty/monitor_sdk_common/compress/block_compression_codec.h"

namespace common {

class BlockCompressionCodecNone : public BlockCompressionCodec {
public:
    BlockCompressionCodecNone();

    virtual int GetType() { return NONE; }
private:
    virtual int DoDeflate(const char *input,
                          size_t input_size,
                          char *output,
                          size_t *output_size);

    virtual int DoInflate(const char *input,
                          size_t input_size,
                          char *output,
                          size_t *output_size);

};

} // namespace common



namespace intern {
DEPRECATED_BY(::common::BlockCompressionCodecNone)
typedef ::common::BlockCompressionCodecNone BlockCompressionCodecNone;
}

#endif  // BLOCK_COMPRESSION_CODEC_NONE_H__
