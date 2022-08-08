//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 12:03
//  @file:      barrier_closure.h
//  @author:    
//  @brief:     
//
//********************************************************************


#ifndef COMMON_BASE_BARRIER_CLOSURE_H
#define COMMON_BASE_BARRIER_CLOSURE_H

#include "thirdparty/monitor_sdk_common/base/closure2.h"

namespace common {

// BarrierClosure是一种特殊的 Closure，其包含两个参数：N 和 done。
// 在 BarrierClosure 本身被调用 N 次后，会自动调用 done，并且销毁
// BarrierClosure 本身。
//
// 一个典型的应用是并行发送多个rpc调用，并在它们都返回之后使用done进行下一步的
// 处理：
// Closure<void>* barrier_done = new BarrierClosure(2, done);
// rpc.Foo(barrier_done);
// rpc.Bar(barrier_done);
//
// 在 Foo 和 Bar 返回的时候，barrier_done 分别会被调用一次，他们之间的顺序是不定
// 的，但是他们都返回之后，done 会被调用，并且 barrier_done 会被自动释放。
class BarrierClosure : public Closure<void> {
public:
    BarrierClosure(int N, Closure<void>* done) : m_all_done(done), m_left(N)  {}
    ~BarrierClosure();
    void Run();
    virtual bool IsSelfDelete() const { return false; }
private:
    Closure<void>* m_all_done;
    int m_left;
};

Closure<void>* NewBarrierClosure(int n, Closure<void>* done);

} // end of namespace common

#endif // COMMON_BASE_BARRIER_CLOSURE_H

