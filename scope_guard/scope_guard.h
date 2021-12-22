/***************************************************************************
 *
 * Copyright (c) 2021 dandyhuang.com, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/

/**
 * @file scope_guard.h
 * @author
 * @date 2021/06/24 15:32:07
 * @version $Revision$
 * @brief
 *
 **/
#pragma once

#include <functional>


namespace dandyhuang {
template <typename R = void, typename... Args>
class ScopeGuard {
 public:
  explicit ScopeGuard(std::function<R(Args...)> onExitScope)
      : onExitScope_(onExitScope), dismissed_(false) {}

  ~ScopeGuard() {
    if (!dismissed_) {
      onExitScope_();
    }
  }

  R operator()(Args... args) { return onExitScope_(args...); }

  void Dismiss() { dismissed_ = true; }

 private:
  std::function<R(Args...)> onExitScope_;
  bool dismissed_;

 private:  // noncopyable
  ScopeGuard(ScopeGuard const&);
  ScopeGuard& operator=(ScopeGuard const&);
};

#define SCOPEGUARD_LINENAME_CAT(name, line) name##line
#define SCOPEGUARD_LINENAME(name, line) SCOPEGUARD_LINENAME_CAT(name, line)

#define ON_SCOPE_EXIT(callback) ScopeGuard<> SCOPEGUARD_LINENAME(EXIT, __LINE__)(callback)

}  // namespace dandyhuang
