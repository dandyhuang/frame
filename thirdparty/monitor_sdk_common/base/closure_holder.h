//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-06-20 17:30
//  @file:      closure_default_runner.h
//  @author:    杨一飞(yangyifei@baidu.com)
//  @brief:
//
//********************************************************************

#ifndef COMMON_BASE_CLOSURE_DEFAULT_RUNNER_H
#define COMMON_BASE_CLOSURE_DEFAULT_RUNNER_H

////////////////////////////////////////////////////////
// closure 自动运行保护
// 在生命周期结束时以用户指定的默认参数Run
// 也可以显式Run
// 目前支持的参数长度：6
////////////////////////////////////////////////////////
// pre-bind: 
//
// Closure<int>* done = NewClosure...
// ClosureHolder<Closure<int>, int()> holder(done);
//
// ... (保护中...)
//
// int result =holder.Run();
// 
//
//
// ExClosure<int()>* done = NewExClosure...
// ClosureHolder<ExClosure<int()>, int()> holder(done);
//
// ... (保护中...)
//
// int result = holder.Run();
//
//////////////////////////////////////////////////////////
// post-bind:
//
// Closure<bool, int, int>* done = NewClosure...
// 以done->Run(5, 6); 为默认运行方式
// ClosureHolder<Closure<bool, int, int>, bool(int, int)> holder(done, 5, 6); 
//
// ... (保护中...)
//
// bool result = holder.Run(7, 8); //显式Run
//
//
// ExClosure<bool(int, int)>* done = NewExClosure...
// ClosureHolder<ExClosure<bool(int, int)>, bool(int, int)> holder(done, 5, 6);
//
// ... (保护中...)
//
// bool result = holder.Run(7, 8); // 显式Run
//
///////////////////////////////////////////////////////////

namespace common {

template <typename C>
class ClosureHolderBase {
public:
    ClosureHolderBase(C* closure) :
        m_is_to_default_run(true), m_closure(closure) {
    }

    void SetIfDefaultRun(bool if_default_run = true) {
        m_is_to_default_run = if_default_run;
    }

protected:
    virtual void DefaultRun() = 0;

    bool m_is_to_default_run;
    C* m_closure;
};

template <typename C, typename R>
class ClosureHolder : public ClosureHolderBase<C> {
protected:
    virtual void DefaultRun() {}
};

template <typename C, typename R>
class ClosureHolder<C, R ()> : public ClosureHolderBase<C> {
public:
    ClosureHolder(C* closure) : ClosureHolderBase<C>(closure) {
    }

    virtual ~ClosureHolder() {
        if (this->m_is_to_default_run) {
            this->DefaultRun();
        }
    }

    R Run() {
        this->m_is_to_default_run = false;
        return this->m_closure->Run();
    }

protected:
    virtual void DefaultRun() {
        this->m_closure->Run();
    }
};


template <
    typename C,
    typename R,
    typename Arg1
>
class ClosureHolder<C, R (Arg1)> : public ClosureHolderBase<C> {
public:
    ClosureHolder(C* closure, Arg1 arg1) : 
            ClosureHolderBase<C>(closure), m_arg1(arg1) {
    }

    virtual ~ClosureHolder() {
        if (this->m_is_to_default_run) {
            this->DefaultRun();
        }
    }

    R Run(Arg1 arg1) {
        this->m_is_to_default_run = false;
        return this->m_closure->Run(arg1);
    }

protected:
    virtual void DefaultRun() {
        this->m_closure->Run(m_arg1);
    }
    
    Arg1 m_arg1;
};


template <
    typename C,
    typename R,
    typename Arg1,
    typename Arg2
>
class ClosureHolder<C, R (Arg1, Arg2)> : public ClosureHolderBase<C> {
public:
    ClosureHolder(C* closure, Arg1 arg1, Arg2 arg2) : 
        ClosureHolderBase<C>(closure), m_arg1(arg1), m_arg2(arg2) {
    }

    virtual ~ClosureHolder() {
        if (this->m_is_to_default_run) {
            this->DefaultRun();
        }
    }

    R Run(Arg1 arg1, Arg2 arg2) {
        this->m_is_to_default_run = false;
        return this->m_closure->Run(arg1, arg2);
    }

protected:
    virtual void DefaultRun() {
        this->m_closure->Run(m_arg1, m_arg2);
    }

