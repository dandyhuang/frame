// Copyright (c) 2018, The Toft Authors.
// All rights reserved.
//
// Author: Zhao Pu
#ifndef COMMON_ALGORITHM_ACAUTOMATION_H_
#define COMMON_ALGORITHM_ACAUTOMATION_H_

#include <assert.h>
#include <fcntl.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <algorithm>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "ACNode.h"

namespace Ahocorasick {

/**
    @see     CDataNode
    @desc    索引&字符串原串的组合结构
    @note
*/
class CDataNode {
 public:
  CDataNode() : Index(0), Value("") {}
  CDataNode(int index, const std::string &value) : Index(index), Value(value) {}
  virtual ~CDataNode(){};

  int Index;
  std::string Value;
};

/**
    @see     TACNode节点类
    @desc
   字典树节点的结构，包含父节点，孩子节点，失败返回节点，节点深度，最后匹配的字符串等，以及存储匹配过，或包含过
   自身节点的词的集合。
    @note 深度，取决于最长字符串的字节数。宽度，取决于utf8字节串的样本多样性。
*/
template <typename NodeChar>
struct TACNode {
 public:
  TACNode(NodeChar mchar, TACNode *pParent, TACNode *pFailed, int pos)
      : lastMchar(mchar), ptParent(pParent), ptFailed(pFailed), iPos(pos) {}

  ~TACNode() {
    ptParent = NULL;
    ptFailed = NULL;

    ptNexts.clear();
    MatchKeys.clear();
    ContainKeys.clear();
  }

  NodeChar lastMchar;
  TACNode<NodeChar> *ptParent;
  TACNode<NodeChar> *ptFailed;
  int iPos;  // 节点所在深度

  std::vector<TACNode<NodeChar> *> ptNexts;
  std::vector<CDataNode *> MatchKeys;
  std::vector<CDataNode *> ContainKeys;
};

template <typename NodeChar>
class CACNodePointerComp {
 public:
  bool operator()(TACNode<NodeChar> *i, TACNode<NodeChar> *j) const {
    return i->lastMchar < j->lastMchar;
  }
};

struct CheckFunHelper {
  virtual ~CheckFunHelper() {}
};

template <typename NodeChar>
bool checkFun(TACNode<NodeChar> *node, const char *buf, int bufLen, int next,
              std::vector<CDataNode *> &v,
              std::map<int, std::shared_ptr<CheckFunHelper>> &HelperMap) {
  if (node == NULL) return false;

  TACNode<NodeChar> *checknode = node;
  while (checknode != checknode->ptFailed) {
    if (!checknode->MatchKeys.empty()) {
      v.assign(checknode->MatchKeys.begin(), checknode->MatchKeys.end());
      return true;
    }
    checknode = checknode->ptFailed;
  }

  return false;
}

template <typename NodeChar>
bool checkFunV2(TACNode<NodeChar> *node, const char *buf, int bufLen,
                int startPos, int next, std::vector<CDataNode *> &v) {
  if (node == NULL) return false;

  if (!node->MatchKeys.empty()) {
    v.assign(node->MatchKeys.begin(), node->MatchKeys.end());
    return true;
  }

  return false;
}

struct Segment {
  int StartPos;                        // 开始位置
  int Length;                          // 长度
  std::vector<CDataNode *> DataNodes;  // 指向的数据
};

template <class T>
struct SFuncPtr {
  typedef std::function<bool(TACNode<T> *, const char *, int, int,
                             std::vector<CDataNode *> &,
                             std::map<int, std::shared_ptr<CheckFunHelper>> &)>
      callbackType;
  typedef std::function<bool(TACNode<T> *, const char *, int, int, int,
                             std::vector<CDataNode *> &)>
      callbackTypeV2;
};

/**
    @see     CACAutomation 字典树or 前缀树
    @desc
   字典树节点的结构，包含父节点，孩子节点，失败返回节点，节点深度，最后匹配的字符串等，以及存储匹配过，或包含过
   自身节点的词的集合。
    @note 深度，取决于最长字符串的字节数。宽度，取决于utf8字节串的样本多样性。
*/
template <typename NodeChar>
class CACAutomation {
 public:
  typedef typename SFuncPtr<NodeChar>::callbackType CALLBACKTYPE;
  typedef typename SFuncPtr<NodeChar>::callbackTypeV2 CALLBACKTYPEV2;

  CACAutomation();
  ~CACAutomation();
  int Release();
  void ClearKeywordNodes();

  void AddKeywordNode(std::shared_ptr<CDataNode> node);
  bool BuildAutomation(
      const std::vector<std::string> &skipStrings = std::vector<std::string>(),
      bool needFaildPt = true, bool needMatchVector = true,
      bool needContainVector = true);

  int GetNodeCount() const;

