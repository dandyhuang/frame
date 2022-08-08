//
// Created by 吴婷 on 2020-07-22.
//

#include "object/token_info.h"
#include "utils/json_utils.h"

std::string TokenInfo::toString(){
    return JsonUtils::ToJSONString(*this);
}

std::string ResponseError::toString(){
    return JsonUtils::ToJSONString(*this);
}