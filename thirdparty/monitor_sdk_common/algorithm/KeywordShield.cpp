// Copyright (c) 2018, The Toft Authors.
// All rights reserved.
//
// Author: Zhao Pu

#include "thirdparty/monitor_sdk_common/algorithm/KeywordShield.h"
#include <algorithm>
#include <fstream>
#include <iostream>

using namespace std;

namespace Ahocorasick {
/*
@note 匿名空间，只在本类中使用，提供给本类的字符串替换，去特殊字符的功能
*/
namespace {
string TrimRight(string str, string delstr = "\r\t\n") {
  if (str.empty()) return "";

  string::size_type pos = str.find_last_not_of(delstr);
  if (pos == string::npos) return "";
  str = str.substr(0, pos + 1).c_str();
  return str;
}
string TrimLeft(string str, string delstr = "\r\t\n") {
  if (str.empty()) return "";

  string::size_type pos = str.find_first_not_of(delstr);
  if (pos == string::npos) return "";
  return str.substr(pos, str.length() - pos + 1).c_str();
}

string Trim(string str, string delstr = "\r\t\n") {
  string ret = TrimLeft(str, delstr);
  ret = TrimRight(ret, delstr);
  return ret;
}
static string Replace(string &str, const char *from, const char *to) {
  if (NULL == from || NULL == to) return str;
  if (strlen(from) == 0) return str;

  string::size_type index = 0;
  while ((index = str.find(from, index)) != string::npos) {
    str.replace(index, strlen(from), to, strlen(to));
    index += strlen(to);
  }

  return str;
}
static string StrToLower(const std::string &str) {
  string s(str.c_str(), str.size());
  transform(s.begin(), s.end(), s.begin(), ::tolower);
  return s;
}
}  // namespace

CKeywordShield::CKeywordShield() {}

CKeywordShield::~CKeywordShield() {}

bool CKeywordShield::Initialize(const std::string &strFileName) {
  if (strFileName == "") {
    return false;
  }
  if (access(strFileName.c_str(), F_OK) != 0) {
    return false;
  }
  ifstream inFile(strFileName.c_str());

  int nLineNum = 0;
  std::string strFileLine = "";
  while (getline(inFile, strFileLine)) {
    CDataNode *pDataNode = new CDataNode(nLineNum++, Trim(strFileLine));
    if (NULL == pDataNode) {
      continue;
    }
    m_shieldAcDatas.AddKeywordNode(std::shared_ptr<CDataNode>(pDataNode));
  }

  m_shieldAcDatas.BuildAutomation();
  inFile.close();

  return true;
}

bool CKeywordShield::Initialize(
    const std::unordered_map<std::string, bool> hashmap_str2bool) {
  if (hashmap_str2bool.size() == 0) {
    return false;
  }

  int nLineNum = 0;
  std::string strFileLine = "";
  for (auto iter = hashmap_str2bool.begin(); iter != hashmap_str2bool.end();
       ++iter) {
    strFileLine = iter->first;
    CDataNode *pDataNode = new CDataNode(nLineNum++, Trim(strFileLine));
    if (NULL == pDataNode) {
      continue;
    }
    m_shieldAcDatas.AddKeywordNode(std::shared_ptr<CDataNode>(pDataNode));
  }
  m_shieldAcDatas.BuildAutomation();
  return true;
}

bool CKeywordShield::FilterSheild(std::string &strKeyword, std::string &strHit,
                                  bool isFilter) {
  if (strKeyword == "") {
    return false;
  }

  std::vector<Segment> vecNodes;
  CACAutomation<CUtf8Char>::CALLBACKTYPEV2 checkStatusFunc = std::bind(
      checkFunV2<CUtf8Char>, std::placeholders::_1, std::placeholders::_2,
      std::placeholders::_3, std::placeholders::_4, std::placeholders::_5,
      std::placeholders::_6);
  int nCount = m_shieldAcDatas.SegmentationByMM(
      strKeyword.c_str(), strKeyword.size(), vecNodes, checkStatusFunc);

  for (int i = 0; i < nCount; i++) {
    CDataNode *pNode = vecNodes[i].DataNodes[0];
    if (strHit != "") {
      strHit = strHit + "|" + pNode->Value;
    } else {
      strHit = pNode->Value;
    }
    if (isFilter) {
      Replace(strKeyword, pNode->Value.c_str(), "");
    }
  }

  // printf("strHit = %s",strHit.c_str());
  if (strHit == "") {
    return false;
  }
  return true;
}

bool CKeywordShield::FilterSheild(std::string &strKeyword,
                                  const std::vector<std::string> &skipStrings) {
  if (strKeyword == "") {
    return false;
  }

  std::string strHit = "";
  std::vector<Segment> vecNodes;
  CACAutomation<CUtf8Char>::CALLBACKTYPEV2 checkStatusFunc = std::bind(
      checkFunV2<CUtf8Char>, std::placeholders::_1, std::placeholders::_2,
      std::placeholders::_3, std::placeholders::_4, std::placeholders::_5,
      std::placeholders::_6);
  int nCount =
      m_shieldAcDatas.SegmentationByMM(strKeyword.c_str(), strKeyword.size(),
                                       vecNodes, checkStatusFunc, skipStrings);

  for (int i = 0; i < nCount; i++) {
    CDataNode *pNode = vecNodes[i].DataNodes[0];
    if (strHit != "") {
      strHit = strHit + "|" + pNode->Value;
    } else {
      strHit = pNode->Value;
    }

    Replace(strKeyword, pNode->Value.c_str(), "");
  }

  if (strHit == "") {
    return false;
  }
  return true;
}

bool CKeywordShield::FuzzyMatch(const std::string &strKeyword,
                                std::vector<std::string> &vecCandidate) {
  if (strKeyword == "") {
    return false;
  }
  std::vector<CDataNode *> vecNodes;
  std::vector<std::string> skipStrings;
  skipStrings.push_back(" ");
  skipStrings.push_back("-");

  int nCount = m_shieldAcDatas.StrContain(strKeyword.c_str(), strKeyword.size(),
                                          vecNodes, skipStrings);
  for (int i = 0; i < nCount; i++) {
    // printf("vecNodes size = %d,value = %s\n
    // ",nCount,vecNodes[i]->Value.c_str());
    vecCandidate.push_back(vecNodes[i]->Value);
  }
  return true;
}

bool CKeywordShield::TailWithKeyTag(std::string strKeyword, std::string &key) {
  if (strKeyword == "") return false;

  string tmpstr = StrToLower(strKeyword);
  std::vector<Segment> vecNodes;
  CACAutomation<CUtf8Char>::CALLBACKTYPEV2 checkStatusFunc = std::bind(
      checkFunV2<CUtf8Char>, std::placeholders::_1, std::placeholders::_2,
      std::placeholders::_3, std::placeholders::_4, std::placeholders::_5,
      std::placeholders::_6);
  int nCount = m_shieldAcDatas.SegmentationByMM(tmpstr.c_str(), tmpstr.size(),
                                                vecNodes, checkStatusFunc);
  for (int i = 0; i < nCount; i++) {
    CDataNode *pNode = vecNodes[i].DataNodes[0];

    int index = strKeyword.size() - pNode->Value.size();
    if (index > 0 && tmpstr.substr(index) == pNode->Value) {
      key = strKeyword.substr(index);
      Replace(strKeyword, pNode->Value.c_str(), "");
      return true;
    }
  }

  return false;
}

bool CKeywordShield::FindAll(const std::string &query, std::vector<std::string> &match_group){
 if (query.empty()) {
    return false;
  }

  std::vector<Segment> vecNodes;
  CACAutomation<CUtf8Char>::CALLBACKTYPEV2 checkStatusFunc = std::bind(
      checkFunV2<CUtf8Char>, std::placeholders::_1, std::placeholders::_2,
      std::placeholders::_3, std::placeholders::_4, std::placeholders::_5,
      std::placeholders::_6);
  int nCount = m_shieldAcDatas.SegmentationByMM(
      query.c_str(), query.size(), vecNodes, checkStatusFunc);

  for (int i = 0; i < nCount; i++) {
    CDataNode *pNode = vecNodes[i].DataNodes[0];
	if(!pNode->Value.empty()){
		match_group.push_back(pNode->Value);
	}
  }
  
  if(match_group.size() <= 0){
	  return false;
  }
  return true;
}
}  // namespace Ahocorasick