  int SegmentationByMM(
      const char *buf, int bufLen, std::vector<Segment> &v,
      CALLBACKTYPEV2 &checkFun,
      const std::vector<std::string> &skipStrings = std::vector<std::string>());
  int SegmentationByMM(
      const char *buf, int bufLen, std::vector<Segment> &v,
      const std::vector<std::string> &skipStrings = std::vector<std::string>());

  int StrContain(
      const char *buf, int bufLen, std::vector<CDataNode *> &v,
      const std::vector<std::string> &skipStrings = std::vector<std::string>(),
      std::vector<CDataNode *> *vEqual = NULL);

 private:
  TACNode<NodeChar> *addMChar(TACNode<NodeChar> *node, const NodeChar &mchar);
  bool addMKey(CDataNode *node, const std::vector<std::string> &skipStrings,
               bool needMatchVector, bool needContainVecotr);
  TACNode<NodeChar> *step(TACNode<NodeChar> *fromNode, const NodeChar &mchar,
                          bool &otherWord) const;
  TACNode<NodeChar> *stepNotToFail(TACNode<NodeChar> *fromNode,
                                   const NodeChar &mchar) const;

  int m_nodeCount;
  std::vector<TACNode<NodeChar> *> m_nodes;
  TACNode<NodeChar> *m_root;
  std::vector<std::shared_ptr<CDataNode>> m_keywordNodes;
};

/**
    @see     CACAutomation 字典树or 前缀树
    @desc
   字典树节点的结构，包含父节点，孩子节点，失败返回节点，节点深度，最后匹配的字符串等，以及存储匹配过，或包含过
   自身节点的词的集合。
    @note    模板实现
*/

template <typename NodeChar>
CACAutomation<NodeChar>::CACAutomation()
    : m_nodeCount(0), m_nodes(), m_root(NULL), m_keywordNodes() {}

template <typename NodeChar>
CACAutomation<NodeChar>::~CACAutomation() {
  ClearKeywordNodes();
  Release();
}

template <typename NodeChar>
TACNode<NodeChar> *CACAutomation<NodeChar>::addMChar(TACNode<NodeChar> *node,
                                                     const NodeChar &mchar) {
  if ((NULL == node) || mchar.IsEmpty()) {
    return NULL;
  }

  int count = node->ptNexts.size();
  for (int i = 0; i < count; ++i) {
    if ((NULL != node->ptNexts[i]) &&
        (mchar.Equal(node->ptNexts[i]->lastMchar))) {
      return node->ptNexts[i];
    }
  }

  TACNode<NodeChar> *p = new TACNode<NodeChar>(
      mchar, node, NULL, node->iPos + node->lastMchar.GetBufLen());
  if (NULL == p) {
    return NULL;
  }

  node->ptNexts.push_back(p);

  return p;
}

template <typename NodeChar>
bool CACAutomation<NodeChar>::addMKey(
    CDataNode *node, const std::vector<std::string> &skipStrings,
    bool needMatchVector, bool needContainVecotr) {
  TACNode<NodeChar> *thisNode = m_root;
  int i = 0;
  int len = node->Value.length();

  while (i < len) {
    bool needSkip = false;
    int skipLen = 0;
    for (int j = 0; j < (int)skipStrings.size(); ++j) {
      if (strncmp(node->Value.c_str() + i, skipStrings[j].c_str(),
                  skipStrings[j].length()) == 0) {
        needSkip = true;
        skipLen = skipStrings[j].length();
        break;
      }
    }

    if (needSkip) {
      i += skipLen;
      continue;
    }

    int retval = 0;
    NodeChar mchar;
    if (0 >= (retval = mchar.Parse((const BYTE *)&(node->Value[i]), len - i))) {
      return false;
    }
    i += retval;

    thisNode = addMChar(thisNode, mchar);
    if (NULL != thisNode && needContainVecotr) {
      thisNode->ContainKeys.push_back(node);
    }
  }

  bool r = thisNode != NULL;
  if (r && needMatchVector) {
    thisNode->MatchKeys.push_back(node);
  }

  return r;
}

template <typename NodeChar>
int CACAutomation<NodeChar>::Release() {
  if (NULL == m_root) {
    return 0;
  }

  for (int i = m_nodeCount - 1; i >= 0; --i) {
    delete m_nodes[i];
  }

  m_nodes.clear();
  m_nodeCount = 0;
  m_root = NULL;

  return 0;
}

template <typename NodeChar>
TACNode<NodeChar> *CACAutomation<NodeChar>::step(TACNode<NodeChar> *fromNode,
                                                 const NodeChar &mchar,
                                                 bool &otherWord) const {
  otherWord = false;
  TACNode<NodeChar> *preNode = NULL;
  TACNode<NodeChar> *thisNode = fromNode;

  TACNode<NodeChar> Node(mchar, NULL, NULL, 0);
  while (preNode != thisNode) {
    typename std::vector<TACNode<NodeChar> *>::iterator it;
    it = lower_bound(thisNode->ptNexts.begin(), thisNode->ptNexts.end(), &Node,
                     CACNodePointerComp<NodeChar>());
    if (it != thisNode->ptNexts.end() && (*it)->lastMchar.Equal(mchar)) {
      return *it;
    }

    preNode = thisNode;
    thisNode = thisNode->ptFailed;
    otherWord = true;
  }

  otherWord = true;
  return m_root;
}

template <typename NodeChar>
bool CACAutomation<NodeChar>::BuildAutomation(
    const std::vector<std::string> &skipStrings, bool needFaildPt,
    bool needMatchVector, bool needContainVector) {
  if (m_root != NULL) {
    (void)Release();
  }
  /*	m_keywordNodes.resize(m_keywordNodes.size());*/
  NodeChar mchar;
  m_root = new TACNode<NodeChar>(mchar, NULL, NULL, 0);
  if (NULL == m_root) {
    return false;
  }
  m_root->ptFailed = m_root;
  m_nodeCount = 1;
  m_nodes.clear();
  m_nodes.push_back(m_root);

  for (int i = 0; i < (int)m_keywordNodes.size(); ++i) {
    if (!addMKey(m_keywordNodes[i].get(), skipStrings, needMatchVector,
                 needContainVector)) {
      printf("i=%d addMKey %s fail\r\n", i, m_keywordNodes[i]->Value.c_str());
    }
  }

  int count = 1;
  int i = 0;
  int matchkeycount = 0;
  int containkeycount = 0;
  int nextkeycount = 0;

  // 广度优先将所有节点加到m_nodes
  while (i < count) {
    for (int j = 0; j < (int)m_nodes[i]->ptNexts.size(); ++j) {
      m_nodes.push_back(m_nodes[i]->ptNexts[j]);
      ++count;
      ++m_nodeCount;
    }

    matchkeycount += m_nodes[i]->MatchKeys.size();
    containkeycount += m_nodes[i]->ContainKeys.size();
    nextkeycount += m_nodes[i]->ptNexts.size();
    ++i;
  }
  // printf("datacount:%d\n datacapacity:%d\n acNodeCount:%d\n
  // matchkeycount:%d\n containkeycount:%d\n
  // nextkeycount:%d\n",m_keywordNodes.size(), m_keywordNodes.capacity(),
  // m_nodeCount,matchkeycount,containkeycount, nextkeycount);

  /*	m_nodes.resize(m_nodes.size());*/
  for (int i = 0; i < (int)m_nodes.size(); ++i) {
    sort(m_nodes[i]->ptNexts.begin(), m_nodes[i]->ptNexts.end(),
         CACNodePointerComp<NodeChar>());
  }

  if (!needFaildPt) return true;

  // 构造错误跳转
  for (int i = 1; i < (int)m_nodes.size(); ++i) {
    TACNode<NodeChar> *preNode = NULL;
    TACNode<NodeChar> *parentFailNode = m_nodes[i]->ptParent->ptFailed;
    assert(parentFailNode != NULL);
    while (preNode != parentFailNode) {
      for (int j = 0; j < (int)parentFailNode->ptNexts.size(); ++j) {
        if ((parentFailNode->ptNexts[j] != m_nodes[i]) &&
            (NULL != parentFailNode->ptNexts[j]) &&
            parentFailNode->ptNexts[j]->lastMchar.Equal(
                m_nodes[i]->lastMchar)) {
          m_nodes[i]->ptFailed = parentFailNode->ptNexts[j];
          break;
        }
      }

      if (NULL != m_nodes[i]->ptFailed) {
        break;
      }

      preNode = parentFailNode;
      parentFailNode = parentFailNode->ptFailed;
    }

    if (NULL == m_nodes[i]->ptFailed) {
      m_nodes[i]->ptFailed = m_root;
    }
  }
  return true;
}

template <typename NodeChar>
void CACAutomation<NodeChar>::AddKeywordNode(std::shared_ptr<CDataNode> node) {
  if (!node) return;

  m_keywordNodes.push_back(node);
}

template <typename NodeChar>
void CACAutomation<NodeChar>::ClearKeywordNodes() {
  m_keywordNodes.clear();
}

template <typename NodeChar>
int CACAutomation<NodeChar>::GetNodeCount() const {
  return m_nodeCount;
}

template <typename NodeChar>
int CACAutomation<NodeChar>::SegmentationByMM(
    const char *buf, int bufLen, std::vector<Segment> &v, CALLBACKTYPEV2 &fun,
    const std::vector<std::string> &skipStrings) {
  if (NULL == m_root) return 0;

  v.clear();
  int i = 0;
  TACNode<NodeChar> *thisNode = m_root;
  std::vector<CDataNode *> maxWord;
  int startPos = 0;
  int maxLen = 0;

  bool otherWord = true;

  NodeChar mchar;
  while (i < bufLen) {
    bool needSkip = false;
    int skipLen = 0;
    for (int j = 0; j < (int)skipStrings.size(); ++j) {
      if (strncmp(buf + i, skipStrings[j].c_str(), skipStrings[j].length()) ==
          0) {
        needSkip = true;
        skipLen = skipStrings[j].length();
        break;
      }
    }

    if (needSkip) {
      i += skipLen;
      continue;
    }

    if (thisNode == m_root) {
      startPos = i;
      maxWord.clear();
      maxLen = 0;
    }

    int retval = 0;
    if (0 >= (retval = mchar.Parse((const BYTE *)(buf + i), bufLen - i))) {
      return -1;
    }
    i += retval;

    thisNode = step(thisNode, mchar, otherWord);
    if (otherWord) {
      if (!maxWord.empty()) {
        Segment segMent;
        segMent.DataNodes = maxWord;
        segMent.Length = maxLen;
        segMent.StartPos = startPos;
        v.push_back(segMent);

        i = startPos + maxLen;
        thisNode = m_root;
        continue;
      }

      startPos = i - thisNode->iPos - mchar.GetBufLen();
      maxWord.clear();
      maxLen = 0;
    }

    std::vector<CDataNode *> result;
    if ((thisNode != m_root) &&
        fun(thisNode, buf, bufLen, startPos, i, result)) {
      maxWord = thisNode->MatchKeys;
      maxLen = i - startPos;
    }
  }

  if (!maxWord.empty()) {
    Segment segMent;
    segMent.DataNodes = maxWord;
    segMent.Length = maxLen;
    segMent.StartPos = startPos;
    v.push_back(segMent);
  }

  return v.size();
}

template <typename NodeChar>
int CACAutomation<NodeChar>::SegmentationByMM(
    const char *buf, int bufLen, std::vector<Segment> &v,
    const std::vector<std::string> &skipStrings) {
  CALLBACKTYPEV2 checkStatusFunc = std::bind(
      checkFunV2<NodeChar>, std::placeholders::_1, std::placeholders::_2,
      std::placeholders::_3, std::placeholders::_4, std::placeholders::_5,
      std::placeholders::_6);
  return SegmentationByMM(buf, bufLen, v, checkStatusFunc, skipStrings);
}

template <typename NodeChar>
TACNode<NodeChar> *CACAutomation<NodeChar>::stepNotToFail(
    TACNode<NodeChar> *fromNode, const NodeChar &mchar) const {
  TACNode<NodeChar> Node(mchar, NULL, NULL, 0);
  typename std::vector<TACNode<NodeChar> *>::iterator it =
      lower_bound(fromNode->ptNexts.begin(), fromNode->ptNexts.end(), &Node,
                  CACNodePointerComp<NodeChar>());
  if (it != fromNode->ptNexts.end() && (*it)->lastMchar.Equal(mchar)) {
    return *it;
  }
  return NULL;
}
template <typename NodeChar>
int CACAutomation<NodeChar>::StrContain(
    const char *buf, int bufLen, std::vector<CDataNode *> &v,
    const std::vector<std::string> &skipStrings,
    std::vector<CDataNode *> *vEqual) {
  v.clear();
  if (vEqual != NULL) {
    vEqual->clear();
  }
  if (NULL == buf) {
    return 0;
  }
  TACNode<NodeChar> *thisNode = m_root;
  if (NULL == thisNode) {
    printf("%s: %s", __FUNCTION__, "NULL == thisNode");
    return -1;
  }
  NodeChar mchar;
  int i = 0;
  while (i < bufLen) {
    bool needSkip = false;
    int skipLen = 0;
    for (int j = 0; j < (int)skipStrings.size(); ++j) {
      if (strncmp(buf + i, skipStrings[j].c_str(), skipStrings[j].length()) ==
          0) {
        needSkip = true;
        skipLen = skipStrings[j].length();
        break;
      }
    }
    if (needSkip) {
      i += skipLen;
      continue;
    }
    int retval = 0;
    if (0 >= (retval = mchar.Parse((const BYTE *)(buf + i), bufLen - i))) {
      return -1;
    }
    i += retval;
    thisNode = stepNotToFail(thisNode, mchar);
    if (thisNode == NULL) {
      return 0;
    }
  }
  if (thisNode != NULL) {
    v.assign(thisNode->ContainKeys.begin(), thisNode->ContainKeys.end());
    if (vEqual != NULL) {
      vEqual->assign(thisNode->MatchKeys.begin(), thisNode->MatchKeys.end());
    }
  }
  return v.size();
}
}  // namespace Ahocorasick
#endif  // COMMON_ALGORITHM_ACAUTOMATION_H_
