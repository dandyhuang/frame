//
// Created by 王锦涛 on 2020-2-12.
//

#include <iostream>
#include <memory>
#include <string>

#include "thirdparty/monitor_sdk_common/libtrace/trace_manager.h"

using namespace std;
using namespace common;

int main(int argc, char const *argv[])
{
    string app_name = "TestApp";
    string loc = "SheKou";
    string env = ENV_DEV;

    TraceManager::Instance()->Init(app_name, loc, env);                 // 初始化TraceManager，设置应用名，机房，环境

    while(true){
        TracerPtr tracer = TraceManager::Instance()->StartTracer();         // 开始tracer

        SpanPtr span_1 = tracer->StartSpan("1");                            // 开始Span 1

        {
            {
                SpanPtr span_1_1 = tracer->StartSpan("1.1");                // 开始Span 1.1
                sleep(1);                                                   // 业务逻辑代码
                tracer->EndSpan(span_1_1);                                  // 结束Span 1.1
            }

            {
                SpanPtr span_1_2 = tracer->StartSpan("1.2");                // 开始Span 1.2
                sleep(1);                                                   // 业务逻辑代码
                tracer->EndSpan(span_1_2);                                  // 结束Span 1.2
            }
        }

        tracer->EndSpan(span_1);                                            // 结束Span1

        TraceManager::Instance()->EndTracer(tracer);                        // 结束tracer
    
        sleep(1);
    }

    return 0;
}