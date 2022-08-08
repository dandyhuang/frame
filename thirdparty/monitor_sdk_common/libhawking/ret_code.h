//
// Created by 陈嘉豪 on 2019/12/3.
//

#ifndef COMMON_LIBHAWKING_RET_CODE_H
#define COMMON_LIBHAWKING_RET_CODE_H

#include <string>

namespace common {

typedef struct Code {
    int code;
    std::string message;
    std::string to_string_val;
    
    Code(int code, std::string message) {
        this->code = code;
        this->message = message;
        this->to_string_val =  "[" + std::to_string(code) + "]" + message;
    }
} retCode;

struct RetCode {
    static retCode OK;
    static retCode MODULE_CODE_ERROR;
    static retCode TEST_CODE_ERROR;
    static retCode TEST_STATUS_ERROR;
    static retCode HTTP_PARAM_ERROR;
};

} // namespace common

#endif //COMMON_LIBHAWKING_RET_CODE_H
