//
// Created by 黄伟锋 on 2019-04-15.
//

#ifndef COMMON_LIBCFG_COMMON_VO_H_
#define COMMON_LIBCFG_COMMON_VO_H_

#include <string>


namespace common {

template<class T>
class CommonVO {
 public:
  // 返回码
  int retcode = 0;

  // 返回文本消息
  std::string message;

  // 业务数据
  T data;
};

} // namespace common

#endif //COMMON_LIBCFG_COMMON_VO_H_
