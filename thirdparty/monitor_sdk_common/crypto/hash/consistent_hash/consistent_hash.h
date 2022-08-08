// Copyright (c) 2011, Vivo Inc.
// All rights reserved.
//
// Created: 08/31/11
// Description: The implementation of consistent hash algorithm

#ifndef COMMON_CRYPTO_HASH_CONSISTENT_HASH_CONSISTENT_HASH_H
#define COMMON_CRYPTO_HASH_CONSISTENT_HASH_CONSISTENT_HASH_H

#include <cstring>
#include <map>
#include <string>
#include <utility>
#include <vector>
#include "thirdparty/monitor_sdk_common/base/uncopyable.h"
#include "thirdparty/monitor_sdk_common/crypto/hash/city.h"

// namespace common {

/// @brief Implement the consistent hash algorithm
class ConsistentHash : common::Uncopyable
{
public:
    /// The default value for all the servers' replicates
    static const unsigned int kDefaultNodeReplicate = 1000;

    /// The maximum interval num
    static const unsigned int kMaxIntervalNum = 1000000;

    /// @param node_replicate set the default node replicate for nodes
    explicit ConsistentHash(int node_replicate = kDefaultNodeReplicate):
        m_default_replicate(node_replicate), m_interval_size(0) {}

    /// @brief Add a node to handle the query
    /// @param node node name
    /// @param replicate number of replicate,
    ///                  if replicate<=0, a default value will be set for the node
    /// @retval true add success
    /// @retval false add failed, this may be caused by tending to add an existed node
    /// Calling this function will cause replicates computing immediately; consequently,
    /// this function is more appropriate for single node adding, while preferring
    /// AddNodeWithoutSubmit() and Submit() for batch nodes adding
    bool AddNode(const std::string& node, int replicate = -1);

    /// @brief Add a node to handle the query without computing the replicates immediately
    /// @param node node name
    /// @param replicate number of replicate,
    ///                  if replicate<=0, a default value will be set for the node
    /// @retval true add success
    /// @retval false add failed, this may be caused by tending to add an existed node
    /// This function may be efficient for batch adding nodes because replicates of adding
    /// node won't be computed immediately; consequently, Submit() function must be called
    /// after batch of nodes adding before searching, such as calling FindNode() function
    bool AddNodeWithoutSubmit(const std::string& node, int replicate = -1);

    /// @brief Remove a node
    /// @param node node name to be removed
    /// @retval true remove success
    /// @retval false remove failed, this may be cased by tending to remove an not-existed node
    bool RemoveNode(const std::string& node);

    /// @brief Find the node handle the search key
    /// @param item_hash_value hash value of the search key
    /// @param node the pointer to the string store the node handling the key
    /// @retval true success to find the node handle the key
    /// @retval false if there are nodes
    bool FindNode(uint64_t key_hash_value, std::string* node) const;

    /// @brief Find the node handle the search key
    /// @param item_hash_value hash value of the search key
    /// @param node the pointer to the string store the node handling the key
    /// @retval true success to find the node handle the key
    /// @retval false if there are nodes
    bool FindNode(const std::string& key, std::string* node) const;

    /// @brief construct node circle by add new_nodes
    void Submit();

    /// @brief Clear the servers' configuration
    /// @param node_replicate: the default replicate for all nodes
    void Reset(int node_replicate = kDefaultNodeReplicate);

    /// @brief Get all the nodes
    /// @retval std::vector<std::string>& the nodes vector
    const std::vector<std::string> GetNodes() const;

    /// @brief Get node's replicate
    /// @param node node name
    /// @retval >=0 the replicate of the node
    /// @retval -1 the node not exist
    int GetNodeReplicate(const std::string& node) const;

    /// @brief Get the nodes number
    /// @retval int the number of nodes
    size_t GetNodeCount() const
    {
        return m_nodes.size();
    }

    /// @brief Get all nodes' replicates
    /// @retval std::vecotr the replicates vector
    const std::vector<std::pair<uint64_t, std::string> >& GetReplicates() const
    {
        return m_replicates;
    }

    /// @brief Get virtual nodes' number
    /// @retval int the number of replicates
    size_t GetReplicatesCount() const
    {
        return m_replicates.size();
    }

private:
    /// @brief construct node circle by remove a node
    void Reconstruct(const std::string& node);

    /// @brief Compute new virtual nodes
    /// @param nw_virtual_nodes virtual nodes corresponds to new nodes
    void ComputeNewReplicates(
            std::vector<std::pair<uint64_t, std::string> >* new_replicates);

    /// @brief Merge new virtual nodes with orignal virtual nodes
    /// @param new_virtual_nodes new virtual nodes
    /// @param index_map node index map
    void MergeNewNode(const std::vector<std::pair<uint64_t, std::string> >& new_replicates);

    /// @brief Compute the inteval indexes for item search
    /// This function will split the virtual server hash value space into size-equal inteval
    void ComputeCircleInterval();

private:
    /// @brief The default replicate number for node
    /// This value could be set in constructor or Reset();when call AddNode, if the replicate
    /// is not assigned, the node's replicate will be assigned as m_default_replicate
    int m_default_replicate;

    /// @brief The nodes and corresponding replicates, where the key is node and value is replicate
    /// When user call AddNode, the new node with its replicate will be add to m_nodes if
    /// re_construct_node_circle is set to true; otherwise, new nodes will added to m_nodes
    /// when calling ReConstruct
    std::map<std::string, int> m_nodes;

    /// @brief New nodes andd corresponding replicates which no replicates generated, where the key
    /// is node with value is replicate
    /// m_new_nodes will store new nodes when call AddNode with re_construct_node_circle=false,
    /// when calling Construct, replicates will be generated for m_new_nodes and add to
    /// m_replicates and the m_new_nodes will be cleared

    std::map<std::string, int> m_new_nodes;
    /// @brief Replicates for nodes in m_nodes, the element of vector is a pair with key is
    /// hash_value of a node and value is the corresponding node of the replicate
    /// In Construct(), new replicates will be generated for m_new_nodes and be
    /// added to m_replicates

    std::vector<std::pair<uint64_t, std::string> > m_replicates;

    /// @brief Interval indexes of the m_replicates, the element is the index of m_replicates
    /// m_interval_indexes stores all the start indexes in m_replicates when dividing hash
    /// spaces into equal intervals, for details, see ComputeCircleInterval()
    std::vector<int> m_interval_indexes;

    /// @brief The number of intervals
    uint64_t m_interval_size;

    // function object for replicates' comparision
    class ReplicateComparator
    {
    public:
        /// @brief Compare replicate according to corresponding hash_value
        /// @retval true if node_a.first<=node_b.first
        /// @retval false if node_a.first>node_b.first
        bool operator()(const std::pair<uint64_t, std::string>& node_a,
                        const std::pair<uint64_t, std::string>& node_b) const
        {
            return node_a.first <= node_b.first;
        }
    } replicate_comparator;
};

// } // namespace common

#endif // COMMON_CRYPTO_HASH_CONSISTENT_HASH_CONSISTENT_HASH_H
