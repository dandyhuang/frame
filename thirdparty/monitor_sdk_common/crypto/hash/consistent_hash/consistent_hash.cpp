// Copyright (c) 2011, Vivo Inc.
// All rights reserved.
//
// Created: 08/31/11
// Description: The implementation of consistent hash algorithm

#include "thirdparty/monitor_sdk_common/crypto/hash/consistent_hash/consistent_hash.h"
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <iostream>
#include <map>
#include <sstream>
#include "thirdparty/monitor_sdk_common/base/string/concat.h"

// namespace common {

bool ConsistentHash::AddNode(const std::string& node, int replicate)
{
    if (AddNodeWithoutSubmit(node, replicate) == false) {
        return false;
    }
    Submit();
    return true;
}

bool ConsistentHash::AddNodeWithoutSubmit(const std::string& node, int replicate)
{
    // if the node has existed
    if (m_nodes.find(node) != m_nodes.end() ||
            m_new_nodes.find(node) != m_new_nodes.end()) {
        return false;
    }

    // set default replicate
    if (replicate < 0) {
        replicate = m_default_replicate;
    }
    m_new_nodes[node]=replicate;
    return true;
}

bool ConsistentHash::RemoveNode(const std::string& node)
{
    // if node is a new-add node
    std::map<std::string, int>::iterator it;
    if ((it = m_new_nodes.find(node)) != m_new_nodes.end()) {
        m_new_nodes.erase(it);
        return true;
    }

    // if the virtual nodes has existed
    if (m_nodes.find(node) != m_nodes.end()) {
        Reconstruct(node);
        return true;
    }
    return false;
}

bool ConsistentHash::FindNode(uint64_t hash_value, std::string* node) const
{
    if (m_interval_size == 0) {
        return false;
    }
    uint64_t interval_idx = hash_value / m_interval_size;
    if (interval_idx > m_interval_indexes.size()) {
        return false;
    } else if (interval_idx == m_interval_indexes.size()) {
        interval_idx--;
    }

    // if there is no virtual servers in the inteval
    if (m_interval_indexes[interval_idx] == -1) {
        *node = m_replicates[0].second;
        return true;
    }

    // find from the first inteval
    unsigned int st_index = m_interval_indexes[interval_idx];
    // find the first virtual server with the equal or greater hash value
    while (st_index < m_replicates.size()) {
        if (m_replicates[st_index].first >= hash_value) {
            *node = m_replicates[st_index].second;
            return true;
        }
        ++st_index;
    }
    // if no server has equal or greate hash value, routing to the first one default
    *node = m_replicates[0].second;
    return true;
}

bool ConsistentHash::FindNode(const std::string& key, std::string* node) const
{
    uint64_t hash_value = common::CityHash64(key.c_str(), key.length());
    return FindNode(hash_value, node);
}

void ConsistentHash::Submit()
{
    std::vector<std::pair<uint64_t, std::string> > new_virtual_nodes;
    ComputeNewReplicates(&new_virtual_nodes);
    MergeNewNode(new_virtual_nodes);
    ComputeCircleInterval();
}

void ConsistentHash::Reset(int node_replicate)
{
    if (node_replicate <= 0) {
        m_default_replicate = kDefaultNodeReplicate;
    } else {
        m_default_replicate = node_replicate;
    }
    m_nodes.clear();
    m_nodes.clear();
    m_new_nodes.clear();
    m_replicates.clear();
    m_interval_indexes.clear();
    m_interval_size = 0;
}

const std::vector<std::string> ConsistentHash::GetNodes() const
{
    std::vector<std::string> nodes;
    std::map<std::string, int>::const_iterator it;
    for (it = m_nodes.begin() ; it != m_nodes.end() ; ++it) {
        nodes.push_back(it->first);
    }
    return nodes;
}

int ConsistentHash::GetNodeReplicate(const std::string& node) const
{
    std::map<std::string, int>::const_iterator it;
    if ((it = m_nodes.find(node)) != m_nodes.end()) {
        return it->second;
    }
    return -1;
}

void ConsistentHash::Reconstruct(const std::string& delete_node)
{
    std::vector<std::pair<uint64_t, std::string> > new_virtual_nodes;
    for (size_t i = 0 ; i < m_replicates.size(); ++i) {
        if (delete_node != m_replicates[i].second) {
            new_virtual_nodes.push_back(m_replicates[i]);
        }
    }
    m_replicates = new_virtual_nodes;
    m_nodes.erase(delete_node);
    ComputeCircleInterval();
}

void ConsistentHash::ComputeNewReplicates(std::vector<std::pair<uint64_t, std::string> >*
        new_replicates)
{
    std::map<std::string, int>::iterator it;
    for (it = m_new_nodes.begin(); it != m_new_nodes.end() ; ++it) {
        std::string node = it->first;
        int replicate = it->second;
        for (int i = 0 ; i < replicate ; ++i) {
            std::string virtual_node = common::StringConcat(it->first, ":", i);
            uint64_t hash_value = common::CityHash64(virtual_node.c_str(), virtual_node.length());
            new_replicates->push_back(std::pair<uint64_t, std::string>(hash_value, node));
        }
    }

    // sort the virtual nodes
    std::sort(new_replicates->begin(), new_replicates->end(), replicate_comparator);
}

void ConsistentHash::MergeNewNode(
        const std::vector<std::pair<uint64_t, std::string> >& new_replicates)
{
    std::vector<std::pair<uint64_t, std::string> > replicates;
    size_t i = 0;
    size_t j = 0;
    // merge new virtual nodes
    while (i < new_replicates.size() && j < m_replicates.size()) {
        if (replicate_comparator(new_replicates[i], m_replicates[j])) {
            replicates.push_back(new_replicates[i++]);
        } else {
            replicates.push_back(m_replicates[j++]);
        }
    }
    while (i < new_replicates.size()) {
        replicates.push_back(new_replicates[i++]);
    }
    while (j < m_replicates.size()) {
        replicates.push_back(m_replicates[j++]);
    }
    m_replicates = replicates;

    // add new nodes
    std::map<std::string, int>::iterator it;
    for (it = m_new_nodes.begin(); it != m_new_nodes.end() ; ++it) {
        m_nodes.insert(std::pair<std::string, int>(it->first, it->second));
    }
    m_new_nodes.clear();
}

void ConsistentHash::ComputeCircleInterval()
{
    size_t interval_num = m_replicates.size();
    // control the total number of intervals to save memory consuming
    if (interval_num > kMaxIntervalNum) {
        interval_num = kMaxIntervalNum;
    }
    std::vector<uint64_t> interval_vals;

    m_interval_indexes.clear();
    if (interval_num == 0) {
        m_interval_size = 0;
        return;
    } else {
        m_interval_size = UINT64_MAX / interval_num;
        for (size_t i = 0; i < interval_num; ++i) {
            m_interval_indexes.push_back(-1);
            interval_vals.push_back(i*m_interval_size);
        }
    }
    size_t interval_idx = 0;
    size_t node_idx = 0;
    size_t size = m_replicates.size();
    while (interval_idx < interval_num && node_idx < size) {
        uint64_t value_interval = interval_vals[interval_idx];
        uint64_t value_node = m_replicates[node_idx].first;
        if (value_node >= value_interval) {
            m_interval_indexes[interval_idx] = node_idx;
            ++interval_idx;
        } else {
            ++node_idx;
        }
    }
}

// } // namespace common
