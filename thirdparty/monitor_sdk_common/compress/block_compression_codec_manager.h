/**
 * @file  BlockCompressionCodecManager.h
 * @brief ѹ�����������
 * @version 1.0
 * @date    2010-07-12 20ʱ57��46��
 */

#ifndef BLOCK_COMPRESSION_CODEC_MANAGER_H__
#define BLOCK_COMPRESSION_CODEC_MANAGER_H__

#include "thirdparty/monitor_sdk_common/system/concurrency/mutex.h"
#include "thirdparty/monitor_sdk_common/compress/block_compression_codec.h"

namespace common {

class BlockCompressionCodecManager {
public:
    BlockCompressionCodecManager(int max_free_codec = 5);
    ~BlockCompressionCodecManager();

    BlockCompressionCodec *CreateBlockCompressionCodec(int codec_type);
    void DestroyBlockCompressionCodec(BlockCompressionCodec *codec);

private:
    // ÿ�����͵�ѹ������ĳ�פ�ڴ���������
    int m_max_free_codec_size;
    int m_free_codec_size_array[BlockCompressionCodec::COMPRESSION_TYPE_LIMIT];

    // ѹ�������������ά����
    BlockCompressionCodec **m_codec_array;
    SimpleMutex m_mutex;
};

} // namespace common



namespace intern {
DEPRECATED_BY(::common::BlockCompressionCodecManager)
typedef ::common::BlockCompressionCodecManager BlockCompressionCodecManager;
}

#endif  // BLOCK_COMPRESSION_CODEC_MANAGER_H__
