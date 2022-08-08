/***************************************************************************
 *
 * Copyright (c) 2021 dandy.com, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/

/**
 * @file factory.h
 * @author (dag)
 * @date 2021/06/24 15:32:07
 * @version $Revision$
 * @brief
 *
 **/

#pragma once

#include <string>
#include <unordered_map>

#include "boost/functional/factory.hpp"
#include "boost/functional/value_factory.hpp"

namespace dag {

template <class BaseClass>
class Factory {
 public:
  template <class Derived>
  void add(const std::string& name) {
    static_assert(std::is_base_of<BaseClass, Derived>::value, "Derived is not the base of T");
    _inner_map.insert({name, [this]() -> BaseClass* { return new Derived(); }});
  }
  BaseClass* get(const std::string& name) {
    auto it = _inner_map.find(name);
    if (it == _inner_map.end()) {
      return nullptr;
    }
    return it->second();
  }

 private:
  std::unordered_map<std::string, std::function<BaseClass*()>> _inner_map;
};

}  // namespace dag
