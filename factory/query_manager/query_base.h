/***************************************************************************
 *
 * Copyright (c) 2021 .com, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/

/**
 * @file strategy_factory.h
 * @author 
 * @date 2021/06/24 15:32:07
 * @version $Revision$
 * @brief
 *
 **/

#pragma once
#include <memory>
#include <string>


namespace dandyhuang {

class QueryBase {
 public:
  QueryBase() = default;
  virtual ~QueryBase() = default;
  virtual int init(std::string strategy_name) {}
  int QueryData();
};
}  // namespace dandyhuang
