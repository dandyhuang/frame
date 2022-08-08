// Copyright (c) 2018, The Toft Authors.
// All rights reserved.
//
// Author: Zhao Pu
#ifndef COMMON_ALGORITHM_KEYWORDSHIELD_H_
#define COMMON_ALGORITHM_KEYWORDSHIELD_H_

#include <stdio.h>
#include <unistd.h>
#include <string>
#include <unordered_map>
#include <vector>
#include "thirdparty/monitor_sdk_common/algorithm/ACAutomation.h"
#include "thirdparty/monitor_sdk_common/algorithm/ACNode.h"

namespace Ahocorasick {
class CKeywordShield {
 public:
  /**
     @see     CKeywordShield()
     @desc    构造函数
     @param   std::string &strFileName utf8 of wchar 型文件名
     @return  无
     @note
  */
  CKeywordShield();
  /**
 @see     ~CKeywordShield()
 @desc    析构函数
 @return  无
 @note
*/
  ~CKeywordShield();
  /**
@see     Initialize()
@desc    初始化函数
@param   std::string &strFileName utf8 of wchar 型文件名
@return  无
@note
*/
  bool Initialize(const std::string &strFileName);
  bool Initialize(const std::unordered_map<std::string, bool> hashmap_str2bool);
  /**
 @see     FilterSheild()
 @desc    过滤屏蔽词函数
 @param   std::string &strKeyword 查询的关键字
 @return  true 命中了过滤库，打印命中的过滤词(可能有多个，如色情，AV等) false
 无命中
 @note    1.可以对输入的非法query，过滤关键词。 2.
 可以对输出的结果，判断是否包含过滤词。
*/
  bool FilterSheild(std::string &strKeyword, std::string &strHit,
                    bool isFilter = false);
  /**
 @see     FilterSheild()
 @desc    过滤屏蔽词函数
 @param   std::string &strKeyword 查询的关键字
 @return  true 命中了过滤库
 @note    ?
*/
  bool FilterSheild(
      std::string &strKeyword,
      const std::vector<std::string> &skipStrings = std::vector<std::string>());
  /**
 @see     FuzzyMatch()
 @desc    查找前缀词函数
 @param   std::string &strKeyword 查询的关键字
     @param   vecCandidate命中了包含以该keyword为前缀的所有前缀树结果
 @return  true 命中了过滤库，
 @note    1.可以对输入的非法query，过滤关键词。 2.
 可以对输出的结果，判断是否包含过滤词。
*/
  bool FuzzyMatch(const std::string &strKeyword,
                  std::vector<std::string> &vecCandidate);
  /**
 @see     TailWithKeyTag()
 @desc    查找前缀词函数
 @param   std::string &strKeyword 查询的关键字
     @param   key,命中词
 @return  true 命中了过滤库，
 @note    1.可以对输入的非法query，过滤关键词。 2.
 可以对输出的结果，判断是否包含过滤词。
*/
  bool TailWithKeyTag(std::string strKeyword, std::string &key);

  // FindAll 模式
  bool FindAll(const std::string &query, std::vector<std::string> &match_group);
 private:
  /**
      AC自动机结构
  */
  CACAutomation<CUtf8Char> m_shieldAcDatas;
};
}  // namespace Ahocorasick
#endif  // COMMON_ALGORITHM_KEYWORDSHIELD_H_