/**
 * @file BlockCompressionCodec.h
 * @brief
 * @date 2010-04-28
 */


#ifndef BLOCK_COMPRESSION_CODEC_H__
#define BLOCK_COMPRESSION_CODEC_H__

#include <string>
#include <vector>
#include "thirdparty/monitor_sdk_common/base/stdint.h"
#include "thirdparty/monitor_sdk_common/base/deprecate.h"

namespace common {

class BlockCompressionCodec {
public:
    enum compression_type{
        UNKNOWN = -1,
        NONE = 0,
        BMZ = 1,
        ZLIB = 2,
        LZO = 3,
        QUICKLZ = 4,
        SNAPPY = 5,
        GZIP = 6,
        COMPRESSION_TYPE_LIMIT = 7
    };

    enum compression_error {
        COMPRESSION_E_OK = 0,
        COMPRESSION_E_PACK_ERROR = -1,
        COMPRESSION_E_UNPACK_ERROR = -2,
        COMPRESSION_E_INPUT_OVERRUN = -3,
        COMPRESSION_E_OUTPUT_OVERRUN = -4,
        COMPRESSION_E_PACK_LEN_ERROR = -5,
        COMPRESSION_E_UNPACK_LEN_ERROR = -6,
        COMPRESSION_E_CHECKSUM_ERROR = -7,
        COMPRESSION_E_TYPE_ERROR = -8,
    };

    BlockCompressionCodec() {}
    virtual ~BlockCompressionCodec() {}

    int Deflate(const char *input,
                size_t input_size,
                char *output,
                size_t *output_size);

    int Inflate(const char *input,
                size_t input_size,
                char *output,
                size_t *output_size);

    // The following two interfaces do extra CRC checking which is
    // computational intensive, and are being deprecated.
    //
    // NOTE(huanyu): The output_size here is reference in contrast of
    // pointer in new interface. We keep this only for backward compatiblity.
    //
    // Do NOT use them if your code is performance sensitive.
    DEPRECATED int Deflate(const char *input,
                           size_t input_size,
                           char *output,
                           size_t &output_size,
                           uint32_t &crc);

    DEPRECATED int Inflate(const char *input,
                           size_t input_size,
                           char *output,
                           size_t &output_size,
                           uint32_t crc);

    virtual bool SetArgs(const char *name, const char *value) {
        return false;
    }
    virtual int GetType() = 0;

    static BlockCompressionCodec *CreateCodec(int type);

private:
    virtual int DoDeflate(const char *input,
                          size_t input_size,
                          char *output,
                          size_t *output_size) = 0;

    virtual int DoInflate(const char *input,
                          size_t input_size,
                          char *output,
                          size_t *output_size) = 0;
};

} // namespace common



namespace intern {
DEPRECATED_BY(::common::BlockCompressionCodec)
typedef ::common::BlockCompressionCodec BlockCompressionCodec;
}

#endif  // BLOCK_COMPRESSION_CODEC_H__
