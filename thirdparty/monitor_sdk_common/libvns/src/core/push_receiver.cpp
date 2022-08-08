//
// Created by 吴婷 on 2020-06-17.
//
#include <iostream>

#include "core/push_receiver.h"
#include "core/host_reactor.h"
#include "utils/log_utils.h"
#include "utils/json_utils.h"

using namespace common;

PushReceiver::PushReceiver(HostReactor* host_reactor){
    SERVER_PORT = PropertyKeyConst::UDP_PORT;

    _host_reactor = host_reactor;

    // 获取套接字文件描述符
    _server_fd = socket(AF_INET, SOCK_DGRAM, 0); //AF_INET:IPV4;SOCK_DGRAM:UDP
    if (_server_fd < 0) {
        LogUtils::LogError("create socket fail!");
        return;
    }

    // 将套接字和ip、端口绑定
    int ret = -1;
    int try_max_num = 3;    // 端口绑定重试3次，自动生成
    int try_count = 0;
    do {
        // 首次之后，自动生成新的端口
        if(try_count > 0){
            int port = (rand() % (10000)) + 1;  // 生成随机数：(0, 10000]
            PropertyKeyConst::UDP_PORT = port;  // 全局udp端口
            SERVER_PORT = PropertyKeyConst::UDP_PORT;
            LogUtils::LogInfo(StringPrint("new udp port: %d", SERVER_PORT));
        }

        struct sockaddr_in ser_addr;
        memset(&ser_addr, 0, sizeof(ser_addr));
        ser_addr.sin_family = AF_INET;
        ser_addr.sin_addr.s_addr = htonl(INADDR_ANY); //IP地址，需要进行网络序转换，INADDR_ANY：本地地址【自动获取ip地址】
        ser_addr.sin_port = htons(SERVER_PORT);  //端口号，需要网络序转换

        // 绑定socket
        ret = bind(_server_fd, (struct sockaddr*)&ser_addr, sizeof(ser_addr));

        try_count++;
    } while ((ret<0) && (try_count<try_max_num));

    if (ret < 0) {
        LogUtils::LogError(StringPrint("Try [%d] count, socket bind fail!", try_max_num));
    } else {
        LogUtils::LogInfo(StringPrint("socket bind success, port: %d", SERVER_PORT));
    }
}

PushReceiver::~PushReceiver(){
    close(_server_fd);
}

// 具体任务内容
void PushReceiver::run() {
    char buf[BUFF_LEN];  //接收缓冲区，1024字节
    socklen_t len;

    int count;
    struct sockaddr_in clent_addr;  //clent_addr用于记录发送方的地址信息
    // 一直等待消息处理
    while(1)
    {
        memset(buf, 0, BUFF_LEN);
        len = sizeof(clent_addr);
        count = recvfrom(_server_fd, buf, BUFF_LEN, 0, (struct sockaddr*)&clent_addr, &len);  //recvfrom是拥塞函数，没有数据就一直拥塞
        if(count == -1){
            LogUtils::LogError("recieve data fail!");
            return;
        }

        std::string buff = buf;
        LogUtils::LogInfo(StringPrint("vns server push msg: [%s]", buff));    //打印client发过来的信息

        // 对接收消息buf的处理
        std::shared_ptr<PushPacket> push_packet = std::make_shared<PushPacket>();   // 接收消息的类
        JsonUtils::JsonStr2PushPacket(buf, push_packet);
        std::string ack;    // 确认回复消息
        if("dom" == push_packet->type || "service" == push_packet->type){
            LogUtils::LogInfo("PushReceiver push 主动更新 HostReactor！");
            _host_reactor->processServiceJSON(push_packet->data);
            // ack
            PushPacket push_ack;      // ack消息
            push_ack.type = "push-ack";
            push_ack.last_ref_time = push_packet->last_ref_time;
            push_ack.data = "";
            ack = push_ack.toString();
        } else if ("dump" == push_packet->type) {
            // TODO
        } else {
            PushPacket push_ack;      // ack消息
            push_ack.type = "unknown-ack";
            push_ack.last_ref_time = push_packet->last_ref_time;
            push_ack.data = "";
            ack = push_ack.toString();
        }

        // TODO udp消息回复
        /*
        // 消息回复确认：ack
        memset(buf, 0, BUFF_LEN);
        int size = ack.length() < BUFF_LEN ? ack.length() : BUFF_LEN;
        for (int i = 0 ; i < size; ++i) {
            buf[i] = ack[i];
        }
        std::cout << "vns 消息回复:" << buf << std::endl;
        int send_num = sendto(_server_fd, buf, BUFF_LEN, 0, (struct sockaddr*)&clent_addr, len);  //发送信息给client，注意使用了clent_addr结构体指针
        if(send_num < 0){
            printf("sendto error %d!", send_num);
        } else {
            printf("sendto 成功!");
        }
         */

    }

}

int PushReceiver::getUDPPort(){
    return SERVER_PORT;
}

std::string PushPacket::toString(){
    return JsonUtils::ToJSONString(*this);
}