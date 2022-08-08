/**
 * @file BlockCompressionCodecZlib.h
 * @brief
 * @date 2010-04-29
 */

#ifndef BLOCK_COMPRESSION_CODEC_ZLIB_H__
#define BLOCK_COMPRESSION_CODEC_ZLIB_H__

#include <zlib.h>
#include "thirdparty/monitor_sdk_common/base/stdint.h"
#include "thirdparty/monitor_sdk_common/compress/block_compression_codec.h"

namespace common {

class BlockCompressionCodecZlib: public BlockCompressionCodec {
public:
    BlockCompressionCodecZlib();
    virtual ~BlockCompressionCodecZlib();

    /**
     * @brief ����ѹ��������zlib��֧��"best","9","normal"��������
     *
     * @param name ��������
     * @param value ����ֵΪNULL
     */
    bool SetArgs(const char *args,  const char *value = NULL);

    int GetType() {return ZLIB;}

private:
    virtual int DoDeflate(const char *input,
                          size_t input_size,
                          char *output,
                          size_t *output_size);

    virtual int DoInflate(const char *input,
                          size_t input_size,
                          char *output,
                          size_t *output_size);

    int  m_level;
};

} // namespace common



namespace intern {
DEPRECATED_BY(::common::BlockCompressionCodecZlib)
typedef ::common::BlockCompressionCodecZlib BlockCompressionCodecZlib;
}

#endif  // BLOCK_COMPRESSION_CODEC_ZLIB_H__

