/**
 * @file BlockCompressionCodecLzo.cc
 * @brief
 * @date 2010-05-05
 */
#include <string.h>
#include <assert.h>
#include "thirdparty/monitor_sdk_common/compress/block_compression_codec_lzo.h"

namespace common {

BlockCompressionCodecLzo::BlockCompressionCodecLzo()
    : m_temp_outbuf_size(0), m_temp_outbuf(NULL) {
    assert(lzo_init() == LZO_E_OK);
    m_workmem = new uint8_t[LZO1X_1_MEM_COMPRESS];
}

BlockCompressionCodecLzo::~BlockCompressionCodecLzo() {
    delete [] m_temp_outbuf;
    m_temp_outbuf = NULL;
    m_temp_outbuf_size = 0;

    delete [] m_workmem;
    m_workmem = NULL;
}

void BlockCompressionCodecLzo::ResizeTempBuffer(lzo_uint  size) {
    if (size > m_temp_outbuf_size) {
        delete [] m_temp_outbuf;
        m_temp_outbuf = new lzo_byte[size];
        m_temp_outbuf_size = size;
    }
}

int BlockCompressionCodecLzo::DoDeflate(const char *input,
                                        size_t input_size,
                                        char *output,
                                        size_t *output_size) {

    //I still haven't computed the exact values, but I suggest using
    //these formulas for a worst-case expansion calculation:
    //see formula in doc [http://www.oberhumer.com/opensource/lzo/lzofaq.php]
    lzo_uint lzo_output_size = input_size + (input_size / 16) + 64 + 3;

    int ret = COMPRESSION_E_OK;

    //if outbuf is no enough then using the temp outbuf
    if (lzo_output_size > *output_size) {
        ResizeTempBuffer(lzo_output_size);
        ret = lzo1x_1_compress((const uint8_t *)input,
                               input_size,
                               m_temp_outbuf,
                               &lzo_output_size,
                               m_workmem);

        if (ret != LZO_E_OK) {
            return COMPRESSION_E_PACK_ERROR;
        }

        if (lzo_output_size > *output_size) {
            return COMPRESSION_E_OUTPUT_OVERRUN;
        }
        memcpy(output, m_temp_outbuf, *output_size);

    } else {
        ret = lzo1x_1_compress((const uint8_t *)input,
                               input_size,
                               (uint8_t *)output,
                               &lzo_output_size,
                               m_workmem);

        if (ret != LZO_E_OK) {
            return COMPRESSION_E_PACK_ERROR;
        }
    }

    *output_size = lzo_output_size;
    return COMPRESSION_E_OK;
}

int BlockCompressionCodecLzo::DoInflate(const char *input,
                                        size_t input_size,
                                        char *output,
                                        size_t *output_size) {
    lzo_uint lzo_output_size = *output_size;
    int ret = lzo1x_decompress((const uint8_t *)input,
                               input_size,
                               (uint8_t *)output,
                               &lzo_output_size,
                               0);
    *output_size = lzo_output_size;

    if (ret != LZO_E_OK) {
        return COMPRESSION_E_UNPACK_ERROR;
    }

    return COMPRESSION_E_OK;
}

} // namespace common


