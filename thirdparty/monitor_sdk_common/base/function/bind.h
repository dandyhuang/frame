//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-14 13:33
//  @file:      bind.h
//  @author:
//  @brief:
//
//********************************************************************


#ifndef COMMON_BASE_FUNCTION_BIND_H
#define COMMON_BASE_FUNCTION_BIND_H
#pragma once

#include "thirdparty/monitor_sdk_common/base/function/invoker_storage.h"
#include "thirdparty/monitor_sdk_common/base/function/function.h"

// See base/function.h for how to use these functions.
//
// IMPLEMENTATION NOTE
// Though Bind()'s result is meant to be stored in a Function<> type, it
// cannot actually return the exact type without requiring a large amount
// of extra template specializations. The problem is that in order to
// discern the correct specialization of Function<>, Bind would need to
// unwrap the function signature to determine the signature's arity, and
// whether or not it is a method.
//
// Each unique combination of (arity, function_type, num_prebound) where
// function_type is one of {function, method, const_method} would require
// one specialization.  We eventually have to do a similar number of
// specializations anyways in the implementation (see the FunctionTraitsN,
// classes).  However, it is avoidable in Bind if we return the result
// via an indirection like we do below.

// GLOBAL_NOLINT(whitespace/operators)

namespace common {

template <typename Signature>
internal::InvokerStorageHolder<internal::InvokerStorage0<Signature> >
Bind(Signature f)
{
    return internal::MakeInvokerStorageHolder(
        new internal::InvokerStorage0<Signature>(f)
    );
}

template <
    typename Signature,
    typename P1
>
internal::InvokerStorageHolder<internal::InvokerStorage1<
        Signature,
        P1
    >
>
Bind(
    Signature f,
    const P1& p1
)
{
    return internal::MakeInvokerStorageHolder(
        new internal::InvokerStorage1<Signature, P1>(
            f,
            p1
        )
    );
}

template <
    typename Signature,
    typename P1,
    typename P2
>
internal::InvokerStorageHolder<internal::InvokerStorage2<
        Signature,
        P1,
        P2
    >
>
Bind(
    Signature f,
    const P1& p1,
    const P2& p2
)
{
    return internal::MakeInvokerStorageHolder(
        new internal::InvokerStorage2<Signature, P1, P2>(
            f,
            p1,
            p2
        )
    );
}

template <
    typename Signature,
    typename P1,
    typename P2,
    typename P3
>
internal::InvokerStorageHolder<internal::InvokerStorage3<
        Signature,
        P1,
        P2,
        P3
    >
>
Bind(
    Signature f,
    const P1& p1,
    const P2& p2,
    const P3& p3
)
{
    return internal::MakeInvokerStorageHolder(
        new internal::InvokerStorage3<Signature, P1, P2, P3>(
            f,
            p1,
            p2,
            p3
        )
    );
}

template <
    typename Signature,
    typename P1,
    typename P2,
    typename P3,
    typename P4
>
internal::InvokerStorageHolder<internal::InvokerStorage4<
        Signature,
        P1,
        P2,
        P3,
        P4
    >
>
Bind(
    Signature f,
    const P1& p1,
    const P2& p2,
    const P3& p3,
    const P4& p4
)
{
    return internal::MakeInvokerStorageHolder(
        new internal::InvokerStorage4<Signature, P1, P2, P3, P4>(
            f,
            p1,
            p2,
            p3,
            p4
        )
    );
}

template <
    typename Signature,
    typename P1,
    typename P2,
    typename P3,
    typename P4,
    typename P5
>
internal::InvokerStorageHolder<internal::InvokerStorage5<
        Signature,
        P1,
        P2,
        P3,
        P4,
        P5
    >
>
Bind(
    Signature f,
    const P1& p1,
    const P2& p2,
    const P3& p3,
    const P4& p4,
    const P5& p5
)
{
    return internal::MakeInvokerStorageHolder(
        new internal::InvokerStorage5<Signature, P1, P2, P3, P4, P5>(
            f,
            p1,
            p2,
            p3,
            p4,
            p5
        )
    );
}

template <
    typename Signature,
    typename P1,
    typename P2,
    typename P3,
    typename P4,
    typename P5,
    typename P6
>
internal::InvokerStorageHolder<internal::InvokerStorage6<
        Signature,
        P1,
        P2,
        P3,
        P4,
        P5,
        P6
    >
>
Bind(
    Signature f,
    const P1& p1,
    const P2& p2,
    const P3& p3,
    const P4& p4,
    const P5& p5,
    const P6& p6
)
{
    return internal::MakeInvokerStorageHolder(
        new internal::InvokerStorage6<Signature, P1, P2, P3, P4, P5, P6>(
            f,
            p1,
            p2,
            p3,
            p4,
            p5,
            p6
        )
    );
}

template <
    typename Signature,
    typename P1,
    typename P2,
    typename P3,
    typename P4,
    typename P5,
    typename P6,
    typename P7
>
internal::InvokerStorageHolder<internal::InvokerStorage7<
        Signature,
        P1,
        P2,
        P3,
        P4,
        P5,
        P6,
        P7
    >
>
Bind(
    Signature f,
    const P1& p1,
    const P2& p2,
    const P3& p3,
    const P4& p4,
    const P5& p5,
    const P6& p6,
    const P7& p7
)
{
    return internal::MakeInvokerStorageHolder(
        new internal::InvokerStorage7<Signature, P1, P2, P3, P4, P5, P6, P7>(
            f,
            p1,
            p2,
            p3,
            p4,
            p5,
            p6,
            p7
        )
    );
}

template <
    typename Signature,
    typename P1,
    typename P2,
    typename P3,
    typename P4,
    typename P5,
    typename P6,
    typename P7,
    typename P8
>
internal::InvokerStorageHolder<internal::InvokerStorage8<
        Signature,
        P1,
        P2,
        P3,
        P4,
        P5,
        P6,
        P7,
        P8
    >
>
Bind(
    Signature f,
    const P1& p1,
    const P2& p2,
    const P3& p3,
    const P4& p4,
    const P5& p5,
    const P6& p6,
    const P7& p7,
    const P8& p8
)
{
    return internal::MakeInvokerStorageHolder(
        new internal::InvokerStorage8<Signature, P1, P2, P3, P4, P5, P6, P7,
            P8>(
            f,
            p1,
            p2,
            p3,
            p4,
            p5,
            p6,
            p7,
            p8
        )
    );
}

} // end of namespace common
#endif // COMMON_BASE_FUNCTION_BIND_H
