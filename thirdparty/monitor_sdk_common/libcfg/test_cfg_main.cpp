
#include <iostream>
#include <sstream>
#include <cstdlib>
#include "thirdparty/monitor_sdk_common/base/string/format.h"
#include "thirdparty/monitor_sdk_common/base/string/number.h"
#include "thirdparty/monitor_sdk_common/libcfg/config_listener.h"
#include "thirdparty/monitor_sdk_common/libcfg/config_logger.h"
#include "thirdparty/monitor_sdk_common/libcfg/config_manager.h"
#include "thirdparty/monitor_sdk_common/system/net/socket_address.h"
#include "thirdparty/monitor_sdk_common/base/string/algorithm.h"

#include "thirdparty/gflags/gflags.h"
#include "thirdparty/glog/logging.h"
#include "thirdparty/gtest/gtest.h"

using namespace std;
using namespace common;

int request_num = 0;

class TestConfigListener : public ConfigListener {
 public:
  TestConfigListener() {}
  ~TestConfigListener() {}

  void EventReceived(const PropertyItem &item, const ChangeEventType &type) {
    LOG(INFO) << "EventReceived: " << type;
    LOG(INFO) << "EventReceived: " << item.ToString();
  }

};


class TestConfigLogger: public ConfigLogger {
 public:
  TestConfigLogger() {}
  ~TestConfigLogger() {}

  void LogInfo(const std::string &msg) {
    LOG(INFO) << msg;
  }

  void LogError(const std::string &msg) {
    LOG(ERROR) << msg;
  }

  void LogWarn(const std::string &msg) {
    LOG(WARNING) << msg;
  }

  void LogDebug(const std::string &msg) {
    DLOG(INFO) << msg;
  }
};


int main(int argc, char** argv) {
    // ConfigManager::Instance()->SetAppName("demo");
    // ConfigManager::Instance()->SetAppEnv("dev");
    ConfigManager::Instance()->SetAppName("tars-app-server");
    ConfigManager::Instance()->SetAppEnv("test");
    ConfigManager::Instance()->SetConfigVersion("1");
    ConfigManager::Instance()->SetConfigHost("http://vivocfg-agent.test.vivo.xyz/vivocfg");
    //ConfigManager::Instance()->LoadLocalConfigFile("vivo.properties");
    // [选设：开启线程数, 默认为4]
    ConfigManager::Instance()->SetThreadNum(4);

    std::shared_ptr<ConfigListener> listener(new TestConfigListener());
    ConfigManager::Instance()->AddListener(listener);

    std::shared_ptr<ConfigLogger> logger(new TestConfigLogger());
    ConfigManager::Instance()->SetLogger(logger);

    ConfigManager::Instance()->Initialize();

    short s_ret;
    ConfigManager::Instance()->GetShort("db.maxpool", &s_ret);
    LOG(INFO) << "db.maxpool: " << s_ret << endl;

    int i_ret;
    ConfigManager::Instance()->GetInteger("key_int", &i_ret);
    LOG(INFO) << "key_int: " << i_ret << endl;

    long l_ret;
    ConfigManager::Instance()->GetLong("sysConfigRate", &l_ret);
    LOG(INFO) << "sysConfigRate: " << l_ret << endl;

    double d_ret;
    ConfigManager::Instance()->GetDouble("sysConfigTimeout", &d_ret);
    LOG(INFO) << "sysConfigTimeout: " << d_ret << endl;

    bool b_ret;
    ConfigManager::Instance()->GetBoolean("bool_test", &b_ret);
    LOG(INFO) << "bool_test: " << b_ret << endl;

    cout << "\n\n全量返回配置内容>>>>" << endl;
    // 全量返回配置
    std::map<std::string, std::string> props;
    props = ConfigManager::Instance()->GetProps();
    for (auto kv : props) {
        cout << kv.first << ": " << kv.second << endl;
    }
}
