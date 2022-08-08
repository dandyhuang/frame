//
// Created by 吴婷 on 2020-06-15.
//

#include "beat/beat_info.h"
#include "utils/json_utils.h"

std::string BeatInfo::toString(){
    return JsonUtils::ToJSONString(*this);
}