    Arg1 m_arg1;
    Arg2 m_arg2;
};



template <
    typename C,
    typename R,
    typename Arg1,
    typename Arg2,
    typename Arg3
>
class ClosureHolder<C, R (Arg1, Arg2, Arg3)> : public ClosureHolderBase<C> {
public:
    ClosureHolder(C* closure, Arg1 arg1, Arg2 arg2, Arg3 arg3) : 
        ClosureHolderBase<C>(closure), m_arg1(arg1), m_arg2(arg2), m_arg3(arg3) {
    }

    virtual ~ClosureHolder() {
        if (this->m_is_to_default_run) {
            this->DefaultRun();
        }
    }

    void Run(Arg1 arg1, Arg2 arg2, Arg3 arg3) {
        this->m_is_to_default_run = false;
        return this->m_closure->Run(arg1, arg2, arg3);
    }

protected:
    virtual void DefaultRun() {
        this->m_closure->Run(m_arg1, m_arg2, m_arg3);
    }

    Arg1 m_arg1;
    Arg2 m_arg2;
    Arg3 m_arg3;
};


template <
    typename C,
    typename R,
    typename Arg1,
    typename Arg2,
    typename Arg3,
    typename Arg4
>
class ClosureHolder<C, R (Arg1, Arg2, Arg3, Arg4)> : public ClosureHolderBase<C> {
public:
    ClosureHolder(C* closure, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4) : 
        ClosureHolderBase<C>(closure), m_arg1(arg1), m_arg2(arg2), m_arg3(arg3), m_arg4(arg4) {
    }

    virtual ~ClosureHolder() {
        if (this->m_is_to_default_run) {
            this->DefaultRun();
        }
    }

    void Run(Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4) {
        this->m_is_to_default_run = false;
        return this->m_closure->Run(arg1, arg2, arg3, arg4);
    }

protected:
    virtual void DefaultRun() {
        this->m_closure->Run(m_arg1, m_arg2, m_arg3, m_arg4);
    }

    Arg1 m_arg1;
    Arg2 m_arg2;
    Arg3 m_arg3;
    Arg4 m_arg4;
};


template <
    typename C,
    typename R,
    typename Arg1,
    typename Arg2,
    typename Arg3,
    typename Arg4,
    typename Arg5
>
class ClosureHolder<C, R (Arg1, Arg2, Arg3, Arg4, Arg5)> : public ClosureHolderBase<C> {
public:
    ClosureHolder(C* closure, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5) : 
        ClosureHolderBase<C>(closure), m_arg1(arg1), m_arg2(arg2), m_arg3(arg3), m_arg4(arg4), m_arg5(arg5) {
    }

    virtual ~ClosureHolder() {
        if (this->m_is_to_default_run) {
            this->DefaultRun();
        }
    }

    void Run(Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5) {
        this->m_is_to_default_run = false;
        return this->m_closure->Run(arg1, arg2, arg3, arg4, arg5);
    }

protected:
    virtual void DefaultRun() {
        this->m_closure->Run(m_arg1, m_arg2, m_arg3, m_arg4, m_arg5);
    }

    Arg1 m_arg1;
    Arg2 m_arg2;
    Arg3 m_arg3;
    Arg4 m_arg4;
    Arg5 m_arg5;
};


template <
    typename C,
    typename R,
    typename Arg1,
    typename Arg2,
    typename Arg3,
    typename Arg4,
    typename Arg5,
    typename Arg6
>
class ClosureHolder<C, R (Arg1, Arg2, Arg3, Arg4, Arg5, Arg6)> : public ClosureHolderBase<C> {
public:
    ClosureHolder(C* closure, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5, Arg6 arg6) : 
            ClosureHolderBase<C>(closure), m_arg1(arg1), m_arg2(arg2), m_arg3(arg3), m_arg4(arg4), m_arg5(arg5), m_arg6(arg6) {
    }

    virtual ~ClosureHolder() {
        if (this->m_is_to_default_run) {
            this->DefaultRun();
        }
    }

    void Run(Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5, Arg6 arg6) {
        this->m_is_to_default_run = false;
        return this->m_closure->Run(arg1, arg2, arg3, arg4, arg5);
    }

protected:
    virtual void DefaultRun() {
        this->m_closure->Run(m_arg1, m_arg2, m_arg3, m_arg4, m_arg5, m_arg6);
    }

    Arg1 m_arg1;
    Arg2 m_arg2;
    Arg3 m_arg3;
    Arg4 m_arg4;
    Arg5 m_arg5;
    Arg6 m_arg6;
};

} // end of namespace common

#endif // COMMON_BASE_CLOSURE_DEFAULT_RUNNER_H
