/***************************************************************************
 *
 * Copyright (c) 2021 dandyhuang.com, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/

 /**
 * @file factory.h
 * @author 
 * @date 2021/06/24 15:32:07
 * @version $Revision$
 * @brief
 *
 **/

#pragma once

#include <string>
#include <unordered_map>
#include <memory>

namespace dandyhuang {

template <class BaseClass>
class Factory {
 public:
    template<class Derived>
    void add(const std::string& name) {
        static_assert(std::is_base_of<BaseClass, Derived>::value,
        "Derived is not the base of T");
        _inner_map.insert({name, [this]() -> std::shared_ptr<BaseClass> {
            return std::make_shared<Derived>();
        }});
    }

    // std::function<std::shared_ptr<BaseClass>()> get(const std::string& name) {
    //     auto it = _inner_map.find(name);
    //     if (it == _inner_map.end()) {
    //         return nullptr;
    //     }
    //     return it->second;
    // }

    std::shared_ptr<BaseClass> get(const std::string& name) {
        auto it = _inner_map.find(name);
        if (it == _inner_map.end()) {
            return nullptr;
        }
        return (it->second)();
    }

 private:
    std::unordered_map<std::string,
        std::function<std::shared_ptr<BaseClass>()>> _inner_map;
};

}  // namespace dandyhuang
