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

// BarrierClosure��һ������� Closure�����������������N �� done��
// �� BarrierClosure �������� N �κ󣬻��Զ����� done����������
// BarrierClosure ����
//
// һ�����͵�Ӧ���ǲ��з��Ͷ��rpc���ã��������Ƕ�����֮��ʹ��done������һ����
// ����
// Closure<void>* barrier_done = new BarrierClosure(2, done);
// rpc.Foo(barrier_done);
// rpc.Bar(barrier_done);
//
// �� Foo �� Bar ���ص�ʱ��barrier_done �ֱ�ᱻ����һ�Σ�����֮���˳���ǲ���
// �ģ��������Ƕ�����֮��done �ᱻ���ã����� barrier_done �ᱻ�Զ��ͷš�
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

