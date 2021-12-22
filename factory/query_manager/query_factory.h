/***************************************************************************
 *
 * Copyright (c) 2021 dandy.com, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/

 /**
 * @file query_factory.h
 * @author 
 * @date 2021/06/24 15:32:07
 * @version $Revision$
 * @brief
 *
 **/

#pragma once

#include "factory/factory.h"
#include "factory/query_manager/query_base.h"

namespace dandyhuang {

using QueryFactory = dandyhuang::Factory<QueryBase>;

inline QueryFactory& query_factory() {
    static QueryFactory _query_factory;
    return _query_factory;
}

#define REGISTER_QUERY(QueryClass) \
struct Register##QueryClass { \
    Register##QueryClass() { \
        dandyhuang::query_factory().add<QueryClass>(#QueryClass); \
    } \
};\
static Register##QueryClass QueryClass##_t;

#define GET_QUERY(name) \
dandyhuang::query_factory().get(name);

}  // namespace dandyhuang
