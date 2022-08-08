//
// Created by 陈嘉豪 on 2019/11/28.
//

#ifndef COMMON_LIBHAWKING_GRAY_TEST_QUERY_CLIENT_H
#define COMMON_LIBHAWKING_GRAY_TEST_QUERY_CLIENT_H

#include <exception>
#include <memory>
#include <string>

#include "thirdparty/monitor_sdk_common/base/scoped_ptr.h"
#include "thirdparty/monitor_sdk_common/libhawking/gray_test_cache_manager.h"
#include "thirdparty/monitor_sdk_common/libhawking/gray_test_config.h"
#include "thirdparty/monitor_sdk_common/libhawking/gray_test_log_utils.h"
#include "thirdparty/monitor_sdk_common/libhawking/gray_test_logger.h"
#include "thirdparty/monitor_sdk_common/libhawking/gray_test_scheduled_executor.h"
#include "thirdparty/monitor_sdk_common/libhawking/gray_test_query_service.h"
#include "thirdparty/monitor_sdk_common/libhawking/item.h"
#include "thirdparty/monitor_sdk_common/libhawking/query_client.h"
#include "thirdparty/monitor_sdk_common/libhawking/ret_code.h"
#include "thirdparty/monitor_sdk_common/net/http/client.h"

#include "thirdparty/jsoncpp/json.h"

namespace common {

class GrayTestQueryClient : public QueryClient {
public:
    GrayTestQueryClient(std::string business_code, std::string domain, std::string env, bool init = true,
            std::shared_ptr<GrayTestLogger> logger = nullptr);
    virtual ~GrayTestQueryClient() {}
    
    void SetLogger(std::shared_ptr<GrayTestLogger> logger);
    
    void Init();
    
    void InitializeScheduledExecutor();
    
    bool IsStartDependOn();
    
    void RefreshCache(int refresh_type);
    
    void StartRemoteFetchTask(int delay_second);
    
    bool RemoteFetch(std::shared_ptr<GrayTestRemoteResult> gray_test_remote_result);
    
    std::shared_ptr<TestPlanQuery> QueryTestPlan(std::string module_code,
            std::string test_code, std::map<std::string, std::string> filter) override;
    
    struct GrayTestSdkStartException : public std::exception {
        //TODO(chenjiahao)
    };
    
private:
    std::string business_code_;
    std::string domain_;
    std::string env_;
    
    // 定时拉取远端配置的线程池
    std::shared_ptr<GrayTestScheduledExecutor> scheduled_executor_;
};

} // namespace common

#endif //COMMON_LIBHAWKING_GRAY_TEST_QUERY_CLIENT_H
