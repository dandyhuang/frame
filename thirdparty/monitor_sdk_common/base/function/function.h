//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-14 13:35
//  @file:      function.h
//  @author:    
//  @brief:     
//
//********************************************************************


#ifndef COMMON_BASE_FUNCTION_FUNCTION_H
#define COMMON_BASE_FUNCTION_FUNCTION_H
#pragma once

#include <assert.h>
#include "thirdparty/monitor_sdk_common/base/function/function_base.h"
#include "thirdparty/monitor_sdk_common/base/function/param_traits.h"

// GLOBAL_NOLINT(whitespace/parens)
// GLOBAL_NOLINT(runtime/explicit)

namespace common {

// First, we forward declare the Function class template. This informs the
// compiler that the template only has 1 type parameter which is the function
// signature that the Function is representing.
//
// After this, create template specializations for 0-8 parameters. Note that
// even though the template typelist grows, the specialization still
// only has one type: the function signature.
template <typename Signature>
class Function;

template <typename R>
class Function<R (void)> : public internal::FunctionBase
{
private:
    typedef R (*PolymorphicInvoke)(internal::InvokerStorageBase*
    );

public:
    Function(FunctionBase::ClearType* p = NULL) : FunctionBase(NULL, NULL)
    {
        assert(p == NULL);
    }

    // We pass InvokerStorageHolder by const ref to avoid incurring an
    // unnecessary AddRef/Unref pair even though we will modify the object.
    // We cannot use a normal reference because the compiler will warn
    // since this is often used on a return value, which is a temporary.
    //
    // Note that this constructor CANNOT be explicit, and that Bind() CANNOT
    // return the exact Function<> type.  See base/function/bind.h for details.
    template <typename T>
    Function(const internal::InvokerStorageHolder<T>& invoker_holder) :
        FunctionBase(
            reinterpret_cast<InvokeFuncStorage>(&T::Invoker::DoInvoke),
            &invoker_holder.m_invoker_storage
        )
    {
        CheckSignatureConsistency(&T::Invoker::DoInvoke);
    }

    R operator()(

    ) const
    {
        PolymorphicInvoke f =
            reinterpret_cast<PolymorphicInvoke>(m_polymorphic_invoke);

        return f(
            m_invoker_storage.get()
        );
    }

    bool operator==(const Function& rhs) const
    {
        return this->IsEqualTo(rhs);
    }

    Function& operator=(FunctionBase::ClearType* p)
    {
        assert(p == NULL);
        this->Clear();
        return *this;
    }

