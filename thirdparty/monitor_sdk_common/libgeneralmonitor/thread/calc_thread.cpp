//
// Created by 杨民善 on 2020/6/30.
//

#include "calc_thread.h"

namespace vivo_general_monitor_cpp_report
{
    void CalcThread::calcTask(MonitorContext &ctx)
    {
        const std::thread::id CURRENT_THREAD_ID(std::this_thread::get_id());
        while (!ctx.m_calcIsTerminate)
        {
            QueueData *pQueueData = NULL;
            while (G_P_LOCK_FREE_QUEUE && G_P_LOCK_FREE_QUEUE->pop(pQueueData))
            {
                if (pQueueData)
                {
                    ctx.handleMapStat(*pQueueData, CURRENT_THREAD_ID);
                    delete pQueueData;
                    pQueueData = NULL;
                }
                else
                {
                    ++ctx.m_statInfo.disruptProcessFail;

                    LOG(ERROR) << "pQueueData is null may be alloc memory failed!" << std::endl;
                }
            }

            std::this_thread::sleep_for(std::chrono::milliseconds (10));
        }
    }
}