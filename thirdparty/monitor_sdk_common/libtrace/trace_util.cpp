//
// Created by 王锦涛 on 2020-2-12.
//

#include "thirdparty/monitor_sdk_common/libtrace/trace_util.h"

using namespace common;

////////////////////TraceID////////////////////
TraceID::TraceID() {
    serial_number = 0;
}

std::string TraceID::GenTraceId(const std::string & env, const std::string & ip_str) {
    // pid_str
    std::stringstream ss;
    ss << std::hex << getpid();
    std::string pid_str = ss.str();
    if (pid_str.size() > 4) {
        pid_str = pid_str.substr(pid_str.size() - 4);
    }
    // encoded_ip
    std::string encoded_ip = EncodeIp(ip_str);
    // timestamp
    std::string timestamp = std::to_string(TraceTime::Instance()->GetTimeMS());
    // seqStr
    serial_number = serial_number++ % CYCLE;
    std::string seq_str = std::to_string(serial_number);
    if (seq_str.size() < 4) {
        seq_str.insert (0, 4 - seq_str.size(), '0');
    }

    std::string trace_id = "";
    trace_id = pid_str + encoded_ip + env + timestamp + seq_str;
    return trace_id;
}

std::string TraceID::EncodeIp(const std::string & ip_str) {
    long ip[4];
    std::string tmp = "";
    int j = 0;

    for (int i = 0; i < (int)ip_str.size(); ++i) {
        if (ip_str[i] != '.') {
            tmp += ip_str[i];
        }
        else {
            if (j < 3) {
                ip[j] = atoi(tmp.c_str());
                tmp = "";
                j++;
            } else {
                return "00000000";   // 返回一个值不至于程序出错
            }
        }
    }
    ip[j] = atoi(tmp.c_str());
    long ip_long = (ip[0] << 24) + (ip[1] << 16) + (ip[2] << 8) + ip[3];

    std::stringstream ss;
    ss << std::hex << ip_long;
    std::string encoded_ip = ss.str();
    if (encoded_ip.size() != 8) {
        encoded_ip = "0" + encoded_ip;
    }

    return encoded_ip;
}

////////////////////TraceTime////////////////////
int64_t TraceTime::GetTimeS() {
    using namespace std::chrono;
    seconds s = duration_cast<seconds>(system_clock::now().time_since_epoch());
    return int64_t(s.count());
}

int64_t TraceTime::GetTimeMS() {
    using namespace std::chrono;
    milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    return int64_t(ms.count());
}