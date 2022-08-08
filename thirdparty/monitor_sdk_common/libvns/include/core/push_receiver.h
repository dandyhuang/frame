//
// Created by 吴婷 on 2020-06-17.
//

#ifndef COMMON_LIBVNS_PUSH_RECEIVER_H
#define COMMON_LIBVNS_PUSH_RECEIVER_H

#include <memory>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#include "thirdparty/monitor_sdk_common/base/functional.h"
#include "thirdparty/monitor_sdk_common/system/threading/thread_pool.h"
#include "thirdparty/monitor_sdk_common/system/threading/mutex.h"
#include "thirdparty/monitor_sdk_common/system/threading/this_thread.h"

#include "thirdparty/monitor_sdk_common_params.h"
#include "utils/asyn_queue.h"

#define BUFF_LEN (64 * 1024)    // 接收的字节数

namespace common {
class HostReactor;  // 相互引用

/**
 * 接受服务端的UDP推送(作为server)
 */
class PushReceiver : public WorkItem{
public:
    explicit PushReceiver(HostReactor* host_reactor);
    ~PushReceiver();

    void run();
    int getUDPPort();

public:
    int SERVER_PORT;   // UDP端口，默认为1111

private:
    HostReactor* _host_reactor;
    int _server_fd;     // 监听端口的套接口文件描述符
};

/**
 * push推送数据类
 */
class PushPacket {
public:
    std::string toString();

public:
    std::string type;
    long last_ref_time;
    std::string data;

};

} //namespace common

#endif //COMMON_LIBVNS_PUSH_RECEIVER_H
