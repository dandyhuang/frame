//
// Created by 陈嘉豪 on 2019/12/16.
//

#ifndef COMMON_LIBHAWKING_ID_UTILS_H
#define COMMON_LIBHAWKING_ID_UTILS_H

#include "thirdparty/monitor_sdk_common/base/singleton.h"

namespace common {

class IdUtils : public SingletonBase<IdUtils> {
public:
    static int GetNextId() {
        return Instance()->id_++;
    }
    
    static void ResetId() {
        Instance()->id_ = 0;
    }

private:
    int id_ = 0;
};

} // namespace common

#endif //COMMON_LIBHAWKING_ID_UTILS_H
