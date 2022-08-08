// Copyright (c) 2011, Vivo Inc.
// All rights reserved.
//
// Created: 08/31/11
// Description: The implementation of consistent hash algorithm

#include <sstream>
#include "thirdparty/monitor_sdk_common/base/byte_order.h"
#include "thirdparty/monitor_sdk_common/base/stdint.h"
#include "thirdparty/monitor_sdk_common/base/string/concat.h"
#include "thirdparty/monitor_sdk_common/base/string/number.h"
#include "thirdparty/monitor_sdk_common/crypto/hash/consistent_hash/consistent_hash.h"
#include "thirdparty/gtest/gtest.h"

namespace common {

static const int kMaxServerNum = 1000;
static const int kServerPort = 26000;
static const int kRunTimesForSpeed = 1000000;

class ConsistentHashTest : public testing::Test {
protected:
    bool AddNodes(int node_num, bool set_node_replicate) {
        if (node_num > kMaxServerNum) {
            node_num = kMaxServerNum;
        }
        m_nodes.clear();

        for (int i = 0; i < node_num; ++i) {
            std::string node_id = IntegerToString(i);
            m_nodes.push_back(node_id);
            int replicate = -1;
            bool add_result = false;
            if (set_node_replicate == true) {
                replicate = (i+1)*1000;
                add_result = m_consistent_hash.AddNodeWithoutSubmit(node_id, replicate);
            } else {
                add_result = m_consistent_hash.AddNodeWithoutSubmit(node_id);
            }

            if (add_result == false) {
                return false;
            }
        }
        m_consistent_hash.Submit();
        return true;
    }

    int GetNodeIndex(const std::string& node)
    {
        for (unsigned int i = 0; i < m_nodes.size(); ++i) {
            if (m_nodes[i] == node) {
                return i;
            }
        }
        return -1;
    }

    uint64_t ComputeHash(int src_value)
    {
        std::string item = IntegerToString(src_value);
        uint64_t hash_value = CityHash64(item.c_str(), item.length());
        return hash_value;
    }

