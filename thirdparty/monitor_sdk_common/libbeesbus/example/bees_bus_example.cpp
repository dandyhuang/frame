//
// Created by 杨民善 on 2020/6/18.
//

#include "bees_bus_example.h"

#include <random>
#include <map>

using namespace vivo_bees_bus_cpp_report;

#define NOW_MILLI_SECONDS std::chrono::duration_cast< std::chrono::milliseconds >(std::chrono::system_clock::now().time_since_epoch())

static void getHeaders(std::map<std::string, std::string> &headers)
{
    headers.insert(std::pair<std::string, std::string>("appName", BEES_BUS_CPP_REPORT_APP_NAME));
    std::chrono::milliseconds ms = NOW_MILLI_SECONDS;
    std::ostringstream buf;
    buf << ms.count();
    headers.insert(std::pair<std::string, std::string>("data_timestamp", buf.str()));
    //headers.insert(std::pair<std::string, std::string>("topic", "general-api-test"));
    headers.insert(std::pair<std::string, std::string>("topic", "bees-sdk-test"));
    headers.insert(std::pair<std::string, std::string>("lines", "1"));

    // 测试环境 flume 参数
    headers.insert(std::pair<std::string, std::string>("project", "monitor-sdk"));
}

int reportToBees(const AppendContext &ctx)
{
    BeesBusThriftFlumeEvent event;
    getHeaders(event.headers);
    event.body = "fdajklfjsaldfkjlaadfdfdsfdfaqqeeferoi;dfdfd";
    //std::cout << "event body: " << event.body << std::endl;

    return BeesBusReport::append(ctx, event);
}

int reportToBeesBatch(const AppendContext &ctx)
{
    std::vector<BeesBusThriftFlumeEvent> events;
    const size_t size{500};
    for (size_t i = 0; i < size; ++i)
    {
        BeesBusThriftFlumeEvent event;
        getHeaders(event.headers);
        event.body = "fdajklfjsaldfkjlaldadpoewedfadf";
        std::cout << "event body: " << event.body << std::endl;

        events.push_back(event);
    }

    return BeesBusReport::appendBatch(ctx, events);
}

static std::atomic<bool> Terminate{false};

void updatTask()
{
    const std::string task{"bees-sdk-test_thrift"};
    bool isAdd{true};
    while (!Terminate)
    {
        if (isAdd)
        {
            isAdd = false;
            auto result = BeesBusReport::addTask(task);
            std::cout << "addTask result: " << task << "-" << result << std::endl;
        }
        else
        {
            isAdd = true;
            auto result = BeesBusReport::delTask(task);
            std::cout << "delTask result: " << task << "-" << result << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::seconds (20));
    }
}

void reportTask()
{
    AppendContext ctx{"bees-sdk-test_thrift"};
    while (!Terminate)
    {
        reportToBees(ctx);

        std::this_thread::sleep_for(std::chrono::seconds (1));
    }
}

int main(int argc, char** argv)
{
    const std::string beesManagerUrl{"http://10.101.32.107:8080"};
    const std::string localIp{""};

    std::vector<std::string> vtTaskName{"bees-sdk-test_thrift"};
    AppendContext ctx{"bees-sdk-test_thrift"};
    // VIVO_BEES_GLOG_LEVEL_INFO VIVO_BEES_GLOG_LEVEL_ERROR
    auto ret = BeesBusReport::init(beesManagerUrl, vtTaskName, DEFAULT_INTERVAL, localIp, VIVO_BEES_GLOG_LEVEL_INFO, BEES_THRIFT_PORT
                                   , DEFAULT_THRIFT_TIMEOUT, DEFAULT_THRIFT_TIMEOUT, DEFAULT_THRIFT_RETRY_MAX
                                   , VIVO_BEES_CPP_REPORT_METHOD_BEES, "10.101.51.11", 30305);
    Terminate = false;
    auto thredId = std::thread(updatTask);
    if (VIVO_BEES_CPP_REPORT_SUCCESS == ret)
    {
        //BeesBusReport::setLogLevel(VIVO_BEES_GLOG_LEVEL_ERROR);
        /*const size_t size{10000000};
        for (size_t i = 0; i < size; ++i)
        {
            reportToBees(ctx);
            std::this_thread::sleep_for(std::chrono::microseconds (1));
        }*/

        const size_t size{1};
        std::vector<std::thread> vtThread;
        for (size_t i = 0; i < size; ++i)
        {
            vtThread.emplace_back(std::thread(reportTask));
        }

        std::this_thread::sleep_for(std::chrono::seconds (600));
        Terminate = true;
        for (size_t i = 0; i < size; ++i)
        {
            vtThread[i].join();
        }

        thredId.join();

        BeesBusReport::destroy();
    }

    return 0;
}