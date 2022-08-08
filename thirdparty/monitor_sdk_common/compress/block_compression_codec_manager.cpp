/**
 * @file  BlockCompressionCodecManager.cc
 * @brief
 * @version 1.0
 * @date    2010-07-12 21时15分30秒
 */

#include "thirdparty/monitor_sdk_common/compress/block_compression_codec_manager.h"

#include <assert.h>
#include <stdlib.h>
#include "thirdparty/monitor_sdk_common/compress/block_compression_codec_bmz.h"
#include "thirdparty/monitor_sdk_common/compress/block_compression_codec_lzo.h"
#include "thirdparty/monitor_sdk_common/compress/block_compression_codec_none.h"
#include "thirdparty/monitor_sdk_common/compress/block_compression_codec_quicklz.h"
#include "thirdparty/monitor_sdk_common/compress/block_compression_codec_snappy.h"
#include "thirdparty/monitor_sdk_common/compress/block_compression_codec_zlib.h"

namespace common {

BlockCompressionCodecManager::BlockCompressionCodecManager(int max_free_codec)
    : m_max_free_codec_size(max_free_codec) {
    if (m_max_free_codec_size  < 0) {
        m_max_free_codec_size = 1;
    }
    for (int i = 0; i < BlockCompressionCodec::COMPRESSION_TYPE_LIMIT; i++) {
        m_free_codec_size_array[i] = 0;
    }

    int msize = BlockCompressionCodec::COMPRESSION_TYPE_LIMIT *
        m_max_free_codec_size * sizeof(BlockCompressionCodec*);
    m_codec_array = (BlockCompressionCodec**)malloc(msize);

    assert(m_codec_array != NULL);
    memset(m_codec_array, 0, msize);
}

BlockCompressionCodecManager::~BlockCompressionCodecManager() {
    BlockCompressionCodec **p_codec = m_codec_array;

    for (int i = 0; i < BlockCompressionCodec::COMPRESSION_TYPE_LIMIT; i++) {
        for (int j = 0; j < m_free_codec_size_array[i]; j++) {
            delete p_codec[j];
        }
        p_codec += m_max_free_codec_size;
    }

    free(m_codec_array);
}

BlockCompressionCodec*
BlockCompressionCodecManager::CreateBlockCompressionCodec(int codec_type) {
    if (codec_type >=
        BlockCompressionCodec::COMPRESSION_TYPE_LIMIT || codec_type < 0){
        return NULL;
    }

    // scope locker
    MutexLocker scopelocker(m_mutex);;

    // 如果空闲列表存在该类型的codec, 则返回节点
    if (m_free_codec_size_array[codec_type] > 0) {
        --m_free_codec_size_array[codec_type];
        BlockCompressionCodec **p_codec = m_codec_array
               + codec_type * m_max_free_codec_size;
        return p_codec[m_free_codec_size_array[codec_type]];
    }

    BlockCompressionCodec *bc = NULL;
    switch (codec_type) {
        case BlockCompressionCodec::NONE :
            bc = new BlockCompressionCodecNone();
            break;

        case BlockCompressionCodec::ZLIB :
            bc = new BlockCompressionCodecZlib();
            break;

        case BlockCompressionCodec::BMZ :
            bc = new BlockCompressionCodecBmz();
            break;

        case BlockCompressionCodec::LZO :
            bc = new BlockCompressionCodecLzo();
            break;

        case BlockCompressionCodec::QUICKLZ :
            bc = new BlockCompressionCodecQuicklz();
            break;

        case BlockCompressionCodec::SNAPPY:
            bc = new BlockCompressionCodecSnappy();
            break;

        default:
            break;
    }

    return bc;
}

void
BlockCompressionCodecManager::DestroyBlockCompressionCodec(
    BlockCompressionCodec *codec) {
    // scoper locker
    MutexLocker scopelocker(m_mutex);;

    int codec_type = codec->GetType();

    if (m_free_codec_size_array[codec_type] >= m_max_free_codec_size) {
        delete codec;
    } else {
        BlockCompressionCodec **p_codec = m_codec_array
                + codec_type * m_max_free_codec_size;
        p_codec[m_free_codec_size_array[codec_type]++] = codec;
    }
}

} // namespace common


