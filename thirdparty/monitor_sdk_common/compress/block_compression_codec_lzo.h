/**
 * @file BlockCompressionCodecLzo.h
 * @brief
 * @date 2010-05-05
 */

#ifndef BLOCK_COMPRESSION_CODEC_LZO_H__
#define BLOCK_COMPRESSION_CODEC_LZO_H__

#include "thirdparty/monitor_sdk_common/compress/lzo/minilzo.h"
#include "thirdparty/monitor_sdk_common/compress/block_compression_codec.h"

namespace common {

class BlockCompressionCodecLzo : public BlockCompressionCodec {

    public:
        BlockCompressionCodecLzo();
        virtual ~BlockCompressionCodecLzo();

        virtual int GetType() { return LZO; }

    private:
        virtual int DoDeflate(const char *input,
                              size_t input_size,
                              char *output,
                              size_t *output_size);

        virtual int DoInflate(const char *input,
                              size_t input_size,
                              char *output,
                              size_t *output_size);

        void ResizeTempBuffer(lzo_uint size);

    private:
        lzo_uint m_temp_outbuf_size;
        lzo_bytep m_temp_outbuf;
        uint8_t *m_workmem;
};

} // namespace common



namespace intern {
DEPRECATED_BY(::common::BlockCompressionCodecLzo)
typedef ::common::BlockCompressionCodecLzo BlockCompressionCodecLzo;
}

#endif  // BLOCK_COMPRESSION_CODEC_LZO_H__

