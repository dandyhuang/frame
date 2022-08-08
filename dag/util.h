// Authors: weitao.zhang@vivo.com

#ifndef REC_UTIL_H_
#define REC_UTIL_H_

#include <string>
#include <memory>
#include "google/protobuf/message.h"
#include <google/protobuf/util/json_util.h>
#include <set>
#include <vector>
#include <unordered_map>

namespace Rec {
namespace Util {

class Reloadable {

    // 
    virtual int Reload() = 0;
};

// get time in seconds
inline double get_cur_time() {
    struct timespec tp;
    clock_gettime(CLOCK_REALTIME, &tp);
    return tp.tv_sec + tp.tv_nsec * 1e-9;
} 
int64_t get_cur_time_us();

std::shared_ptr<google::protobuf::Message> createMessage(const std::string& typeName);

std::string message_to_json(const google::protobuf::Message& message);

class TimeProfiler {
public:
    TimeProfiler() {
        _start_time = get_cur_time();
    }

    uint64_t Start() {
        _last_start_time = get_cur_time();
        return (_last_start_time - _start_time)*1e6; 
    }

    uint64_t Stop() {
        _last_end_time = get_cur_time();
        return (_last_end_time - _last_start_time)*1e6;
    }

    uint64_t TotalTimeEclapse() {
        _end_time = get_cur_time();
        return (_end_time - _start_time)*1e6;
    }
private:
    double _start_time;
    double _end_time;
    double _last_start_time;
    double _last_end_time;
};

}
}
#endif // REC_UTIL_H_
