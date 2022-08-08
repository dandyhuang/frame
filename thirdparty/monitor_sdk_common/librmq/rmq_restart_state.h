//
// Created by 黄伟锋 on 2019-04-24.
//

#ifndef COMMON_LIBRMQ_RMQ_RESTART_STATE_H_
#define COMMON_LIBRMQ_RMQ_RESTART_STATE_H_

namespace common {

enum RmqRestartState {
  //运行中
  RUNNING,
  //重启中
  RESTARTING,
  //重启成功
  RESTARTSUCCESS,
  //重启失败
  RESTARTFAIL,
  //重启结束
  RESTARTED,
};

} // namespace common

#endif //COMMON_LIBRMQ_RMQ_RESTART_STATE_H_
