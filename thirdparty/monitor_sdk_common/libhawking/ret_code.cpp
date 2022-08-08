//
// Created by 陈嘉豪 on 2019/12/10.
//

#include "thirdparty/monitor_sdk_common/libhawking/ret_code.h"

using namespace common;

retCode RetCode::OK{0, "处理成功"};

retCode RetCode::MODULE_CODE_ERROR{1, "模块编码错误"};

retCode RetCode::TEST_CODE_ERROR{2, "实验编码错误"};

retCode RetCode::TEST_STATUS_ERROR{3, "实验状态错误"};

retCode RetCode::HTTP_PARAM_ERROR{4, "网络请求参数异常"};
