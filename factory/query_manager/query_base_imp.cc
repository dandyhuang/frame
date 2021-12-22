//  Copyright [2021.05] <vivo inc>
#include <iostream>
#include <memory>
#include <set>

#include "factory/query_manager/query_base.h"
#include "factory/query_manager/query_factory.h"

namespace dandyhuang {

REGISTER_QUERY(QueryBase)
int QueryBase::QueryData() { std::cout << "do something" << std::endl; }

}  // end namespace dandyhuang
