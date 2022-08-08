/**
 * @file BlockCompressionCodecBmz.h
 * @brief
 * @date 2010-04-29
 */

#ifndef BLOCK_COMPRESSION_CODEC_BMZ_H__
#define BLOCK_COMPRESSION_CODEC_BMZ_H__

#include "thirdparty/monitor_sdk_common/compress/block_compression_codec.h"

namespace common {

class BlockCompressionCodecBmz : public BlockCompressionCodec {
public:
    BlockCompressionCodecBmz();
    virtual ~BlockCompressionCodecBmz();


    virtual bool SetArgs(const char *name, const char *value);
    virtual int GetType() { return BMZ; }

private:
    virtual int DoDeflate(const char *input,
                          size_t input_size,
                          char *output,
                          size_t *output_size);

    virtual int DoInflate(const char *input,
                          size_t input_size,
                          char *output,
                          size_t *output_size);

    char *m_workmem;
    size_t m_workmem_size;
    size_t m_fp_len;
};

} // namespace common



namespace intern {
DEPRECATED_BY(::common::BlockCompressionCodecBmz)
typedef ::common::BlockCompressionCodecBmz BlockCompressionCodecBmz;
}

#endif  // BLOCK_COMPRESSION_CODEC_BMZ_H__
