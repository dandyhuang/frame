// Copyright (c) 2010, Vivo Inc.
// All rights reserved.
//

#ifndef COMMON_SYSTEM_TIME_STOPWATCH_H
#define COMMON_SYSTEM_TIME_STOPWATCH_H

#include "thirdparty/monitor_sdk_common/base/stdint.h"

namespace common {

/// ����࣬���ڼ�ʱ
class Stopwatch
{
public:
    /// @param auto_start �Ƿ���ʱ�Զ���ʼ
    explicit Stopwatch(bool auto_start = true);

    /// ��ʼ��ʱ���Ѿ��ۼ�ʱ������ۼ�
    void Start();

    /// ֹͣ��ʱ���ۼ�ʱ�䲻������
    void Stop();

    /// ��ʱ��λΪ 0
    void Restart();

    /// �����ۻ�������
    double ElapsedSeconds() const;

    /// �����ۻ���΢����
    int64_t ElapsedMicroSeconds() const;

    /// �����ۻ��ĺ�����
    int64_t ElapsedMilliSeconds() const;

    /// �Ƿ��ڼ�ʱ
    bool IsRunning() const;

    /// ֹͣ��ʱ����ʱ��λΪ 0��
    void Reset();
private:
    Stopwatch(const Stopwatch&);
    Stopwatch& operator=(const Stopwatch&);
private:
    int64_t LastInterval() const;
private:
    bool m_is_running;
    int64_t m_cumulated_time; ///< ���۵��ۼ�ʱ��
    int64_t m_start_time; ///< �ϴ�������ʱ��
};

} // namespace common

#endif // COMMON_SYSTEM_TIME_STOPWATCH_H