    void Swap(Function* other)
    {
        this->DoSwap(other);
    }

private:
    // inconsist param type may cause compiling error
    static void CheckSignatureConsistency(PolymorphicInvoke) {}
};

template <
    typename R,
    typename A1
>
class Function<R (A1)> :
    public internal::FunctionBase
{
private:
    typedef R (*PolymorphicInvoke)(internal::InvokerStorageBase*,
        typename internal::ParamTraits<A1>::ForwardType
    );

public:
    Function(FunctionBase::ClearType* p = NULL) : FunctionBase(NULL, NULL)
    {
        assert(p == NULL);
    }

    // We pass InvokerStorageHolder by const ref to avoid incurring an
    // unnecessary AddRef/Unref pair even though we will modify the object.
    // We cannot use a normal reference because the compiler will warn
    // since this is often used on a return value, which is a temporary.
    //
    // Note that this constructor CANNOT be explicit, and that Bind() CANNOT
    // return the exact Function<> type.  See base/function/bind.h for details.
    template <typename T>
    Function(const internal::InvokerStorageHolder<T>& invoker_holder) :
        FunctionBase(
            reinterpret_cast<InvokeFuncStorage>(&T::Invoker::DoInvoke),
            &invoker_holder.m_invoker_storage
        )
    {
        CheckSignatureConsistency(&T::Invoker::DoInvoke);
    }

    R operator()(
        typename internal::ParamTraits<A1>::ForwardType a1
    ) const
    {
        PolymorphicInvoke f =
            reinterpret_cast<PolymorphicInvoke>(m_polymorphic_invoke);

        return f(
            m_invoker_storage.get(), a1
        );
    }

    bool operator==(const Function& rhs) const
    {
        return this->IsEqualTo(rhs);
    }

    Function& operator=(FunctionBase::ClearType* p)
    {
        assert(p == NULL);
        this->Clear();
        return *this;
    }

    void Swap(Function* other)
    {
        this->DoSwap(other);
    }

private:
    // inconsist param type may cause compiling error
    static void CheckSignatureConsistency(PolymorphicInvoke) {}
};

template <
    typename R,
    typename A1,
    typename A2
>
class Function<R (A1, A2)> :
    public internal::FunctionBase
{
private:
    typedef R (*PolymorphicInvoke)(internal::InvokerStorageBase*,
        typename internal::ParamTraits<A1>::ForwardType,
        typename internal::ParamTraits<A2>::ForwardType
    );

public:
    Function(FunctionBase::ClearType* p = NULL) : FunctionBase(NULL, NULL)
    {
        assert(p == NULL);
    }

    // We pass InvokerStorageHolder by const ref to avoid incurring an
    // unnecessary AddRef/Unref pair even though we will modify the object.
    // We cannot use a normal reference because the compiler will warn
    // since this is often used on a return value, which is a temporary.
    //
    // Note that this constructor CANNOT be explicit, and that Bind() CANNOT
    // return the exact Function<> type.  See base/function/bind.h for details.
    template <typename T>
    Function(const internal::InvokerStorageHolder<T>& invoker_holder) :
        FunctionBase(
            reinterpret_cast<InvokeFuncStorage>(&T::Invoker::DoInvoke),
            &invoker_holder.m_invoker_storage
        )
    {
        CheckSignatureConsistency(&T::Invoker::DoInvoke);
    }

    R operator()(
        typename internal::ParamTraits<A1>::ForwardType a1,
        typename internal::ParamTraits<A2>::ForwardType a2
    ) const
    {
        PolymorphicInvoke f =
            reinterpret_cast<PolymorphicInvoke>(m_polymorphic_invoke);

        return f(
            m_invoker_storage.get(), a1,
            a2
        );
    }

    bool operator==(const Function& rhs) const
    {
        return this->IsEqualTo(rhs);
    }

    Function& operator=(FunctionBase::ClearType* p)
    {
        assert(p == NULL);
        this->Clear();
        return *this;
    }

    void Swap(Function* other)
    {
        this->DoSwap(other);
    }

private:
    // inconsist param type may cause compiling error
    static void CheckSignatureConsistency(PolymorphicInvoke) {}
};

template <
    typename R,
    typename A1,
    typename A2,
    typename A3
>
class Function<R (A1, A2, A3)> :
    public internal::FunctionBase
{
private:
    typedef R (*PolymorphicInvoke)(internal::InvokerStorageBase*,
        typename internal::ParamTraits<A1>::ForwardType,
        typename internal::ParamTraits<A2>::ForwardType,
        typename internal::ParamTraits<A3>::ForwardType
    );

public:
    Function(FunctionBase::ClearType* p = NULL) : FunctionBase(NULL, NULL)
    {
        assert(p == NULL);
    }

    // We pass InvokerStorageHolder by const ref to avoid incurring an
    // unnecessary AddRef/Unref pair even though we will modify the object.
    // We cannot use a normal reference because the compiler will warn
    // since this is often used on a return value, which is a temporary.
    //
    // Note that this constructor CANNOT be explicit, and that Bind() CANNOT
    // return the exact Function<> type.  See base/function/bind.h for details.
    template <typename T>
    Function(const internal::InvokerStorageHolder<T>& invoker_holder) :
        FunctionBase(
            reinterpret_cast<InvokeFuncStorage>(&T::Invoker::DoInvoke),
            &invoker_holder.m_invoker_storage
        )
    {
        CheckSignatureConsistency(&T::Invoker::DoInvoke);
    }

    R operator()(
        typename internal::ParamTraits<A1>::ForwardType a1,
        typename internal::ParamTraits<A2>::ForwardType a2,
        typename internal::ParamTraits<A3>::ForwardType a3
    ) const
    {
        PolymorphicInvoke f =
            reinterpret_cast<PolymorphicInvoke>(m_polymorphic_invoke);

        return f(
            m_invoker_storage.get(), a1,
            a2,
            a3
        );
    }

    bool operator==(const Function& rhs) const
    {
        return this->IsEqualTo(rhs);
    }

    Function& operator=(FunctionBase::ClearType* p)
    {
        assert(p == NULL);
        this->Clear();
        return *this;
    }

    void Swap(Function* other)
    {
        this->DoSwap(other);
    }

private:
    // inconsist param type may cause compiling error
    static void CheckSignatureConsistency(PolymorphicInvoke) {}
};

template <
    typename R,
    typename A1,
    typename A2,
    typename A3,
    typename A4
>
class Function<R (A1, A2, A3, A4)> :
    public internal::FunctionBase
{
private:
    typedef R (*PolymorphicInvoke)(internal::InvokerStorageBase*,
        typename internal::ParamTraits<A1>::ForwardType,
        typename internal::ParamTraits<A2>::ForwardType,
        typename internal::ParamTraits<A3>::ForwardType,
        typename internal::ParamTraits<A4>::ForwardType
    );

public:
    Function(FunctionBase::ClearType* p = NULL) : FunctionBase(NULL, NULL)
    {
        assert(p == NULL);
    }

    // We pass InvokerStorageHolder by const ref to avoid incurring an
    // unnecessary AddRef/Unref pair even though we will modify the object.
    // We cannot use a normal reference because the compiler will warn
    // since this is often used on a return value, which is a temporary.
    //
    // Note that this constructor CANNOT be explicit, and that Bind() CANNOT
    // return the exact Function<> type.  See base/function/bind.h for details.
    template <typename T>
    Function(const internal::InvokerStorageHolder<T>& invoker_holder) :
        FunctionBase(
            reinterpret_cast<InvokeFuncStorage>(&T::Invoker::DoInvoke),
            &invoker_holder.m_invoker_storage
        )
    {
        CheckSignatureConsistency(&T::Invoker::DoInvoke);
    }

    R operator()(
        typename internal::ParamTraits<A1>::ForwardType a1,
        typename internal::ParamTraits<A2>::ForwardType a2,
        typename internal::ParamTraits<A3>::ForwardType a3,
        typename internal::ParamTraits<A4>::ForwardType a4
    ) const
    {
        PolymorphicInvoke f =
            reinterpret_cast<PolymorphicInvoke>(m_polymorphic_invoke);

        return f(
            m_invoker_storage.get(), a1,
            a2,
            a3,
            a4
        );
    }

    bool operator==(const Function& rhs) const
    {
        return this->IsEqualTo(rhs);
    }

    Function& operator=(FunctionBase::ClearType* p)
    {
        assert(p == NULL);
        this->Clear();
        return *this;
    }

    void Swap(Function* other)
    {
        this->DoSwap(other);
    }

private:
    // inconsist param type may cause compiling error
    static void CheckSignatureConsistency(PolymorphicInvoke) {}
};

template <
    typename R,
    typename A1,
    typename A2,
    typename A3,
    typename A4,
    typename A5
>
class Function<R (A1, A2, A3, A4, A5)> :
    public internal::FunctionBase
{
private:
    typedef R (*PolymorphicInvoke)(internal::InvokerStorageBase*,
        typename internal::ParamTraits<A1>::ForwardType,
        typename internal::ParamTraits<A2>::ForwardType,
        typename internal::ParamTraits<A3>::ForwardType,
        typename internal::ParamTraits<A4>::ForwardType,
        typename internal::ParamTraits<A5>::ForwardType
    );

public:
    Function(FunctionBase::ClearType* p = NULL) : FunctionBase(NULL, NULL)
    {
        assert(p == NULL);
    }

    // We pass InvokerStorageHolder by const ref to avoid incurring an
    // unnecessary AddRef/Unref pair even though we will modify the object.
    // We cannot use a normal reference because the compiler will warn
    // since this is often used on a return value, which is a temporary.
    //
    // Note that this constructor CANNOT be explicit, and that Bind() CANNOT
    // return the exact Function<> type.  See base/function/bind.h for details.
    template <typename T>
    Function(const internal::InvokerStorageHolder<T>& invoker_holder) :
        FunctionBase(
            reinterpret_cast<InvokeFuncStorage>(&T::Invoker::DoInvoke),
            &invoker_holder.m_invoker_storage
        )
    {
        CheckSignatureConsistency(&T::Invoker::DoInvoke);
    }

    R operator()(
        typename internal::ParamTraits<A1>::ForwardType a1,
        typename internal::ParamTraits<A2>::ForwardType a2,
        typename internal::ParamTraits<A3>::ForwardType a3,
        typename internal::ParamTraits<A4>::ForwardType a4,
        typename internal::ParamTraits<A5>::ForwardType a5
    ) const
    {
        PolymorphicInvoke f =
            reinterpret_cast<PolymorphicInvoke>(m_polymorphic_invoke);

        return f(
            m_invoker_storage.get(), a1,
            a2,
            a3,
            a4,
            a5
        );
    }

    bool operator==(const Function& rhs) const
    {
        return this->IsEqualTo(rhs);
    }

    Function& operator=(FunctionBase::ClearType* p)
    {
        assert(p == NULL);
        this->Clear();
        return *this;
    }

    void Swap(Function* other)
    {
        this->DoSwap(other);
    }

private:
    // inconsist param type may cause compiling error
    static void CheckSignatureConsistency(PolymorphicInvoke) {}
};

template <
    typename R,
    typename A1,
    typename A2,
    typename A3,
    typename A4,
    typename A5,
    typename A6
>
class Function<R (A1, A2, A3, A4, A5, A6)> :
    public internal::FunctionBase
{
private:
    typedef R (*PolymorphicInvoke)(internal::InvokerStorageBase*,
        typename internal::ParamTraits<A1>::ForwardType,
        typename internal::ParamTraits<A2>::ForwardType,
        typename internal::ParamTraits<A3>::ForwardType,
        typename internal::ParamTraits<A4>::ForwardType,
        typename internal::ParamTraits<A5>::ForwardType,
        typename internal::ParamTraits<A6>::ForwardType
    );

public:
    Function(FunctionBase::ClearType* p = NULL) : FunctionBase(NULL, NULL)
    {
        assert(p == NULL);
    }

    // We pass InvokerStorageHolder by const ref to avoid incurring an
    // unnecessary AddRef/Unref pair even though we will modify the object.
    // We cannot use a normal reference because the compiler will warn
    // since this is often used on a return value, which is a temporary.
    //
    // Note that this constructor CANNOT be explicit, and that Bind() CANNOT
    // return the exact Function<> type.  See base/function/bind.h for details.
    template <typename T>
    Function(const internal::InvokerStorageHolder<T>& invoker_holder) :
        FunctionBase(
            reinterpret_cast<InvokeFuncStorage>(&T::Invoker::DoInvoke),
            &invoker_holder.m_invoker_storage
        )
    {
        CheckSignatureConsistency(&T::Invoker::DoInvoke);
    }

    R operator()(
        typename internal::ParamTraits<A1>::ForwardType a1,
        typename internal::ParamTraits<A2>::ForwardType a2,
        typename internal::ParamTraits<A3>::ForwardType a3,
        typename internal::ParamTraits<A4>::ForwardType a4,
        typename internal::ParamTraits<A5>::ForwardType a5,
        typename internal::ParamTraits<A6>::ForwardType a6
    ) const
    {
        PolymorphicInvoke f =
            reinterpret_cast<PolymorphicInvoke>(m_polymorphic_invoke);

        return f(
            m_invoker_storage.get(), a1,
            a2,
            a3,
            a4,
            a5,
            a6
        );
    }

    bool operator==(const Function& rhs) const
    {
        return this->IsEqualTo(rhs);
    }

    Function& operator=(FunctionBase::ClearType* p)
    {
        assert(p == NULL);
        this->Clear();
        return *this;
    }

    void Swap(Function* other)
    {
        this->DoSwap(other);
    }

private:
    // inconsist param type may cause compiling error
    static void CheckSignatureConsistency(PolymorphicInvoke) {}
};

template <
    typename R,
    typename A1,
    typename A2,
    typename A3,
    typename A4,
    typename A5,
    typename A6,
    typename A7
>
class Function<R (A1, A2, A3, A4, A5, A6, A7)> :
    public internal::FunctionBase
{
private:
    typedef R (*PolymorphicInvoke)(internal::InvokerStorageBase*,
        typename internal::ParamTraits<A1>::ForwardType,
        typename internal::ParamTraits<A2>::ForwardType,
        typename internal::ParamTraits<A3>::ForwardType,
        typename internal::ParamTraits<A4>::ForwardType,
        typename internal::ParamTraits<A5>::ForwardType,
        typename internal::ParamTraits<A6>::ForwardType,
        typename internal::ParamTraits<A7>::ForwardType
    );

public:
    Function(FunctionBase::ClearType* p = NULL) : FunctionBase(NULL, NULL)
    {
        assert(p == NULL);
    }

    // We pass InvokerStorageHolder by const ref to avoid incurring an
    // unnecessary AddRef/Unref pair even though we will modify the object.
    // We cannot use a normal reference because the compiler will warn
    // since this is often used on a return value, which is a temporary.
    //
    // Note that this constructor CANNOT be explicit, and that Bind() CANNOT
    // return the exact Function<> type.  See base/function/bind.h for details.
    template <typename T>
    Function(const internal::InvokerStorageHolder<T>& invoker_holder) :
        FunctionBase(
            reinterpret_cast<InvokeFuncStorage>(&T::Invoker::DoInvoke),
            &invoker_holder.m_invoker_storage
        )
    {
        CheckSignatureConsistency(&T::Invoker::DoInvoke);
    }

    R operator()(
        typename internal::ParamTraits<A1>::ForwardType a1,
        typename internal::ParamTraits<A2>::ForwardType a2,
        typename internal::ParamTraits<A3>::ForwardType a3,
        typename internal::ParamTraits<A4>::ForwardType a4,
        typename internal::ParamTraits<A5>::ForwardType a5,
        typename internal::ParamTraits<A6>::ForwardType a6,
        typename internal::ParamTraits<A7>::ForwardType a7
    ) const
    {
        PolymorphicInvoke f =
            reinterpret_cast<PolymorphicInvoke>(m_polymorphic_invoke);

        return f(
            m_invoker_storage.get(), a1,
            a2,
            a3,
            a4,
            a5,
            a6,
            a7
        );
    }

    bool operator==(const Function& rhs) const
    {
        return this->IsEqualTo(rhs);
    }

    Function& operator=(FunctionBase::ClearType* p)
    {
        assert(p == NULL);
        this->Clear();
        return *this;
    }

    void Swap(Function* other)
    {
        this->DoSwap(other);
    }

private:
    // inconsist param type may cause compiling error
    static void CheckSignatureConsistency(PolymorphicInvoke) {}
};

template <
    typename R,
    typename A1,
    typename A2,
    typename A3,
    typename A4,
    typename A5,
    typename A6,
    typename A7,
    typename A8
>
class Function<R (A1, A2, A3, A4, A5, A6, A7, A8)> :
    public internal::FunctionBase
{
private:
    typedef R (*PolymorphicInvoke)(internal::InvokerStorageBase*,
        typename internal::ParamTraits<A1>::ForwardType,
        typename internal::ParamTraits<A2>::ForwardType,
        typename internal::ParamTraits<A3>::ForwardType,
        typename internal::ParamTraits<A4>::ForwardType,
        typename internal::ParamTraits<A5>::ForwardType,
        typename internal::ParamTraits<A6>::ForwardType,
        typename internal::ParamTraits<A7>::ForwardType,
        typename internal::ParamTraits<A8>::ForwardType
    );

public:
    Function(FunctionBase::ClearType* p = NULL) : FunctionBase(NULL, NULL)
    {
        assert(p == NULL);
    }

    // We pass InvokerStorageHolder by const ref to avoid incurring an
    // unnecessary AddRef/Unref pair even though we will modify the object.
    // We cannot use a normal reference because the compiler will warn
    // since this is often used on a return value, which is a temporary.
    //
    // Note that this constructor CANNOT be explicit, and that Bind() CANNOT
    // return the exact Function<> type.  See base/function/bind.h for details.
    template <typename T>
    Function(const internal::InvokerStorageHolder<T>& invoker_holder) :
        FunctionBase(
            reinterpret_cast<InvokeFuncStorage>(&T::Invoker::DoInvoke),
            &invoker_holder.m_invoker_storage
        )
    {
        CheckSignatureConsistency(&T::Invoker::DoInvoke);
    }

    R operator()(
        typename internal::ParamTraits<A1>::ForwardType a1,
        typename internal::ParamTraits<A2>::ForwardType a2,
        typename internal::ParamTraits<A3>::ForwardType a3,
        typename internal::ParamTraits<A4>::ForwardType a4,
        typename internal::ParamTraits<A5>::ForwardType a5,
        typename internal::ParamTraits<A6>::ForwardType a6,
        typename internal::ParamTraits<A7>::ForwardType a7,
        typename internal::ParamTraits<A8>::ForwardType a8
    ) const
    {
        PolymorphicInvoke f =
            reinterpret_cast<PolymorphicInvoke>(m_polymorphic_invoke);

        return f(
            m_invoker_storage.get(), a1,
            a2,
            a3,
            a4,
            a5,
            a6,
            a7,
            a8
        );
    }

    bool operator==(const Function& rhs) const
    {
        return this->IsEqualTo(rhs);
    }

    Function& operator=(FunctionBase::ClearType* p)
    {
        assert(p == NULL);
        this->Clear();
        return *this;
    }

    void Swap(Function* other)
    {
        this->DoSwap(other);
    }

private:
    // inconsist param type may cause compiling error
    static void CheckSignatureConsistency(PolymorphicInvoke) {}
};

// the following is taken from
// /usr/local/gcc4.5.1/include/c++/4.5.1/tr1/functional

// [3.7.2.7] null pointer comparisons

///  @brief Compares a polymorphic function object wrapper against 0
///  (the NULL pointer).
///  @returns @c true if the wrapper has no target, @c false otherwise
///
///  This function will not throw an %exception.
template<typename Signature>
inline bool operator==(
    const Function<Signature>& f,
    internal::FunctionBase::ClearType*)
{
    return !static_cast<bool>(f);
}

/// @overload
template<typename Signature>
inline bool operator==(
    internal::FunctionBase::ClearType*,
    const Function<Signature>& f)
{
    return !static_cast<bool>(f);
}

/// @brief Compares a polymorphic function object wrapper against 0
/// (the NULL pointer).
/// @returns @c false if the wrapper has no target, @c true otherwise
///
/// This function will not throw an %exception.
template<typename Signature>
inline bool operator!=(
    const Function<Signature>& f,
    internal::FunctionBase::ClearType*)
{
    return static_cast<bool>(f);
}

/// @overload
template<typename Signature>
inline bool operator!=(
    internal::FunctionBase::ClearType*,
    const Function<Signature>& f)
{
    return static_cast<bool>(f);
}

} // end of namespace common

// [3.7.2.8] specialized algorithms

// fit to STL
namespace std {
/// @brief Swap the targets of two polymorphic function object wrappers.
///
/// This function will not throw an %exception.
template<typename Signature>
inline void swap(::common::Function<Signature>& x, ::common::Function<Signature>& y)
{
    x.Swap(&y);
}

} // namespace std

#endif // COMMON_BASE_FUNCTION_FUNCTION_H