    void BalanceFactorTest(int split_index)
    {
        int before_alloc = 0;
        int after_alloc = 0;
        for (int i = 0 ; i < 100000 ; ++i) {
            uint64_t hash_value = ComputeHash(i);
            std::string node;
            bool find_result = m_consistent_hash.FindNode(hash_value, &node);
            if (find_result == true) {
                int id = -1;
                StringToNumber(node, &id);
                if (id < split_index) {
                    before_alloc++;
                } else {
                    after_alloc++;
                }
            } else {
                break;
            }
        }
        float balance_low_bound = 0.9;
        float balance_high_bound = 1.1;
        float float_after_alloc = static_cast<float>(after_alloc);
        ASSERT_TRUE(after_alloc > 0
                &&(float_after_alloc / before_alloc >= balance_low_bound
                    &&float_after_alloc / before_alloc <= balance_high_bound));
    }

protected:
    std::vector<std::string> m_nodes;
    ConsistentHash m_consistent_hash;
};

TEST_F(ConsistentHashTest, Init)
{
    AddNodes(8, true);
    ASSERT_EQ((size_t)8, m_consistent_hash.GetNodeCount());
    int virtual_num = 0;
    for (int i = 0; i < 8 ; i++) {
        virtual_num += (i+1)*1000;
    }
    ASSERT_EQ((size_t)virtual_num, m_consistent_hash.GetReplicatesCount());

    m_consistent_hash.Reset();
    AddNodes(7, false);
    ASSERT_EQ((size_t)7, m_consistent_hash.GetNodeCount());
    ASSERT_EQ((size_t)7000, m_consistent_hash.GetReplicatesCount());

    m_consistent_hash.Reset();
    ASSERT_EQ((size_t)0, m_consistent_hash.GetNodeCount());
    ASSERT_EQ((size_t)0, m_consistent_hash.GetReplicatesCount());
}

TEST_F(ConsistentHashTest, AddNode)
{
    int number = 0;
    for (int i = 0; i < 8 ; i++) {
        m_consistent_hash.AddNodeWithoutSubmit(IntegerToString(i), 1000);
        number += 1000;
        ASSERT_EQ((size_t)0, m_consistent_hash.GetNodeCount());
        ASSERT_EQ((size_t)0, m_consistent_hash.GetReplicatesCount());
    }
    m_consistent_hash.Submit();
    ASSERT_EQ((size_t)8, m_consistent_hash.GetNodeCount());
    ASSERT_EQ((size_t)number, m_consistent_hash.GetReplicatesCount());

    int inc_num = 0;
    for (int i = 0 ; i < 10; i++) {
        if (i < 8) {
            ASSERT_FALSE(m_consistent_hash.AddNodeWithoutSubmit(IntegerToString(i), 1000));
        } else {
            ASSERT_TRUE(m_consistent_hash.AddNodeWithoutSubmit(IntegerToString(i), 1000));
            inc_num += 1000;
        }
    }

    ASSERT_EQ((size_t)8, m_consistent_hash.GetNodeCount());
    ASSERT_EQ((size_t)number, m_consistent_hash.GetReplicatesCount());

    m_consistent_hash.Submit();
    ASSERT_EQ((size_t)10, m_consistent_hash.GetNodeCount());
    ASSERT_EQ((size_t)(number+inc_num), m_consistent_hash.GetReplicatesCount());

    number = 0;
    m_consistent_hash.Reset();

    for (int i = 0 ; i < 8 ; i++) {
        m_consistent_hash.AddNode(IntegerToString(i), 1000);
        number += 1000;
        ASSERT_EQ((size_t)(i+1), m_consistent_hash.GetNodeCount());
        ASSERT_EQ((size_t)number, m_consistent_hash.GetReplicatesCount());
    }
}

TEST_F(ConsistentHashTest, DeleteNode) {
    ASSERT_FALSE(m_consistent_hash.RemoveNode(IntegerToString(0)));
    for (int i = 0 ; i < 8 ; i++) {
        m_consistent_hash.AddNodeWithoutSubmit(IntegerToString(i), 1000);
    }
    m_consistent_hash.Submit();
    for (int i = 0 ; i < 8 ; i++) {
        ASSERT_TRUE(m_consistent_hash.RemoveNode(IntegerToString(i)));
        ASSERT_EQ((size_t)(7-i), m_consistent_hash.GetNodeCount());
        int number = (7-i)*1000;
        ASSERT_EQ((size_t)number, m_consistent_hash.GetReplicatesCount());
        break;
    }

    for (int i = 0 ; i < 16 ; i++) {
        if (i < 8) {
            m_consistent_hash.AddNode(IntegerToString(i), 1000);
        } else {
            m_consistent_hash.AddNodeWithoutSubmit(IntegerToString(i), 1000);
        }
    }
    for (int i = 15 ; i >= 0 ; i--) {
        if (i >= 8) {
            ASSERT_TRUE(m_consistent_hash.RemoveNode(IntegerToString(i)));
            ASSERT_EQ((size_t)8, m_consistent_hash.GetNodeCount());
            ASSERT_EQ((size_t)8000, m_consistent_hash.GetReplicatesCount());
        } else {
            ASSERT_TRUE(m_consistent_hash.RemoveNode(IntegerToString(i)));
            ASSERT_EQ((size_t)i, m_consistent_hash.GetNodeCount());
            int number = i*1000;
            ASSERT_EQ((size_t)number, m_consistent_hash.GetReplicatesCount());
        }
    }
    ASSERT_EQ((size_t)0, m_consistent_hash.GetNodeCount());
    ASSERT_EQ((size_t)0, m_consistent_hash.GetReplicatesCount());
}

TEST_F(ConsistentHashTest, ClientHashCorrection)
{
    AddNodes(8, true);
    std::vector<uint64_t> item_hash_values;
    std::vector<std::string> nodes;

    for (int i = 0; i < 10000; ++i) {
        uint64_t hash_value = ComputeHash(i);
        std::string node;
        bool find_result = m_consistent_hash.FindNode(hash_value, &node);
        ASSERT_TRUE(find_result);
        item_hash_values.push_back(hash_value);
        nodes.push_back(node);
    }

    for (unsigned int i = 0; i < item_hash_values.size(); ++i) {
        std::string node;
        bool find_result = m_consistent_hash.FindNode(item_hash_values[i], &node);
        ASSERT_TRUE(find_result);
        ASSERT_STREQ(node.c_str(), nodes[i].c_str());
        find_result =  m_consistent_hash.FindNode(IntegerToString(i), &node);
        ASSERT_STREQ(node.c_str(), nodes[i].c_str());
    }

    std::string node;
    ASSERT_TRUE(m_consistent_hash.FindNode(-1, &node));

    m_consistent_hash.Reset();
    for (int i = 0; i < 10000; ++i) {
        uint64_t hash_value = ComputeHash(i);
        ASSERT_FALSE(m_consistent_hash.FindNode(hash_value, &node));
    }
}

TEST_F(ConsistentHashTest, ConsistentHashSpeed)
{
    AddNodes(8, true);
    for (int i = 0; i < kRunTimesForSpeed; ++i) {
        uint64_t hash_value = ComputeHash(i);
        std::string node;
        m_consistent_hash.FindNode(hash_value, &node);
    }
}

TEST_F(ConsistentHashTest, ConsistentHashAdd)
{
    AddNodes(8, false);
    for (int i = 8 ; i < 16 ; ++i) {
        ASSERT_TRUE(m_consistent_hash.AddNodeWithoutSubmit(IntegerToString(i)));
    }

    for (int i = 0 ; i < 100000 ; ++i) {
        uint64_t hash_value = ComputeHash(i);
        std::string node;
        m_consistent_hash.FindNode(hash_value, &node);
        int id =-1;
        StringToNumber(node, &id);
        ASSERT_LT(id, 8);
    }

    m_consistent_hash.Submit();
    BalanceFactorTest(8);
}

TEST_F(ConsistentHashTest, ConsistentHashDelete)
{
    AddNodes(16, false);

    for (int i = 8 ; i < 16 ; i++) {
        m_consistent_hash.RemoveNode(IntegerToString(i));
    }

    for (int i = 0 ; i < 100000 ; ++i) {
        uint64_t hash_value = ComputeHash(i);
        std::string node;
        m_consistent_hash.FindNode(hash_value, &node);
        int id =-1;
        StringToNumber(node, &id);
        ASSERT_LT(id, 8);
    }
    BalanceFactorTest(4);
}

TEST_F(ConsistentHashTest, ConsistentHashBalance)
{
    for (int k = 0; k < 2; k++) {
        m_consistent_hash.Reset();
        if (k == 0) {
            AddNodes(8, false);
        } else {
            AddNodes(8, true);
        }
        int node_load[8];
        for (int i = 0; i < 8; i++) {
            node_load[i] = 0;
        }
        for (int i = 0; i < 100000; i++) {
            uint64_t hash_value = ComputeHash(i);
            std::string node;
            m_consistent_hash.FindNode(hash_value, &node);
            int node_index = GetNodeIndex(node);
            ASSERT_TRUE(node_index >= 0 && (unsigned int)node_index < m_nodes.size());
            node_load[node_index]++;
        }
    }
}

TEST_F(ConsistentHashTest, ItemMoveAddOneServer)
{
    AddNodes(7, false);
    std::vector<std::string> nodes;
    for (int i = 0; i < 100000; i++) {
        uint64_t hash_value = ComputeHash(i);
        std::string node;
        m_consistent_hash.FindNode(hash_value, &node);
        nodes.push_back(node);
    }
    // add a server
    ASSERT_TRUE(m_consistent_hash.AddNode(IntegerToString(7)));
    std::string new_node = IntegerToString(7);
    for (int i = 0; i < 100000; i++) {
        uint64_t hash_value = ComputeHash(i);
        std::string node;
        m_consistent_hash.FindNode(hash_value, &node);

        // judge items routing to original servers' change
        ASSERT_TRUE(node == nodes[i]|| node == new_node);
    }
}

TEST_F(ConsistentHashTest, ItemMoveDeleteOneServer)
{
    AddNodes(8, false);
    std::vector<std::string> nodes;
    for (int i = 0; i < 100000; i++) {
        uint64_t hash_value = ComputeHash(i);
        std::string node;
        m_consistent_hash.FindNode(hash_value, &node);
        nodes.push_back(node);
    }

    // delete a server
    std::string failed_node_id = IntegerToString(7);
    ASSERT_TRUE(m_consistent_hash.RemoveNode(IntegerToString(7)));
    for (int i = 0; i < 100000; i++) {
        uint64_t hash_value = ComputeHash(i);
        std::string node;
        m_consistent_hash.FindNode(hash_value, &node);
        if (nodes[i] != failed_node_id) {
            // judge items routing to original working servers' change
            ASSERT_STREQ(node.c_str(), nodes[i].c_str());
        }
    }
}

} // namespace common
