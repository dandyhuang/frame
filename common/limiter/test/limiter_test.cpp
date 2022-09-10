#include <brpc/channel.h>
#include <gflags/gflags.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <bvar/bvar.h>
#include <omp.h>
#include "protocol/mixer/mixer.pb.h"
#include "mixer_common/loghelper.h"
//#include "pb_to_json.h"

DEFINE_string(protocol, "baidu_std", "Protocol type. Defined in protocol/brpc/options.proto");
DEFINE_string(connection_type, "", "Connection type. Available values: single, pooled, short");
DEFINE_string(server, "0.0.0.0:17800", "IP Address of server");
DEFINE_string(load_balancer, "", "The algorithm for load balancing");
DEFINE_int32(timeout_ms, 100, "RPC timeout in milliseconds");
DEFINE_int32(backup_request_ms, 70, "backup_request time in milliseconds");
DEFINE_int32(max_retry, 1, "Max retries(not including the first RPC)");
DEFINE_int64(log_id, -1, "log id");
DEFINE_bool(isloop, true,"is loop");
DEFINE_int32(time_step_ms, 1000, "time_step_ms");
DEFINE_int32(run_times, 20000, "run_times");

int main(int argc, char ** argv) {
    // Parse gflags. We recommend you to use gflags as well.
    google::ParseCommandLineFlags(&argc, &argv, true);

    // Initialize the channel, NULL means using default options.
    brpc::ChannelOptions options;
    options.protocol = FLAGS_protocol;
    options.connection_type = FLAGS_connection_type;
    options.timeout_ms = FLAGS_timeout_ms;
    options.backup_request_ms = FLAGS_backup_request_ms;
    options.max_retry = FLAGS_max_retry;

    brpc::Channel channel;
    std::string channel_name = "test_channel";
    if (channel.Init(FLAGS_server.c_str(), FLAGS_load_balancer.c_str(), &options) != 0) {
        VLOG_APP(ERROR) << "Fail to initialize channel";
        return -1;
    }

    // Normally, you should not call a Channel directly, but instead construct
    // a stub Service wrapping it. stub can be shared by all threads as well.
    rec::MixerService_Stub stub(&channel);

    int run_times = 1;
    if (FLAGS_isloop) {
        run_times = FLAGS_run_times;
    }

    omp_set_num_threads(3);

    bvar::LatencyRecorder latency_recorder;
    bvar::Adder<int64_t> send_count;
    bvar::Adder<int64_t> backup_count;
    bvar::Adder<int64_t> failed_count;
    bvar::Adder<int64_t> success_count;

    #pragma omp parallel for
    for (int i = 0; i < run_times; ++i) {
        int64_t log_id = FLAGS_log_id;
        if (log_id == -1) {
            log_id = static_cast<int64_t>(time(NULL));
        }

        ::rec::MixerRequest request;
        ::rec::MixerResponse response;
    
        brpc::Controller cntl;
        cntl.set_log_id(log_id);

        int64_t real_backup_request_ms = FLAGS_backup_request_ms;
        if (FLAGS_backup_request_ms > 0) {
            cntl.set_backup_request_ms(real_backup_request_ms);
        }
        stub.Rank(&cntl, &request, &response, NULL);
        int64_t cost_ms = cntl.latency_us()/1000;
        /*
        std::string res_str;
        std::string error;
        ProtoMessageToJson(response, &res_str, &error);
        */
        send_count << 1;
        if (!cntl.Failed()) {
            if (cntl.has_backup_request()) {
                VLOG_APP(NOTICE) << "logid:" << log_id 
                    << " response latency_ms: " << cost_ms 
                    << ", set_backup_ms: " << real_backup_request_ms
                    << ", backup: " << cntl.has_backup_request();
            }
            success_count << 1;
            if (cntl.has_backup_request()) {
                backup_count << 1;
            }
        } else {
            failed_count << 1;
            VLOG_APP(WARNING) << "logid:" << log_id << " response latency_ms: "<< cost_ms
                << ", failed";
        }
        latency_recorder << cost_ms;
    }

     printf("[Count]\n"
        "  send     %lld\n"
        "  success  %lld\n"
        "  backup   %lld\n"
        "  failed   %lld\n",
        (long long)send_count.get_value(),
        (long long)success_count.get_value(),
        (long long)backup_count.get_value(),
        (long long)failed_count.get_value());
  
    return 0;
}
