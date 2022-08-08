// Copyright (c) 2011, Vivo Inc.
// All rights reserved.
//
// Created: 04/28/10


#include "thirdparty/monitor_sdk_common/compress/block_compression_codec.h"
#include <stdio.h>
#include <stdlib.h>
#include "thirdparty/monitor_sdk_common/base/string/format.h"
#include "thirdparty/gtest/gtest.h"

namespace common{

class BlockCompressionCodecTest :
    public testing::Test {
protected:
    BlockCompressionCodecTest() {
        for (int i = 80; i <= 89; ++i)
            m_files.push_back(StringFormat("data/0000%d.htm", i));
    }

    void LoadTestData(size_t i) {
        FILE *fp = fopen(m_files[i].c_str(), "rb");
        ASSERT_TRUE(fp != NULL) << m_files[i];

        fseek(fp, 0, SEEK_END);

        int filesize = ftell(fp);
        ASSERT_GT(filesize, 0);
        m_test_data.resize(filesize);
        fseek(fp, 0, SEEK_SET);
        ASSERT_EQ(static_cast<size_t>(filesize),
                  fread(&m_test_data[0], 1, filesize, fp));
        fclose(fp);

        m_compressed_buf.resize(2 *filesize);
        m_uncompressed_buf.resize(filesize);
    }

    void TestOnFile(int type, int file_index) {
        LoadTestData(file_index);

        BlockCompressionCodec *bcc = BlockCompressionCodec::CreateCodec(type);
        ASSERT_TRUE(bcc != NULL);

        size_t compressed_buf_size = m_compressed_buf.size();
        EXPECT_EQ(BlockCompressionCodec::COMPRESSION_E_OK, bcc->Deflate(
                    &m_test_data[0], m_test_data.size(),
                    &m_compressed_buf[0], &compressed_buf_size));
        m_compressed_buf.resize(compressed_buf_size);

        // set larger uncompressed size to check uncompress_size setting
        size_t uncompressed_buf_size = m_uncompressed_buf.size() * 2;
        EXPECT_EQ(BlockCompressionCodec::COMPRESSION_E_OK, bcc->Inflate(
                    &m_compressed_buf[0], m_compressed_buf.size(),
                    &m_uncompressed_buf[0], &uncompressed_buf_size));
        m_uncompressed_buf.resize(uncompressed_buf_size);

        EXPECT_EQ(m_test_data.size(), uncompressed_buf_size);
        EXPECT_TRUE(m_test_data == m_uncompressed_buf) << type;
        delete bcc;
    }

    void CheckInflateAndDeflate(int type) {
        for (size_t i = 0; i < m_files.size(); ++i) {
            TestOnFile(type, i);
        }
    }

    void CheckLZODeflateWithNotEnoughSpace() {
        LoadTestData(0);

        BlockCompressionCodec *bcc = BlockCompressionCodec::CreateCodec(
                BlockCompressionCodec::LZO);
        ASSERT_TRUE(bcc != NULL);

        size_t compressed_buf_less_size = 0;
        EXPECT_EQ(BlockCompressionCodec::COMPRESSION_E_OUTPUT_OVERRUN, bcc->Deflate(
                    &m_test_data[0], m_test_data.size(),
                    &m_compressed_buf[0], &compressed_buf_less_size));
        delete bcc;
        bcc = NULL;
    }

    void CheckBMZInflateWithLargeOutputSpace() {
        LoadTestData(0);

        BlockCompressionCodec *bcc = BlockCompressionCodec::CreateCodec(
                BlockCompressionCodec::BMZ);
        ASSERT_TRUE(bcc != NULL);

        size_t compressed_buf_size = m_compressed_buf.size();
        EXPECT_EQ(BlockCompressionCodec::COMPRESSION_E_OK, bcc->Deflate(
                    &m_test_data[0], m_test_data.size(),
                    &m_compressed_buf[0], &compressed_buf_size));
        m_compressed_buf.resize(compressed_buf_size);

        // set larger uncompressed size to check uncompress_size setting
        size_t uncompressed_buf_size = m_uncompressed_buf.size() * 16;
        EXPECT_EQ(BlockCompressionCodec::COMPRESSION_E_OK, bcc->Inflate(
                    &m_compressed_buf[0], m_compressed_buf.size(),
                    &m_uncompressed_buf[0], &uncompressed_buf_size));
        m_uncompressed_buf.resize(uncompressed_buf_size);

        EXPECT_EQ(m_test_data.size(), uncompressed_buf_size);
        EXPECT_TRUE(m_test_data == m_uncompressed_buf) << BlockCompressionCodec::BMZ;
        delete bcc;
    }

    void CheckSNAPPYWithCRC() {
        LoadTestData(0);

        BlockCompressionCodec *bcc = BlockCompressionCodec::CreateCodec(
                BlockCompressionCodec::SNAPPY);
        ASSERT_TRUE(bcc != NULL);

        uint32_t data_crc = 0;

        size_t compressed_buf_size = m_compressed_buf.size();
        EXPECT_EQ(BlockCompressionCodec::COMPRESSION_E_OK, bcc->Deflate(
                    &m_test_data[0], m_test_data.size(),
                    &m_compressed_buf[0], compressed_buf_size, data_crc));
        m_compressed_buf.resize(compressed_buf_size);

        EXPECT_EQ(0xb5a13c6dU, data_crc);

        // set larger uncompressed size to check uncompress_size setting
        size_t uncompressed_buf_size = m_uncompressed_buf.size() * 16;
        EXPECT_EQ(BlockCompressionCodec::COMPRESSION_E_OK, bcc->Inflate(
                    &m_compressed_buf[0], m_compressed_buf.size(),
                    &m_uncompressed_buf[0], uncompressed_buf_size, data_crc));
        m_uncompressed_buf.resize(uncompressed_buf_size);

        EXPECT_EQ(m_test_data.size(), uncompressed_buf_size);
        EXPECT_TRUE(m_test_data == m_uncompressed_buf) << BlockCompressionCodec::SNAPPY;
        delete bcc;
        bcc = NULL;
    }

protected:
    std::vector<std::string> m_files;
    std::vector<char> m_test_data;
    std::vector<char> m_compressed_buf;
    std::vector<char> m_uncompressed_buf;
};

TEST_F(BlockCompressionCodecTest, None) {
    CheckInflateAndDeflate(BlockCompressionCodec::NONE);
}

TEST_F(BlockCompressionCodecTest, BMZ) {
    CheckInflateAndDeflate(BlockCompressionCodec::BMZ);
    CheckBMZInflateWithLargeOutputSpace();
}

TEST_F(BlockCompressionCodecTest, ZLIB) {
    CheckInflateAndDeflate(BlockCompressionCodec::ZLIB);
}

TEST_F(BlockCompressionCodecTest, LZO) {
    CheckInflateAndDeflate(BlockCompressionCodec::LZO);
    CheckLZODeflateWithNotEnoughSpace();
}

TEST_F(BlockCompressionCodecTest, QUICKLZ) {
    CheckInflateAndDeflate(BlockCompressionCodec::QUICKLZ);
}

TEST_F(BlockCompressionCodecTest, SNAPPY) {
    CheckInflateAndDeflate(BlockCompressionCodec::SNAPPY);
    CheckSNAPPYWithCRC();
}

TEST_F(BlockCompressionCodecTest, GZIP) {
    CheckInflateAndDeflate(BlockCompressionCodec::GZIP);
}

}   // namespace common
