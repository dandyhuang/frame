//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 19:10
//  @file:      datetime.cpp
//  @brief:     
//
//********************************************************************


#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "thirdparty/monitor_sdk_common/system/time/datetime.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <sys/timeb.h>
#include <sys/types.h>
#include <time.h>
#include <sstream>
#include <string>

#ifdef _WIN32
#include "thirdparty/monitor_sdk_common/base/common_windows.h"
#else
#include <sys/time.h>
#include <unistd.h>
#endif

// GLOBAL_NOLINT(runtime/printf)
// GLOBAL_NOLINT(readability/casting)

using namespace std;

namespace common {

#define MICROSECONDS_PER_SECOND 1000000
#define SECONDS_PER_HOUR 3600
#define SECONDS_PER_DAY 86400

#define IS_ALPHA(x)             ( (x >= 'a' && x < 'z') || (x >= 'A' && x < 'Z') )
#define IS_DIGIT(x)             (x >= '0' && x <= '9')

#define IS_SEPARATE_CHAR(x) \
    (x == '\'' || x == '"' || x == '\\' || x == '/' || x == ':' || x == '%')

#define IS_PATTERN_CHAR(x) \
    (x == 'd' || x == 'f' || x == 'F' || x == 'g' || x == 'h' || x == 'H' || \
     x == 'K' || x == 'm' || x == 'M' || x == 's'||x == 't'||x == 'y'||x == 'z')

#define IS_BLANK_SPACE_CHAR(x)  (x == ' ' || x == '\t' || x == '\r' || x == '\n')

// ��һ���ַ�����ͷ���õ��ַ���
// strSrc    Դ�ַ���
// dwIndex,  ��ʼλ�ã����ؽ���λ��
// num,  ����ֵ��Ĭ����0
// dwMaxNumLen   ������ĳ���
// bJumpHeadSpace    �Ƿ�������dwIndex��ͷ�Ŀհ��ַ�
bool GetNumFromString(
    const string& strSrc, size_t& dwIndex, int& num,
    const size_t dwMaxNumLen = string::npos,
    bool bJumpHeadSpace = true)
{
    num = 0;

    if (bJumpHeadSpace)
    {
        while (IS_BLANK_SPACE_CHAR(strSrc[dwIndex]) && dwIndex < strSrc.length())
        {
            dwIndex++;
        }

        if (dwIndex == strSrc.length())
        {
            return false;
        }
    }

    if (!(dwIndex < strSrc.length() && IS_DIGIT(strSrc[dwIndex])))
    {
        return false;
    }

    for (size_t i = 0; i < dwMaxNumLen; i++)
    {
        char ch = strSrc[dwIndex];

        if (IS_DIGIT(ch))
        {
            num  = num * 10 + ch - '0';
            dwIndex++;
        }
        else
            break;
    }

    return true;
}

// ����ַ���ָ�������Ƿ��ǿ��ַ�
bool IsSpaceChars(const string& strSrc, size_t dwBegin, size_t dwCount)
{
    size_t dwEnd = dwCount + dwBegin > strSrc.length() ? strSrc.length() : dwCount + dwBegin;

    for (; dwBegin < dwEnd; dwBegin++)
    {
        if (!IS_BLANK_SPACE_CHAR(strSrc[dwBegin]))
        {
            return false;
        }
    }

    return true;
}

// ************************* TimeTick ***********************************

TimeTick::TimeTick(time_t tm, int us)
{
    m_sec = tm;
    m_usec = us;
    Normalize();
}

TimeTick::TimeTick(const TimeTick& tt)
{
    m_sec = tt.m_sec;
    m_usec = tt.m_usec;
    Normalize();
}

TimeTick::TimeTick()
{
    m_sec = 0;
    m_usec = 0;
}

void TimeTick::Normalize()
{
    if (!IsNormalized())
    {
        if (m_sec >= 0)
        {
            if (m_usec >= 0)
            {
                m_sec += m_usec / MICROSECONDS_PER_SECOND;
                m_usec = m_usec % MICROSECONDS_PER_SECOND;
            }
            else
            {
                m_sec -= (-m_usec) / MICROSECONDS_PER_SECOND;
                m_usec = -((-m_usec) % MICROSECONDS_PER_SECOND);

                if (m_sec > 0 && m_usec < 0)
                {
                    --m_sec;
                    m_usec = MICROSECONDS_PER_SECOND + m_usec;
                }
            }
        }
        else
        {
            if (m_usec >= 0)
            {
                m_sec += m_usec / MICROSECONDS_PER_SECOND;
                m_usec = m_usec % MICROSECONDS_PER_SECOND;

                if (m_sec < 0 && m_usec > 0)
                {
                    ++m_sec;
                    m_usec = m_usec - MICROSECONDS_PER_SECOND;
                }
            }
            else
            {
                m_sec -= (-m_usec) / MICROSECONDS_PER_SECOND;
                m_usec = -((-m_usec) % MICROSECONDS_PER_SECOND);
            }
        }
    }
}

bool TimeTick::IsNormalized() const
{
    return (m_sec >= 0 && m_usec >= 0 && m_usec < MICROSECONDS_PER_SECOND) ||
           (m_sec <= 0 && m_usec <= 0 && m_usec > -MICROSECONDS_PER_SECOND);
}

TimeTick TimeTick::operator+(const TimeTick& tt) const
{
    TimeTick tick;
    tick.m_sec = this->m_sec + tt.m_sec;
    tick.m_usec = this->m_usec + tt.m_usec;
    tick.Normalize();
    return tick;
}

TimeTick TimeTick::operator-(const TimeTick& tt) const
{
    TimeTick tick;
    tick.m_sec = this->m_sec - tt.m_sec;
    tick.m_usec = this->m_usec - tt.m_usec;
    tick.Normalize();
    return tick;
}

TimeTick& TimeTick::operator+=(const TimeTick& tt)
{
    this->m_sec += tt.m_sec;
    this->m_usec += tt.m_usec;
    this->Normalize();
    return *this;
}

TimeTick& TimeTick::operator-=(const TimeTick& tt)
{
    this->m_sec -= tt.m_sec;
    this->m_usec -= tt.m_usec;
    this->Normalize();
    return *this;
}

TimeTick TimeTick::operator-() const
{
    TimeTick tick;
    tick.m_sec = -this->m_sec;
    tick.m_usec = -this->m_usec;
    return tick;
}

bool TimeTick::operator>(const TimeTick& tt) const
{
    assert(this->IsNormalized());
    assert(tt.IsNormalized());
    return this->m_sec > tt.m_sec || (this->m_sec == tt.m_sec && this->m_usec > tt.m_usec);
}

bool TimeTick::operator<(const TimeTick& tt) const
{
    assert(this->IsNormalized());
    assert(tt.IsNormalized());
    return this->m_sec < tt.m_sec || (this->m_sec == tt.m_sec && this->m_usec < tt.m_usec);
}

bool TimeTick::operator>=(const TimeTick& tt) const
{
    assert(this->IsNormalized());
    assert(tt.IsNormalized());
    return this->m_sec >= tt.m_sec || (this->m_sec == tt.m_sec && this->m_usec >= tt.m_usec);
}

bool TimeTick::operator<=(const TimeTick& tt) const
{
    assert(this->IsNormalized());
    assert(tt.IsNormalized());
    return this->m_sec <= tt.m_sec || (this->m_sec == tt.m_sec && this->m_usec <= tt.m_usec);
}

bool TimeTick::operator==(const TimeTick& tt) const
{
    assert(this->IsNormalized());
    assert(tt.IsNormalized());
    return this->m_sec == tt.m_sec && this->m_usec == tt.m_usec;
}

bool TimeTick::operator!=(const TimeTick& tt) const
{
    assert(this->IsNormalized());
    assert(tt.IsNormalized());
    return this->m_sec != tt.m_sec && this->m_usec != tt.m_usec;
}


// ************************* TimeSpan **********************************************

const TimeSpan TimeSpan::Zero(TimeTick(0, 0));
const TimeSpan TimeSpan::MaxValue(TimeTick((time_t)0x7FFFFFFFFFFFFFFFLL, 999999));
const TimeSpan TimeSpan::MinValue(TimeTick((time_t)0x8000000000000000LL, -999999));

// TimeSpan abs(const TimeSpan& ts)
// {
//    return ts > TimeSpan::Zero ? ts : -ts;
// }

TimeSpan::TimeSpan(const TimeTick& tmTick)
{
    m_Time = tmTick;
    m_Time.Normalize();
}

TimeSpan::TimeSpan(int dwHour, int dwMinute, int dwSecond)
{
    assert(dwMinute < 60 && dwMinute > -60);
    assert(dwSecond < 60 && dwMinute > -60);
    m_Time.m_sec = (time_t)dwHour * SECONDS_PER_HOUR + dwMinute * 60 + dwSecond;
    m_Time.m_usec = 0;
    m_Time.Normalize();
}

TimeSpan::TimeSpan(int dwDay, int dwHour, int dwMinute, int dwSecond)
{
    assert(dwHour < 24 && dwHour > -24);
    assert(dwMinute < 60 && dwMinute > -60);
    assert(dwSecond < 60 && dwSecond > -60);
    m_Time.m_sec = (time_t)dwDay * SECONDS_PER_DAY + dwHour * SECONDS_PER_HOUR +
                   dwMinute * 60 + dwSecond;
    m_Time.m_usec = 0;
    m_Time.Normalize();
}

TimeSpan::TimeSpan(int dwDay, int dwHour, int dwMinute, int dwSecond, int dwMilliSecond)
{
    assert(dwHour < 24 && dwHour > -24);
    assert(dwMinute < 60 && dwMinute > -60);
    assert(dwSecond < 60 && dwSecond > -60);
    assert(dwMilliSecond < 1000 && dwMilliSecond > -1000);
    m_Time.m_sec = (time_t)dwDay * SECONDS_PER_DAY + dwHour * SECONDS_PER_HOUR +
                   dwMinute * 60 + dwSecond;
    m_Time.m_usec = dwMilliSecond * 1000;
    m_Time.Normalize();
}

TimeSpan::TimeSpan(
    int dwDay, int dwHour, int dwMinute, int dwSecond,
    int dwMilliSecond, int dwMicroSecond)
{
    assert(dwHour < 24 && dwHour > -24);
    assert(dwMinute < 60 && dwMinute > -60);
    assert(dwSecond < 60 && dwSecond > -60);
    assert(dwMilliSecond < 1000 && dwMilliSecond > -1000);
    assert(dwMicroSecond < 1000 && dwMicroSecond > -1000);
    m_Time.m_sec = (time_t)dwDay * SECONDS_PER_DAY + dwHour * SECONDS_PER_HOUR +
                   dwMinute * 60 + dwSecond;
    m_Time.m_usec = dwMilliSecond * 1000 + dwMicroSecond;
    m_Time.Normalize();
}

TimeSpan::TimeSpan()
{
    m_Time = TimeTick(0, 0);
}

int TimeSpan::GetDays() const
{
    return (int)(m_Time.m_sec / SECONDS_PER_DAY);
}

int TimeSpan::GetHours() const
{
    return (m_Time.m_sec % SECONDS_PER_DAY) / SECONDS_PER_HOUR;
}

int TimeSpan::GetMinutes() const
{
    return int(m_Time.m_sec % SECONDS_PER_HOUR) / 60;
}

int TimeSpan::GetSeconds() const
{
    return (int)(m_Time.m_sec % 60);
}

int TimeSpan::GetMilliSeconds() const
{
    return m_Time.m_usec / 1000;
}

int TimeSpan::GetMicroSeconds() const
{
    return m_Time.m_usec % 1000;
}

double TimeSpan::GetTotalDays() const
{
    return ((double)m_Time.m_sec + (double)m_Time.m_usec / MICROSECONDS_PER_SECOND) /
           SECONDS_PER_DAY;
}

double TimeSpan::GetTotalHours() const
{
    return ((double)m_Time.m_sec + (double)m_Time.m_usec / MICROSECONDS_PER_SECOND) /
           SECONDS_PER_HOUR;
}

double TimeSpan::GetTotalMinutes() const
{
    return ((double)m_Time.m_sec + (double)m_Time.m_usec / MICROSECONDS_PER_SECOND) / 60;
}

double TimeSpan::GetTotalSeconds() const
{
    return (double)m_Time.m_sec + (double)m_Time.m_usec / MICROSECONDS_PER_SECOND;
}

double TimeSpan::GetTotalMilliSeconds() const
{
    return (double)m_Time.m_sec * 1000 + (double)m_Time.m_usec / 1000;
}

double TimeSpan::GetTotalMicroSeconds() const
{
    return (double)m_Time.m_sec * MICROSECONDS_PER_SECOND + (double)m_Time.m_usec;
}

TimeSpan TimeSpan::FromTick(const TimeTick& tt)
{
    return TimeSpan(tt);
}

TimeSpan TimeSpan::FromDays(double dwDay)
{
    TimeTick tk;
    double fSecond = dwDay * SECONDS_PER_DAY;
    tk.m_sec = (time_t)(fSecond);
    tk.m_usec = (int)((fSecond - (double)tk.m_sec) * MICROSECONDS_PER_SECOND);
    return TimeSpan(tk);
}

TimeSpan TimeSpan::FromHours(double dwHour)
{
    TimeTick tk;
    double fSecond = dwHour * SECONDS_PER_HOUR;
    tk.m_sec = (time_t)(fSecond);
    tk.m_usec = (int)((fSecond - (double)tk.m_sec) * MICROSECONDS_PER_SECOND);
    return TimeSpan(tk);
}

TimeSpan TimeSpan::FromMinutes(double dwMinute)
{
    TimeTick tk;
    double fSecond = dwMinute * 60;
    tk.m_sec = (time_t)(fSecond);
    tk.m_usec = (int)((fSecond - (double)tk.m_sec) * MICROSECONDS_PER_SECOND);
    return TimeSpan(tk);
}

TimeSpan TimeSpan::FromSeconds(double dwSecond)
{
    TimeTick tk;
    double fSecond = dwSecond;
    tk.m_sec = (time_t)(fSecond);
    tk.m_usec = (int)((fSecond - (double)tk.m_sec) * MICROSECONDS_PER_SECOND);
    return TimeSpan(tk);
}

TimeSpan TimeSpan::FromMilliSeconds(double dwMilliSecond)
{
    TimeTick tk;
    double fSecond = dwMilliSecond / 1000;
    tk.m_sec = (time_t)(fSecond);
    tk.m_usec = (int)((fSecond - (double)tk.m_sec) * MICROSECONDS_PER_SECOND);
    return TimeSpan(tk);
}

TimeSpan TimeSpan::FromMicroSeconds(double dwMicroSecond)
{
    TimeTick tk;
    double fSecond = dwMicroSecond / MICROSECONDS_PER_SECOND;
    tk.m_sec = (time_t)(fSecond);
    tk.m_usec = (int)((fSecond - (double)tk.m_sec) * MICROSECONDS_PER_SECOND);
    return TimeSpan(tk);
}

TimeSpan TimeSpan::operator-() const
{
    return TimeSpan(-this->m_Time);
}

TimeSpan TimeSpan::operator+(const TimeSpan& ts) const
{
    return TimeSpan(this->m_Time + ts.m_Time);
}

TimeSpan TimeSpan::operator-(const TimeSpan& ts) const
{
    return TimeSpan(this->m_Time - ts.m_Time);
}

bool TimeSpan::operator>(const TimeSpan& ts) const
{
    return this->m_Time > ts.m_Time;
}

bool TimeSpan::operator<(const TimeSpan& ts) const
{
    return this->m_Time < ts.m_Time;
}

bool TimeSpan::operator>=(const TimeSpan& ts) const
{
    return this->m_Time >= ts.m_Time;
}

bool TimeSpan::operator<=(const TimeSpan& ts) const
{
    return this->m_Time <= ts.m_Time;
}

bool TimeSpan::operator==(const TimeSpan& ts) const
{
    return this->m_Time == ts.m_Time;
}

bool TimeSpan::operator!=(const TimeSpan& ts) const
{
    return this->m_Time != ts.m_Time;
}

/*  ��ʾ��ʵ����ֵ���ַ���������ֵ��ʽ���£�
[-][d.]hh:mm:ss[.ff]
�����ţ���[���͡�]�����е����ǿ�ѡ�ģ�ð�ź;�ţ���:���͡�.������ԭ���ַ������������¡�
��      ˵��
��-��   ��ѡ�ĸ��ţ�ָʾ��ʱ��
��d��   ��ѡ������
��hh��  Сʱ����Χ�� 0 �� 23
��mm��  ���ӣ���Χ�� 0 �� 59
��ss��  �룬��Χ�� 0 �� 59
��ff��  ��ѡ�����С�����֣��� 1 �� 7 ��С��λ      */
string TimeSpan::ToString() const
{
    string strRst;
    char buf[200];
    int dwDay = GetDays();

    if (dwDay != 0)
    {
        sprintf(buf, "%d.", dwDay);
        strRst.append(buf);
    }

    sprintf(buf, "%02d:%02d:%02d", abs(GetHours()), abs(GetMinutes()), abs(GetSeconds()));
    strRst.append(buf);

    if (m_Time.m_usec != 0)
    {
        sprintf(buf, ".%06d", abs(m_Time.m_usec));
        strRst.append(buf);
    }

    return string(strRst);
}

/* ���ַ�����ָ����ʱ���������µ�TimeSpan����
strTime ��������һ��������ʽ��ʱ�����淶��
[ws][-]{ d | [d.]hh:mm[:ss[.ff]] }[ws]
�����ţ���[���͡�]�����е����ǿ�ѡ�ģ���Ҫ�Ӵ����ţ���{���͡�}������������ (|) �ָ����滻ѡ���б���ѡ��һ�ð�ź;�ţ���:���͡�.�����Ǳ����ԭ���ַ������������¡�
��      ˵��
ws      ��ѡ�Ŀհ�
��-��   ��ѡ�ļ��ţ�ָʾ�� TimeSpan
d       �죬��Χ�� 0 �� 10675199
hh      Сʱ����Χ�� 0 �� 23
mm      ���ӣ���Χ�� 0 �� 59
ss      ��ѡ���룬��Χ�� 0 �� 59
ff      ��ѡ�����С�����֣��� 1 �� 7 ��С��λ

strTime �ķ�����������ָ�����ڻ���� MinValue ��С�ڻ���� MaxValue ��ʱ������        */
TimeSpan TimeSpan::Parse(const string& strTime)
{
    size_t dwIndex = 0;

    while (IS_BLANK_SPACE_CHAR(strTime[dwIndex]) && dwIndex < strTime.length())
    {
        dwIndex++;
    }

    // �հ��ַ���
    if (dwIndex == strTime.length())
    {
        assert(false);
        return TimeSpan::Zero;
    }

    // ���ҵ�һ��:�͵�һ��.
    size_t dwPosSepM = strTime.find(':', dwIndex);
    stringstream strStream(strTime);

    // ֻ������
    if (dwPosSepM == string::npos)
    {
        double fDay;

        if (strStream >> fDay)
        {
            return TimeSpan::FromDays(fDay);
        }

        assert(false);
        return TimeSpan::Zero;
    }

    size_t dwPosSepD = strTime.find('.', dwIndex);
    int dwDay = 0;
    int dwHour = 0;
    int dwMin = 0;
    int dwSec = 0;
    int dwUsec = 0;
    char ch = 0;

    // hh:mm[:ss]��ʽ
    if (dwPosSepD == string::npos)
    {
        strStream >> dwHour >> ch;

        if (ch != ':')
        {
            assert(false);
            return TimeSpan::Zero;
        }

        strStream >> dwMin;

        if (!strStream.eof())
        {
            strStream >> ch;

            if (ch == ':')
            {
                strStream >> dwSec;
            }
        }

        assert(dwHour >= 0 && dwHour < 24);
        assert(dwMin >= 0 && dwMin < 60);
        assert(dwSec >= 0 && dwSec < 60);
        return TimeSpan(dwHour, dwMin, dwSec);
    }
    // [-]d.hh:mm[:ss[.ff]]��ʽ
    else if (dwPosSepD < dwPosSepM)
    {
        if (!(strStream >> dwDay >> ch >> dwHour >> ch >> dwMin))
        {
            assert(false);
            return TimeSpan::Zero;
        }

        if (!strStream.eof())
        {
            strStream >> ch;

            if (ch == ':')
            {
                strStream >> dwSec;

                if (!strStream.eof())
                {
                    strStream >> ch;

                    if (ch == '.')
                    {
                        int dwMode = 100000;

                        while ((strStream >> ch) && dwMode > 0)
                        {
                            if (ch >= '0' && ch <= '9')
                            {
                                dwUsec += (ch - '0') * dwMode;
                            }
                            else
                            {
                                break;
                            }

                            dwMode /= 10;
                        }
                    }
                }
            }
        }

        if (dwDay > 0)
        {
            assert(dwHour >= 0 && dwHour < 24);
            assert(dwMin >= 0 && dwMin < 60);
            assert(dwSec >= 0 && dwSec < 60);
            return TimeSpan(dwDay, dwHour, dwMin, dwSec, dwUsec / 1000, dwUsec % 1000);
        }
        else
        {
            return TimeSpan(dwDay, -dwHour, -dwMin, -dwSec, -dwUsec / 1000, -dwUsec % 1000);
        }
    }
    // hh:mm:ss.ff��ʽ
    else if (dwPosSepD > dwPosSepM)
    {
        if (!(strStream >> dwHour >> ch >> dwMin >> ch >> dwSec >> ch))
        {
            assert(false);
            return TimeSpan::Zero;
        }

        int dwMode = 100000;

        while ((strStream >> ch) && dwMode > 0)
        {
            if (ch >= '0' && ch <= '9')
            {
                dwUsec += (ch - '0') * dwMode;
            }
            else
            {
                break;
            }

            dwMode /= 10;
        }

        return TimeSpan(dwDay, dwHour, dwMin, dwSec, dwUsec / 1000, dwUsec % 1000);
    }

    assert(false);
    return TimeSpan::Zero;
}

TimeSpan& TimeSpan::operator+=(const TimeSpan& ts)
{
    this->m_Time += ts.m_Time;
    return *this;
}

TimeSpan& TimeSpan::operator-=(const TimeSpan& ts)
{
    this->m_Time -= ts.m_Time;
    return *this;
}

// ********************* DateTime **********************************

DateTime::DateTime(const TimeTick& tmTick, int dwTimezoneSeconds)
{
    m_dwTimezoneSeconds = (dwTimezoneSeconds < 0) ?
        DateTime::GetCurrentTimezoneSeconds() : dwTimezoneSeconds;
    m_Time = tmTick;
    // ����ʱ��
    m_Time.m_sec += m_dwTimezoneSeconds;
    m_Time.Normalize();
}

DateTime::DateTime(int dwYear, int dwMonth, int dwDay,
                   int dwHour /*= 0*/, int dwMinute /*= 0*/, int dwSecond /*= 0*/,
                   int dwMilliSecond /*= 0*/, int dwMicroSecond /*= 0*/ ,
                   int dwTimezoneSeconds)
{
    assert(IsValidDate(dwYear, dwMonth, dwDay));
    m_dwTimezoneSeconds = (dwTimezoneSeconds < 0) ?
        DateTime::GetCurrentTimezoneSeconds() : dwTimezoneSeconds;
    struct tm oTime;
    oTime.tm_hour = dwHour;
    oTime.tm_min = dwMinute;
    oTime.tm_sec = dwSecond;
    oTime.tm_year = dwYear - 1900;
    oTime.tm_mon = dwMonth - 1;
    oTime.tm_mday = dwDay;
    time_t t = mktime(&oTime);
    // mktime�õ���ʱ�����ǰϵͳʱ���йأ�����ϵͳ��ǰʱ��ת��Ϊʱ���޹�ʱ��
    m_Time.m_sec = t + DateTime::Now().m_dwTimezoneSeconds;
    m_Time.m_usec = dwMilliSecond * 1000 + dwMicroSecond;
    m_Time.Normalize();
}

DateTime::DateTime(int dwTimezoneSeconds)
{
    m_dwTimezoneSeconds = (dwTimezoneSeconds < 0) ?
        DateTime::GetCurrentTimezoneSeconds() : dwTimezoneSeconds;
    // DateTime::Now��ֵ��gettimeofday�õ����ú�������ֵ��ϵͳ��ǰʱ���޹أ�
    // �����ڼ���Nowʱ�Ѿ�����ʱ��ʱ�䣬������ﲻ�ü�
    m_Time = DateTime::Now().m_Time;
}

DateTime::DateTime(const time_t& tm, int usec /*= 0*/, int dwTimezoneSeconds)
{
    m_dwTimezoneSeconds = (dwTimezoneSeconds < 0) ?
        DateTime::GetCurrentTimezoneSeconds() : dwTimezoneSeconds;
    // �����time_tָ����ʱ���޹غ����ĵ���ֵ����time������������mktime���ֺ�ϵͳʱ����غ���
    // �ķ���ֵ�����������Ҫת����ʱ��ʱ��
    m_Time.m_sec = tm + m_dwTimezoneSeconds;
    m_Time.m_usec = usec;
    m_Time.Normalize();
}

time_t DateTime::GetSecondsSinceEpoch() const
{
    return this->m_Time.m_sec - m_dwTimezoneSeconds;
}

TimeSpan DateTime::operator-(const DateTime& dt) const
{
    // ��ͬʱ����û������
    assert(this->GetTimeZoneSeconds() == dt.GetTimeZoneSeconds());
    return TimeSpan(this->m_Time - dt.m_Time);
}

DateTime DateTime::operator-(const TimeSpan& ts) const
{
    TimeTick temp_tick = this->m_Time - ts.m_Time;
    temp_tick.m_sec -= this->GetTimeZoneSeconds();
    return DateTime(temp_tick);
}

DateTime DateTime::operator+(const TimeSpan& ts) const
{
    TimeTick temp_tick = this->m_Time + ts.m_Time;
    temp_tick.m_sec -= this->GetTimeZoneSeconds();
    return DateTime(temp_tick);
}

DateTime& DateTime::operator-=(const TimeSpan& ts)
{
    this->m_Time -= ts.m_Time;
    return *this;
}

DateTime& DateTime::operator+=(const TimeSpan& ts)
{
    this->m_Time += ts.m_Time;
    return *this;
}

bool DateTime::operator>(const DateTime& dt) const
{
    return this->m_Time > dt.m_Time;
}

bool DateTime::operator<(const DateTime& dt) const
{
    assert(this->GetTimeZoneSeconds() == dt.GetTimeZoneSeconds());
    return this->m_Time < dt.m_Time;
}

bool DateTime::operator>=(const DateTime& dt) const
{
    assert(this->GetTimeZoneSeconds() == dt.GetTimeZoneSeconds());
    return this->m_Time >= dt.m_Time;
}

bool DateTime::operator<=(const DateTime& dt) const
{
    assert(this->GetTimeZoneSeconds() == dt.GetTimeZoneSeconds());
    return this->m_Time <= dt.m_Time;
}

bool DateTime::operator==(const DateTime& dt) const
{
    assert(this->GetTimeZoneSeconds() == dt.GetTimeZoneSeconds());
    return this->m_Time == dt.m_Time;
}

bool DateTime::operator!=(const DateTime& dt) const
{
    assert(this->GetTimeZoneSeconds() == dt.GetTimeZoneSeconds());
    return this->m_Time != dt.m_Time;
}

#ifdef _WIN32
static inline void gmtime_r(const time_t* sec, struct tm* tm_now)
{
    gmtime_s(tm_now, sec);
}
#endif

int DateTime::GetYear() const
{
    struct tm   tm_now;
    gmtime_r(&this->m_Time.m_sec, &tm_now);
    return tm_now.tm_year + 1900;
}

int DateTime::GetMonth() const
{
    struct tm   tm_now;
    gmtime_r(&this->m_Time.m_sec, &tm_now);
    return tm_now.tm_mon + 1;
}

int DateTime::GetDayOfMonth() const
{
    struct tm   tm_now;
    gmtime_r(&this->m_Time.m_sec, &tm_now);
    return tm_now.tm_mday;
}

int DateTime::GetDayOfYear() const
{
    struct tm   tm_now;
    gmtime_r(&this->m_Time.m_sec, &tm_now);
    return tm_now.tm_yday;
}

int DateTime::GetDayOfWeek() const
{
    struct tm   tm_now;
    gmtime_r(&this->m_Time.m_sec, &tm_now);
    return tm_now.tm_wday;
}

int DateTime::GetHour() const
{
    struct tm   tm_now;
    gmtime_r(&this->m_Time.m_sec, &tm_now);
    return tm_now.tm_hour;
}

int DateTime::GetHour12() const
{
    int dwHour = GetHour() % 12;

    if (dwHour == 0)
    {
        dwHour = 12;
    }

    return dwHour;
}

int DateTime::GetMinute() const
{
    struct tm   tm_now;
    gmtime_r(&this->m_Time.m_sec, &tm_now);
    return tm_now.tm_min;
}

int DateTime::GetSecond() const
{
    struct tm   tm_now;
    gmtime_r(&this->m_Time.m_sec, &tm_now);
    return tm_now.tm_sec;
}

int DateTime::GetMilliSecond() const
{
    return m_Time.m_usec / 1000;
}

int DateTime::GetMicroSecond() const
{
    return m_Time.m_usec % 1000;
}

// ��������
DateTime DateTime::GetDay()const
{
    return DateTime(GetYear(), GetMonth(), GetDayOfMonth());
}
// ����ʱ��
TimeSpan DateTime::GetTime()const
{
    return TimeSpan(0, GetHour(), GetMinute(), GetSecond(), GetMilliSecond(), GetMicroSecond());
}

DateTime DateTime::Now()
{
    int dwTimezoneSeconds;
#ifdef _WIN32
    struct _timeb   tbNow;
    _ftime_s(&tbNow);
    dwTimezoneSeconds = -tbNow.timezone * 60;
    TimeTick tt(tbNow.time, tbNow.millitm * 1000);
#else
    struct timeval  tvNow;
    gettimeofday(&tvNow, NULL);
    tzset();
    dwTimezoneSeconds = -timezone;
    TimeTick tt(tvNow.tv_sec, (int) tvNow.tv_usec);
#endif
    return DateTime(tt, dwTimezoneSeconds);
}

DateTime DateTime::UTCNow()
{
    DateTime dt = DateTime::Now();
    // ��ȥʱ��ʱ�䣬�õ�0ʱ����ʱ��
    dt.m_Time.m_sec -= dt.m_dwTimezoneSeconds;
    dt.m_dwTimezoneSeconds = 0;
    return dt;
}

// ���ص�ǰ����
DateTime DateTime::Today()
{
    time_t t;
    int tz_offset;
#ifdef _WIN32
    struct _timeb   tbNow;
    _ftime_s(&tbNow);
    tz_offset = -tbNow.timezone * 60;
    t = tbNow.time;
#else
    t = time(NULL);
    tzset();
    tz_offset = -timezone;
#endif
    // Today���ص�ǰ����
    // ��ȥʱ��ʱ��ȡ��������󣬵õ�����0��ʱ��
    t = (t + tz_offset) / SECONDS_PER_DAY * SECONDS_PER_DAY - tz_offset;
    TimeTick tt(t, 0);
    return DateTime(tt, tz_offset);
}


// ���ص�ǰʱ��
TimeSpan DateTime::Time()
{
    time_t sec;
    int usec;
    int tz_offset;
#ifdef _WIN32
    struct _timeb   tbNow;
    _ftime_s(&tbNow);
    sec = tbNow.time;
    usec = tbNow.millitm * 1000;
    tz_offset = -tbNow.timezone * 60;
#else
    struct timeval  tvNow;
    gettimeofday(&tvNow, NULL);
    sec = tvNow.tv_sec;
    usec = tvNow.tv_usec;
    tzset();
    tz_offset = -timezone;
#endif
    TimeTick tt((sec + tz_offset) % SECONDS_PER_DAY , usec);
    return TimeSpan(tt);
}

DateTime DateTime::Parse(
    const std::string& strTime,
    const DateTimeFormatInfo& formatInfo /*= DateTimeFormatInfo::zh_CNInfo*/)
{
    DateTime dt;
    dt.SetTime(TimeSpan(12, 0, 0));

    if (!TryParse(strTime, dt, formatInfo))
    {
        return DateTime::Now();
    }

    return dt;
}

DateTime DateTime::Parse(
    const std::string& strTime,
    const std::string& strFormat,
    const DateTimeFormatInfo& formatInfo /*= DateTimeFormatInfo::zh_CNInfo*/)
{
    DateTime dt;
    dt.SetTime(TimeSpan(12, 0, 0));

    if (!TryParse(strTime, strFormat, dt, formatInfo))
    {
        return DateTime::Now();
    }

    return dt;
}

static bool HasSegmentStartsWith(const std::vector<FormatSegment>& segments, char p)
{
    for(std::vector<FormatSegment>::const_iterator it = segments.begin();
        it != segments.end();
        ++it) {
        if ((*it).strSeg[0] == p) {
            return true;
        }
    }
    return false;
}

bool DateTime::TryParse(
    const std::string& strTime,
    const std::string& strFormat,
    DateTime& dt,
    const DateTimeFormatInfo& formatInfo /*= DateTimeFormatInfo::zh_CNInfo*/)
{
    vector<FormatSegment> vecSegment;

    if (!SegmentFormatString(strFormat, vecSegment))
        return false;

    //
    // The following ugly code is for a bug.
    // When user try to parse date with string "2010-11-3 12:23:34" at 2012-1-31,
    // the default value in 'dt' is "2012-1-31 12:00:00".
    // In TryParse,
    //      1. YEAR is set to 2010
    //      2. MONTH is set to 11. But now, the DAY of MONTH is 31. there isn't 31st
    //      days in November.
    // We happen to store the time stamp as the time span from 1970. so now the time
    // is "2010-12-1 12:00:00".
    // After the DAY of month is set, the value is "2010-12-3 12:00:00".
    // Finally, we get "2010-12-3 12:23:34".
    //
    DateTime dt2 = dt;
    // if the YEAR will be reset, it's safe to set to a fake one.
    if (HasSegmentStartsWith(vecSegment, 'y')) {
        const int dammy_year = 2008;// don't change it. it's a fake leap year. A leap
                                    // year is safe to handle 29th Feb staff.
        dt2.SetYear(dammy_year);
    }
    if (HasSegmentStartsWith(vecSegment, 'M')) {
        dt2.SetMonth(1);            // be careful when touch the magic number here.
                                    // use Jan as default, it safe to handle 31th case.
        dt2.SetDayOfMonth(1);
    }

    DateTimeInfo date_info;
    size_t dwIndex = 0;

    for (size_t dwSegIndex = 0; dwSegIndex < vecSegment.size(); dwSegIndex++)
    {
        if (!TryParseSegment(strTime, dwIndex, vecSegment, dwSegIndex, &date_info, formatInfo))
            return false;
    }
    MakeDateTime(date_info, &dt2);
    dt = dt2;

    return true;
}

bool DateTime::TryParse(
    const std::string& strTime,
    DateTime& dt,
    const DateTimeFormatInfo& formatInfo /*= DateTimeFormatInfo::zh_CNInfo*/)
{
    const vector<string> &vecAllPattern = formatInfo.GetAllPatterns();
    DateTime tm = dt;

    for (vector<string>::const_iterator itr = vecAllPattern.begin();
         itr != vecAllPattern.end();
         ++itr)
    {
        dt = tm;

        if (TryParse(strTime, *itr, dt, formatInfo))
        {
            return true;
        }
    }

    return false;
}

string DateTime::ToString(
    const DateTimeFormatInfo& formatInfo /*= DateTimeFormatInfo::zh_CNInfo*/) const
{
    DateTime dt = *this;
    string strDstFormat = ExpandPredefinedFormat("F", formatInfo, dt);
    strDstFormat += ".FFFFFF zzz";

    // �����Զ����ʽ
    return ToFormatString(dt, strDstFormat, formatInfo);
}
/*
1.��׼��ʽģʽ
��ʽģʽ    ��������/˵��
d           ShortDatePattern
D           LongDatePattern
f           �������ں�ʱ�䣨�����ںͶ�ʱ�䣩
F           FullDateTimePattern�������ںͳ�ʱ�䣩
g           ���棨�����ںͶ�ʱ�䣩
G           ���棨�����ںͳ�ʱ�䣩
m��M        MonthDayPattern
o��O        ��������/ʱ��ģʽ�������ָ�ʽģʽ�£���ʽ���û��������ʼ��ʹ�ù̶�������
r��R        RFC1123Pattern�������ָ�ʽģʽ�£���ʽ���û��������ʼ��ʹ�ù̶�������
s           ʹ�ñ���ʱ��� SortableDateTimePattern������ ISO 8601���������ָ�ʽģʽ�£�
            ��ʽ���û��������ʼ��ʹ�ù̶�������
t           ShortTimePattern
T           LongTimePattern
u           ʹ��ͨ��ʱ����ʾ��ʽ�� UniversalSortableDateTimePattern�������ָ�ʽģʽ�£�
            ��ʽ���û��������ʼ��ʹ�ù̶�������
U           ʹ��ͨ��ʱ����������ں�ʱ�䣨�����ںͳ�ʱ�䣩
y��Y        YearMonthPattern        */
std::string DateTime::ToString(
    const std::string& strFormat,
    const DateTimeFormatInfo& formatInfo/* = DateTimeFormatInfo::zh_CNInfo*/)const
{
    DateTime dt = *this;
    string strDstFormat = ExpandPredefinedFormat(strFormat, formatInfo, dt);
    // �����Զ����ʽ
    return ToFormatString(dt, strDstFormat, formatInfo);
}

bool DateTime::IsLeapYear() const
{
    int dwYear = GetYear();
    return IsLeapYear(dwYear);
}

bool DateTime::IsLeapYear(int dwYear)
{
    if (dwYear % 4 == 0)
    {
        if (dwYear % 100 == 0)
        {
            if (dwYear % 400 == 0)
            {
                return true;
            }
            return false;
        }
        return true;
    }
    return false;
}

static int s_day_number_of_month[12] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

bool DateTime::IsValidDate(int dwYear, int dwMonth , int dwDay)
{
    if (dwMonth < 1 || dwMonth > 12)
    {
        return false;
    }

    if (dwDay < 1 || dwDay > s_day_number_of_month[dwMonth-1])
    {
        return false;
    }

    if (dwMonth == 2 && dwDay == 29 && !IsLeapYear(dwYear))
    {
        return false;
    }
    return true;
}

bool DateTime::SegmentFormatString(
    const std::string& strFormat,
    std::vector<FormatSegment> &vecSegment)
{
    for (size_t dwIndex = 0; dwIndex < strFormat.length(); dwIndex++)
    {
        char ch = strFormat[dwIndex];

        if (ch == '\'')   // �ַ�����ʼ
        {
            size_t dwPos = strFormat.find('\'', dwIndex + 1);

            if (dwPos == string::npos)
            {
                return false;
            }

            FormatSegment seg;
            seg.strSeg = strFormat.substr(dwIndex + 1, dwPos - dwIndex - 1);
            seg.dwKind = FSTK_String;
            vecSegment.push_back(seg);
            dwIndex = dwPos;
        }
        else if (ch == '"')     // �ַ�����ʼ
        {
            size_t dwPos = strFormat.find('"', dwIndex + 1);

            if (dwPos == string::npos)
            {
                return false;
            }

            FormatSegment seg;
            seg.strSeg = strFormat.substr(dwIndex + 1, dwPos - dwIndex - 1);
            seg.dwKind = FSTK_String;
            vecSegment.push_back(seg);
            dwIndex = dwPos;
        }
        else if (ch == ':')     // ʱ��ָ���
        {
            FormatSegment seg;
            seg.strSeg = ":";
            seg.dwKind = FSTK_TIMESEP_CHAR;
            vecSegment.push_back(seg);
        }
        else if (ch == '/')     // ���ڷָ���
        {
            FormatSegment seg;
            seg.strSeg = "/";
            seg.dwKind = FSTK_DATESEP_CHAR;
            vecSegment.push_back(seg);
        }
        else if (ch == '%')     // ģʽת���
        {
            if (dwIndex == strFormat.length() - 1)
            {
                assert(false);
                return false;
            }

            FormatSegment seg;
            seg.strSeg = strFormat.substr(dwIndex + 1, 1);

            if (!ParseFormatSegment(&seg))
            {
                assert(false);
                return false;
            }

            vecSegment.push_back(seg);
            dwIndex++;
        }
        else if (ch == '\\')     // �ַ�ת���
        {
            if (dwIndex == strFormat.length() - 1)
            {
                assert(false);
                return false;
            }

            FormatSegment seg;
            seg.strSeg = strFormat.substr(dwIndex + 1, 1);
            seg.dwKind = FSTK_String;
            vecSegment.push_back(seg);
            dwIndex++;
        }
        else if (IS_PATTERN_CHAR(ch))
        {
            // ģʽ�ַ���,ʱ��ģʽ�ַ��������ظ����ֵ��ַ��磺d,dd,ddd,dddddd��
            size_t dwPos = dwIndex;

            while (dwPos < strFormat.length() && strFormat[dwPos] == ch)
                dwPos++;

            FormatSegment seg;
            seg.strSeg = strFormat.substr(dwIndex, dwPos - dwIndex);

            if (!ParseFormatSegment(&seg))
            {
                assert(false);
                return false;
            }

            vecSegment.push_back(seg);
            dwIndex = dwPos - 1;
        }
        else     // ��ģʽ�ַ���
        {
            size_t dwPos = dwIndex;

            while (!IS_PATTERN_CHAR(strFormat[dwPos]) &&
                   !IS_SEPARATE_CHAR(strFormat[dwPos]) &&
                   dwPos < strFormat.length())
            {
                dwPos++;
            }

            FormatSegment seg;
            seg.strSeg = strFormat.substr(dwIndex, dwPos - dwIndex);
            seg.dwKind = FSTK_String;
            vecSegment.push_back(seg);
            dwIndex = dwPos - 1;
        }
    }

    return true;
}

/*2.�Զ����ʽģʽ��������ģʽ�ַ�Ӧ���õ�����������
��ʽģʽ    ˵��
d��%d       ���е�ĳһ�졣һλ��������û��ǰ���㡣����˸�ʽģʽû����������ʽģʽ��ϣ�Ӧ�ó���ָ����%d����
dd      ���е�ĳһ�졣һλ��������һ��ǰ���㡣
ddd     ����ĳ�����д���ƣ��� AbbreviatedDayNames �ж��塣
dddd    ����ĳ����������ƣ��� DayNames �ж��塣
f��%f   ���С������Ϊһλ���������ֱ��ضϡ�����˸�ʽģʽû����������ʽģʽ��ϣ�Ӧ�ó���ָ����%f����
ff      ���С������Ϊ��λ���������ֱ��ضϡ�
fff     ���С������Ϊ��λ���������ֱ��ضϡ�
ffff    ���С������Ϊ��λ���������ֱ��ضϡ�
fffff   ���С������Ϊ��λ���������ֱ��ضϡ�
ffffff  ���С������Ϊ��λ���������ֱ��ضϡ�
fffffff ���С������Ϊ��λ���������ֱ��ضϡ�
F��%F   ��ʾ���С�����ֵ������Ч���֡����������Ϊ�㣬����ʾ�κ����ݡ�����˸�ʽģʽû����������ʽģʽ��ϣ�Ӧ�ó���ָ����%F����
FF      ��ʾ���С�����ֵ����������Ч���֡����ǣ�����ʾβ����㣨���������֣���
FFF     ��ʾ���С�����ֵ����������Ч���֡����ǣ�����ʾβ����㣨���������֣���
FFFF    ��ʾ���С�����ֵ��ĸ������Ч���֡����ǣ�����ʾβ����㣨�ĸ������֣���
FFFFF   ��ʾ���С�����ֵ���������Ч���֡����ǣ�����ʾβ����㣨��������֣���
FFFFFF  ��ʾ���С�����ֵ����������Ч���֡����ǣ�����ʾβ����㣨���������֣���
FFFFFFF ��ʾ���С�����ֵ��߸������Ч���֡����ǣ�����ʾβ����㣨�߸������֣���
gg      ʱ�ڻ��Ԫ�����Ҫ���ø�ʽ�����ڲ����й�����ʱ�ڻ��Ԫ�ַ���������Ը�ģʽ��
h��%h   12 Сʱ�Ƶ�Сʱ��һλ����Сʱ��û��ǰ���㡣����˸�ʽģʽû����������ʽģʽ��ϣ�Ӧ�ó���ָ����%h����
hh      12 Сʱ�Ƶ�Сʱ��һλ����Сʱ��һ��ǰ���㡣
H��%H   24 Сʱ�Ƶ�Сʱ��һλ����Сʱ��û��ǰ���㡣����˸�ʽģʽû����������ʽģʽ��ϣ�Ӧ�ó���ָ����%H����
HH      24 Сʱ�Ƶ�Сʱ��һλ����Сʱ��һ��ǰ���㡣
K       Kind ���ԵĲ�ֵͬ�������ء�Utc ��δָ����ͬzzz��
m��%m   ���ӡ�һλ���ķ�����û��ǰ���㡣����˸�ʽģʽû����������ʽģʽ��ϣ�Ӧ�ó���ָ����%m����
mm      ���ӡ�һλ���ķ�����һ��ǰ���㡣
M��%M   �·����֡�һλ�����·�û��ǰ���㡣����˸�ʽģʽû����������ʽģʽ��ϣ�Ӧ�ó���ָ����%M����
MM      �·����֡�һλ�����·���һ��ǰ���㡣
MMM     �·ݵ���д���ƣ��� AbbreviatedMonthNames �ж��塣
MMMM    �·ݵ��������ƣ��� MonthNames �ж��塣
s��%s   �롣һλ��������û��ǰ���㡣����˸�ʽģʽû����������ʽģʽ��ϣ�Ӧ�ó���ָ����%s����
ss      �롣һλ��������һ��ǰ���㡣
t��%t   �� AMDesignator �� PMDesignator �ж���� AM/PM ָʾ��ĵ�һ���ַ���������ڣ�������˸�ʽģʽû����������ʽģʽ��ϣ�Ӧ�ó���ָ����%t����
tt      �� AMDesignator �� PMDesignator �ж���� AM/PM ָʾ�������ڣ���������Ҫά�� AM �� PM ֮��Ĳ�������ԣ�Ӧ�ó���Ӧʹ�ô˸�ʽģʽ��������Ϊ������ AM �� PM ָʾ���Ĳ����Ϊ�ڶ����ַ������ǵ�һ���ַ���
y��%y   ��������Ԫ����ݡ������������Ԫ�����С�� 10������ʾ������ǰ�������ݡ�����˸�ʽģʽû����������ʽģʽ��ϣ�Ӧ�ó���ָ����%y����
yy      ��������Ԫ����ݡ������������Ԫ�����С�� 10������ʾ����ǰ�������ݡ�
yyy     ��λ������ݡ�������С�� 100������Դ�ǰ����ķ�ʽ��ʾ��
yyyy    ������Ԫ����λ����λ������ݣ�ȡ������ʹ�õ������������ڲ�����λ������ݣ���ʹ��ǰ������䡣̩�������ͳ�����������λ������ݡ����ھ�����λ����������ѡ��yyyy��ģʽ���û�����������ǰ�������������λ������������������ձ�����̨������������Ϊʼ�ն������û�ѡ���ˡ�yy����
yyyyy   ��λ������ݡ����ڲ�����λ������ݣ���ʹ��ǰ������䡣��������������ձ�����̨������������Ϊʼ�ն������û�ѡ���ˡ�yy����
yyyyyy  ��λ������ݡ����ڲ�����λ������ݣ���ʹ��ǰ������䡣��������������ձ�����̨������������Ϊʼ�ն������û�ѡ���ˡ�yy������ģʽ�ɲ������ӡ�y�����Ӷ��γ�һ���������ַ�������ʱ����Ҫʹ�ø����ǰ���㡣
z��%z   ʱ��ƫ��������+����-���������Сʱ����һλ����Сʱ��û��ǰ���㡣���磬̫ƽ���׼ʱ���ǡ�-8��������˸�ʽģʽû����������ʽģʽ��ϣ�Ӧ�ó���ָ����%z����
zz      ʱ��ƫ��������+����-���������Сʱ����һλ����Сʱ��һ��ǰ���㡣���磬̫ƽ���׼ʱ���ǡ�-08����
zzz     ����ʱ��ƫ��������+����-���������Сʱ�ͷ��ӣ���һλ����Сʱ���ͷ�������ǰ���㡣���磬̫ƽ���׼ʱ���ǡ�-08:00����
:       �� TimeSeparator �ж����Ĭ��ʱ��ָ�����
/       �� DateSeparator �ж����Ĭ�����ڷָ�����
% c     ���� c �Ǹ�ʽģʽ���������ʹ�ã�����Ҫʹ�ø�ʽģʽ��d������f������F������h������m������s������t������y������z������H������M������Ӧ�ó���Ӧָ����%d������%f������%F������%h������%m������%s������%t������%y������%z������%H����%M���������ʽģʽ��ԭ���ַ���������ʽģʽ�ϲ��������ʡ�ԡ�%���ַ���
\ c     ���� c �������ַ�����ԭ����ʾ�ַ�����Ҫ��ʾ��б���ַ���Ӧ�ó���Ӧʹ�á�\\����           */
std::string DateTime::ToFormatString(
    const DateTime& dt,
    const std::string& strFormat,
    const DateTimeFormatInfo& formatInfo)
{
    vector<FormatSegment> vecSegment;

    if (!SegmentFormatString(strFormat, vecSegment))
        return "";

    return ToFormatString(dt, vecSegment, formatInfo);
}

std::string DateTime::ToFormatString(
    const DateTime& dt,
    const std::vector<FormatSegment> &vecSegment,
    const DateTimeFormatInfo& formatInfo)
{
    string strRst;
    char buf[100];

    for (vector<FormatSegment>::const_iterator itr = vecSegment.begin();
         itr != vecSegment.end(); ++itr)
    {
        switch (itr->dwKind)
        {
        case FSTK_String:
            strRst.append(itr->strSeg);
            break;
        case FSTK_d:
            sprintf(buf, "%d", dt.GetDayOfMonth());
            strRst.append(buf);
            break;
            // dd,   01-31
        case FSTK_dd:
            sprintf(buf, "%02d", dt.GetDayOfMonth());
            strRst.append(buf);
            break;
            // ddd,  һ����ĳ�����������
        case FSTK_ddd:
            strRst.append(formatInfo.AbbreviatedDayNames()[dt.GetDayOfWeek()]);
            break;
            // dddd���߸����d   ��һ����ĳ���ȫ��
        case FSTK_dddd_:
            strRst.append(formatInfo.DayNames()[dt.GetDayOfWeek()]);
            break;
            // f,ff,fff,ffff,fffff,ffffff
        case FSTK_f:
        case FSTK_ff:
        case FSTK_fff:
        case FSTK_ffff:
        case FSTK_fffff:
        case FSTK_ffffff:
            {
                int usec = dt.m_Time.m_usec;
                size_t dwLen = itr->strSeg.length() > 6 ? 6 : itr->strSeg.length();
                sprintf(buf, "%06d", usec);
                strRst.append(buf, dwLen);
            }
            break;
            // F,FF,FFF,FFFF,FFFFF,FFFFFF
        case FSTK_F:
        case FSTK_FF:
        case FSTK_FFF:
        case FSTK_FFFF:
        case FSTK_FFFFF:
        case FSTK_FFFFFF:
            {
                int usec = dt.m_Time.m_usec;
                size_t dwLen = itr->strSeg.length() > 6 ? 6 : itr->strSeg.length();
                sprintf(buf, "%06d", usec);

                while (dwLen > 0 && buf[dwLen - 1] == '0')
                {
                    dwLen--;
                }

                if (dwLen > 0)
                {
                    strRst.append(buf, dwLen);
                }
                else if (*strRst.rbegin() == '.')     // С����
                {
                    strRst.erase(strRst.length() - 1);
                }
            }
            break;
            // g,gg���߸���g
        case FSTK_g_:
            strRst.append(formatInfo.EraNames()[0]);
            break;
            // h     1-12����
        case FSTK_h:
            sprintf(buf, "%d", dt.GetHour12());
            strRst.append(buf);
            break;
            // hh���߸���h��01-12����
        case FSTK_hh_:
            sprintf(buf, "%02d", dt.GetHour12());
            strRst.append(buf);
            break;
            // H     0-23����
        case FSTK_H:
            sprintf(buf, "%d", dt.GetHour());
            strRst.append(buf);
            break;
            // HH���߸���H��00-23����
        case FSTK_HH_:
            sprintf(buf, "%02d", dt.GetHour());
            strRst.append(buf);
            break;
            // K,    ��ʾʱ����localtime��Ч��zzz
        case FSTK_K:
            sprintf(buf, "%c%02d%s%02d",
                    dt.m_dwTimezoneSeconds >= 0 ? '+' : '-',
                    abs(dt.m_dwTimezoneSeconds) / SECONDS_PER_HOUR,
                    formatInfo.TimeSeparator().c_str(),
                    (abs(dt.m_dwTimezoneSeconds) % SECONDS_PER_HOUR) / 60);
            strRst.append(buf);
            break;
            // m     0-59
        case FSTK_m:
            sprintf(buf, "%d", dt.GetMinute());
            strRst.append(buf);
            break;
            // mm,�����m��00-59
        case FSTK_mm_:
            sprintf(buf, "%02d", dt.GetMinute());
            strRst.append(buf);
            break;
            // M     1-12
        case FSTK_M:
            sprintf(buf, "%d", dt.GetMonth());
            strRst.append(buf);
            break;
            // MM    01-12
        case FSTK_MM:
            sprintf(buf, "%02d", dt.GetMonth());
            strRst.append(buf);
            break;
            // MMM   �·����Դ�
        case FSTK_MMM:
            strRst.append(formatInfo.AbbreviatedMonthNames()[dt.GetMonth() - 1]);
            break;
            // MMMM  �·�ȫ��
        case FSTK_MMMM:
            strRst.append(formatInfo.MonthNames()[dt.GetMonth() - 1]);
            break;
            // s     0-59
        case FSTK_s:
            sprintf(buf, "%d", dt.GetSecond());
            strRst.append(buf);
            break;
            // ss,���߸���s  00-59
        case FSTK_ss_:
            sprintf(buf, "%02d", dt.GetSecond());
            strRst.append(buf);
            break;
            // t     AM��PM��ʶ���ĵ�һ����ĸ
        case FSTK_t:

            if (dt.IsAM() && formatInfo.AMDesignator().length() > 0)
            {
                strRst.push_back(formatInfo.AMDesignator()[0]);

                if (formatInfo.AMDesignator()[0] < 0)
                {
                    assert(formatInfo.AMDesignator().length() > 1);
                    strRst.push_back(formatInfo.AMDesignator()[1]);
                }
            }
            else if (dt.IsPM() && formatInfo.PMDesignator().length() > 0)
            {
                strRst.push_back(formatInfo.PMDesignator()[0]);

                if (formatInfo.PMDesignator()[0] < 0)
                {
                    assert(formatInfo.PMDesignator().length() > 1);
                    strRst.push_back(formatInfo.PMDesignator()[1]);
                }
            }

            break;
            // tt�������t   AM��PM��ʶ��ȫ��
        case FSTK_tt_:

            if (dt.IsAM())
            {
                strRst.append(formatInfo.AMDesignator());
            }
            else
            {
                strRst.append(formatInfo.PMDesignator());
            }

            break;
            // y     1-99,����ݱ�ʾΪ1λ������λ����
        case FSTK_y:
            {
                int dwYear = dt.GetYear();

                if (dwYear > 100)
                {
                    sprintf(buf, "%02d", dwYear % 100);
                }
                else
                {
                    sprintf(buf, "%d", dwYear);
                }

                strRst.append(buf);
            }
            break;
            // yy    ��λ����ǰ����0
        case FSTK_yy:
            sprintf(buf, "%02d", dt.GetYear() % 100);
            strRst.append(buf);
            break;
            // yyy����λ����ǰ����0
        case FSTK_yyy:
            sprintf(buf, "%03d", dt.GetYear());
            strRst.append(buf);
            break;
            // yyyy����λ����ǰ����0
        case FSTK_yyyy:
            sprintf(buf, "%04d", dt.GetYear());
            strRst.append(buf);
            break;
            // yyyyy,�����y����ʾΪ5λ����ǰ����0
        case FSTK_yyyyy_:
            sprintf(buf, "%05d", dt.GetYear());
            strRst.append(buf);
            break;
            // z     +8
        case FSTK_z:
            {
                int dwTimezoneSeconds = dt.GetTimeZoneSeconds();
                sprintf(buf, "%c%d",
                        dwTimezoneSeconds >= 0 ? '+' : '-',
                        abs(dwTimezoneSeconds) / SECONDS_PER_HOUR);
                strRst.append(buf);
            }
            break;
            // zz    +08
        case FSTK_zz:
            {
                int dwTimezoneSeconds = dt.GetTimeZoneSeconds();
                sprintf(buf, "%c%02d",
                        dwTimezoneSeconds >= 0 ? '+' : '-',
                        abs(dwTimezoneSeconds) / SECONDS_PER_HOUR);
                strRst.append(buf);
            }
            break;
            // zzz   +08:00
        case FSTK_zzz:
            {
                int dwTimezoneSeconds = dt.GetTimeZoneSeconds();
                sprintf(buf, "%c%02d%s%02d",
                        dwTimezoneSeconds >= 0 ? '+' : '-',
                        abs(dwTimezoneSeconds) / SECONDS_PER_HOUR,
                        formatInfo.TimeSeparator().c_str(),
                        (abs(dwTimezoneSeconds) % SECONDS_PER_HOUR) / 60);
                strRst.append(buf);
            }
            break;
        case FSTK_DATESEP_CHAR:
            strRst.append(formatInfo.DateSeparator());
            break;
        case FSTK_TIMESEP_CHAR:
            strRst.append(formatInfo.TimeSeparator());
            break;
        default:
            assert(false);
            return "";
        }
    }

    return strRst;
}

int DateTime::GetTimeZone() const
{
    return m_dwTimezoneSeconds / SECONDS_PER_HOUR;
}

int DateTime::GetTimeZoneSeconds() const
{
    return m_dwTimezoneSeconds;
}

DateTime DateTime::GetUTCDateTime() const
{
    DateTime dt = *this;
    dt.m_Time.m_sec -= m_dwTimezoneSeconds;
    dt.m_dwTimezoneSeconds = 0;
    return dt;
}

bool DateTime::IsAM() const
{
    return GetHour() < 12;
}

bool DateTime::IsPM() const
{
    return GetHour() >= 12;
}

std::string DateTime::ExpandPredefinedFormat(
    const std::string& strFormat,
    const DateTimeFormatInfo& formatInfo,
    DateTime& dt) const
{
    string strDstFormat = strFormat;

    // �����׼��ʽ
    if (strFormat.length() == 1)
    {
        switch (strFormat[0])
        {
            /*��ʽģʽ  ��������/˵��*/
            // d         ShortDatePattern
        case 'd':
            strDstFormat = formatInfo.ShortDatePattern();
            break;
            // D             LongDatePattern
        case 'D':
            strDstFormat = formatInfo.LongDatePattern();
            break;
            // f             �������ں�ʱ�䣨�����ںͶ�ʱ�䣩
        case 'f':
            strDstFormat = formatInfo.LongDatePattern() + " " + formatInfo.ShortTimePattern();
            break;
            // F             FullDateTimePattern�������ںͳ�ʱ�䣩
        case 'F':
            strDstFormat = formatInfo.LongDatePattern() + " " + formatInfo.LongTimePattern();
            break;
            // g             ���棨�����ںͶ�ʱ�䣩
        case 'g':
            strDstFormat = formatInfo.ShortDatePattern() + " " + formatInfo.ShortTimePattern();
            break;
            // G             ���棨�����ںͳ�ʱ�䣩
        case 'G':
            strDstFormat = formatInfo.ShortDatePattern() + " " + formatInfo.LongTimePattern();
            break;
            // m��M      MonthDayPattern
        case 'm':
        case 'M':
            strDstFormat = formatInfo.MonthDayPattern();
            break;
            // o��O      ��������/ʱ��ģʽ�������ָ�ʽģʽ�£���ʽ���û��������ʼ��ʹ�ù̶�������
        case 'o':
        case 'O':
            // "yyyy'-'MM'-'dd'T'HH':'mm':'ss'.'fffffffzz";
            strDstFormat = formatInfo.RetureBackPattern();
            break;
            // r��R      RFC1123Pattern�������ָ�ʽģʽ�£���ʽ���û��������ʼ��ʹ�ù̶�������
        case 'r':
        case 'R':
            strDstFormat = formatInfo.RFC1123Pattern();
            break;
            // s         ʹ�ñ���ʱ��� SortableDateTimePattern������ ISO 8601����
            // �����ָ�ʽģʽ�£���ʽ���û��������ʼ��ʹ�ù̶�������
        case 's':
            strDstFormat = formatInfo.SortableDateTimePattern();
            break;
            // t             ShortTimePattern
        case 't':
            strDstFormat = formatInfo.ShortTimePattern();
            break;
            // T             LongTimePattern
        case 'T':
            strDstFormat = formatInfo.LongTimePattern();
            break;
            // u    ʹ��ͨ��ʱ����ʾ��ʽ�� UniversalSortableDateTimePattern��
            //      �����ָ�ʽģʽ�£���ʽ���û��������ʼ��ʹ�ù̶�������
        case 'u':
            dt = this->GetUTCDateTime();
            strDstFormat = formatInfo.UniversalSortableDateTimePattern();
            break;
            // U             ʹ��ͨ��ʱ����������ں�ʱ�䣨�����ںͳ�ʱ�䣩
        case 'U':
            dt = this->GetUTCDateTime();
            strDstFormat = formatInfo.FullDateTimePattern();
            break;
            // y��Y      YearMonthPattern
        case 'y':
        case 'Y':
            strDstFormat = formatInfo.YearMonthPattern();
            break;
        default:
            strDstFormat = "";
        }
    }

    return strDstFormat;
}

bool DateTime::ParseFormatSegment(FormatSegment* seg)
{
    if (seg->strSeg == "d")
    {
        seg->dwKind = FSTK_d;
    }
    // dd,   01-31
    else if (seg->strSeg == "dd")
    {
        seg->dwKind = FSTK_dd;
    }
    // ddd,  һ����ĳ�����������
    else if (seg->strSeg == "ddd")
    {
        seg->dwKind = FSTK_ddd;
    }
    // dddd���߸����d   ��һ����ĳ���ȫ��
    else if (seg->strSeg.length() >= 4 && seg->strSeg.find("dddd") == 0)
    {
        seg->dwKind = FSTK_dddd_;
    }
    // f,ff,fff,ffff,fffff,ffffff
    else if (seg->strSeg[0] == 'f')
    {
        seg->dwKind = (FormatSegmentTokenKind)(FSTK_f + seg->strSeg.length() - 1);
    }
    // F,FF,FFF,FFFF,FFFFF,FFFFFF
    else if (seg->strSeg[0] == 'F')
    {
        seg->dwKind = (FormatSegmentTokenKind)(FSTK_F + seg->strSeg.length() - 1);
    }
    // g,gg���߸���g
    else if (seg->strSeg[0] == 'g')
    {
        seg->dwKind = FSTK_g_;
    }
    // h     1-12����
    else if (seg->strSeg == "h")
    {
        seg->dwKind = FSTK_h;
    }
    // hh���߸���h��01-12����
    else if (seg->strSeg.find("hh") == 0)
    {
        seg->dwKind = FSTK_hh_;
    }
    // H     0-23����
    else if (seg->strSeg == "H")
    {
        seg->dwKind = FSTK_H;
    }
    // HH���߸���H��00-23����
    else if (seg->strSeg.find("HH") == 0)
    {
        seg->dwKind = FSTK_HH_;
    }
    // K,    ��ʾʱ����localtime��Ч��zzz
    else if (seg->strSeg == "K")
    {
        seg->dwKind = FSTK_K;
    }
    // m     0-59
    else if (seg->strSeg == "m")
    {
        seg->dwKind = FSTK_m;
    }
    // mm,�����m��00-59
    else if (seg->strSeg.find("mm") == 0)
    {
        seg->dwKind = FSTK_mm_;
    }
    // M     1-12
    else if (seg->strSeg == "M")
    {
        seg->dwKind = FSTK_M;
    }
    // MM    01-12
    else if (seg->strSeg == "MM")
    {
        seg->dwKind = FSTK_MM;
    }
    // MMM   �·����Դ�
    else if (seg->strSeg == "MMM")
    {
        seg->dwKind = FSTK_MMM;
    }
    // MMMM  �·�ȫ��
    else if (seg->strSeg == "MMMM")
    {
        seg->dwKind = FSTK_MMMM;
    }
    // s     0-59
    else if (seg->strSeg == "s")
    {
        seg->dwKind = FSTK_s;
    }
    // ss,���߸���s  00-59
    else if (seg->strSeg.find("ss") == 0)
    {
        seg->dwKind = FSTK_ss_;
    }
    // t     AM��PM��ʶ���ĵ�һ����ĸ
    else if (seg->strSeg == "t")
    {
        seg->dwKind = FSTK_t;
    }
    // tt�������t   AM��PM��ʶ��ȫ��
    else if (seg->strSeg.find("tt") == 0)
    {
        seg->dwKind = FSTK_tt_;
    }
    // y     1-99,����ݱ�ʾΪ1λ������λ����
    else if (seg->strSeg == "y")
    {
        seg->dwKind = FSTK_y;
    }
    // yy    ��λ����ǰ����0
    else if (seg->strSeg == "yy")
    {
        seg->dwKind = FSTK_yy;
    }
    // yyy����λ����ǰ����0
    else if (seg->strSeg == "yyy")
    {
        seg->dwKind = FSTK_yyy;
    }
    // yyyy����λ����ǰ����0
    else if (seg->strSeg == "yyyy")
    {
        seg->dwKind = FSTK_yyyy;
    }
    // yyyyy,�����y����ʾΪ5λ����ǰ����0
    else if (seg->strSeg.find("yyyyy") == 0)
    {
        seg->dwKind = FSTK_yyyyy_;
    }
    // z     +8
    else if (seg->strSeg == "z")
    {
        seg->dwKind = FSTK_z;
    }
    // zz    +08
    else if (seg->strSeg == "zz")
    {
        seg->dwKind = FSTK_zz;
    }
    // zzz   +08:00
    else if (seg->strSeg == "zzz")
    {
        seg->dwKind = FSTK_zzz;
    }
    else if (seg->strSeg == "/")
    {
        seg->dwKind = FSTK_DATESEP_CHAR;
    }
    else if (seg->strSeg == ":")
    {
        seg->dwKind = FSTK_TIMESEP_CHAR;
    }
    else
        return false;

    return true;
}

void DateTime::SetYear(int dwYear)
{
    *this = DateTime(dwYear, this->GetMonth(), this->GetDayOfMonth(),
                     this->GetHour(), this->GetMinute(), this->GetSecond(),
                     this->GetMilliSecond(), this->GetMicroSecond(),
                     this->GetTimeZoneSeconds());
}

void DateTime::SetMonth(int dwMonth)
{
    assert(dwMonth >= 1 && dwMonth <= 12);
    *this = DateTime(this->GetYear(), dwMonth, this->GetDayOfMonth(),
                     this->GetHour(), this->GetMinute(), this->GetSecond(),
                     this->GetMilliSecond(), this->GetMicroSecond(),
                     this->GetTimeZoneSeconds());
}

void DateTime::SetDayOfMonth(int dwDay)
{
    assert(dwDay >= 1 && dwDay <= 31);
    *this = DateTime(this->GetYear(), this->GetMonth(), dwDay,
                     this->GetHour(), this->GetMinute(), this->GetSecond(),
                     this->GetMilliSecond(), this->GetMicroSecond(),
                     this->GetTimeZoneSeconds());
}

void DateTime::SetHour(int dwHour)
{
    m_Time.m_sec = ((m_Time.m_sec - m_Time.m_sec % SECONDS_PER_DAY +
                        m_Time.m_sec % SECONDS_PER_HOUR) +
                    (dwHour % 24) * SECONDS_PER_HOUR);
}

void DateTime::SetHour12(int dwHour12, bool bAM)
{
    dwHour12 = (dwHour12 + (bAM ? 0 : 12));

    if (dwHour12 == 24)
    {
        dwHour12 = 0;
    }

    SetHour(dwHour12);
}

void DateTime::SetMinute(int dwMinute)
{
    m_Time.m_sec = (m_Time.m_sec - m_Time.m_sec % SECONDS_PER_HOUR + m_Time.m_sec % 60) +
                   (dwMinute % 60) * 60;
}

void DateTime::SetSecond(int dwSecond)
{
    m_Time.m_sec = (m_Time.m_sec - m_Time.m_sec % 60) + dwSecond % 60;
}

void DateTime::SetMilliSecond(int dwMilliSecond)
{
    m_Time.m_usec = m_Time.m_usec % 1000 + (dwMilliSecond % 1000) * 1000;
}

void DateTime::SetMicroSecond(int dwMicroSecond)
{
    m_Time.m_usec = (m_Time.m_usec - m_Time.m_usec % 1000) + dwMicroSecond % 1000;
}

void DateTime::SetTimeZone(int dwTimeZone)
{
    this->m_dwTimezoneSeconds = dwTimeZone * SECONDS_PER_HOUR;
}

void DateTime::SetTimeZoneSeconds(int dwTimezoneSeconds)
{
    this->m_dwTimezoneSeconds = dwTimezoneSeconds;
}

void DateTime::SetDay(const DateTime& dt)
{
    *this = DateTime(dt.GetYear(), dt.GetMonth(), dt.GetDayOfMonth(),
                     this->GetHour(), this->GetMinute(), this->GetSecond(),
                     this->GetMilliSecond(), this->GetMicroSecond(),
                     this->GetTimeZoneSeconds());
}

void DateTime::SetTime(const TimeSpan& ts)
{
    SetHour(ts.GetHours());
    SetMinute(ts.GetMinutes());
    SetSecond(ts.GetSeconds());
    SetMilliSecond(ts.GetMilliSeconds());
    SetMicroSecond(ts.GetMicroSeconds());
}

void DateTime::SetAMOrPM(bool bAM)
{
    if (IsAM() == bAM)
    {
        return;
    }
    else
    {
        int dwHour = GetHour();

        if (bAM)
        {
            dwHour -= 12;
        }
        else
        {
            dwHour += 12;
        }

        SetHour(dwHour);
    }
}

bool DateTime::TryParseSegment(
    const std::string& strTime,
    size_t& dwIndex,
    const vector<FormatSegment> &vecSegment,
    const size_t dwSegIndex,
    DateTimeInfo* date_info,
    const DateTimeFormatInfo& formatInfo)
{
    size_t dwPos = 0;
    int dwNum = 0;
    vector<FormatSegment>::const_iterator itr = vecSegment.begin() + dwSegIndex;
    const size_t dwLen = strTime.length();

    switch (itr->dwKind)
    {
    case FSTK_String:
        dwPos = strTime.find(itr->strSeg, dwIndex);

        // dwPosd
        if (dwPos != string::npos &&
            (dwPos == dwIndex || IsSpaceChars(strTime, dwIndex, dwPos - dwIndex)))
        {
            dwIndex = dwPos + itr->strSeg.length();
        }
        else
        {
            if (itr->strSeg == ".")
            {
                vector<FormatSegment>::const_iterator itrNext =  itr + 1;

                if (itrNext == vecSegment.end())
                {
                    return false;
                }

                // s.F����ƥ��s
                if (itrNext->dwKind != FSTK_String &&
                    itrNext->strSeg.length() > 0 &&
                    itrNext->strSeg[0] == 'F')
                {
                    break;
                }
            }

            return false;
        }

        break;
    case FSTK_d:
        // dd,   01-31
    case FSTK_dd:

        if (GetNumFromString(strTime, dwIndex, dwNum, 2))
        {
            if (dwNum >= 1 && dwNum <= 31)
            {
                date_info->day_of_month = dwNum;
                break;
            }
        }

        return false;
        // ddd,  һ����ĳ�����������
    case FSTK_ddd:

        for (vector<string>::const_iterator itrNames = formatInfo.AbbreviatedDayNames().begin();
             itrNames != formatInfo.AbbreviatedDayNames().end(); ++itrNames)
        {
            dwPos = strTime.find(*itrNames, dwIndex);

            if (dwPos != string::npos &&
                (dwPos == dwIndex || IsSpaceChars(strTime, dwIndex, dwPos - dwIndex)))
            {
                dwIndex += dwPos + itrNames->length();
                break;
            }
        }

        return false;
        // dddd���߸����d   ��һ����ĳ���ȫ��
    case FSTK_dddd_:

        for (vector<string>::const_iterator itrNames = formatInfo.DayNames().begin();
             itrNames != formatInfo.DayNames().end(); ++itrNames)
        {
            dwPos = strTime.find(*itrNames, dwIndex);

            if (dwPos != string::npos &&
                (dwPos == dwIndex || IsSpaceChars(strTime, dwIndex, dwPos - dwIndex)))
            {
                dwIndex = dwPos + itrNames->length();
                break;
            }
        }

        return false;
        // f,ff,fff,ffff,fffff,ffffff
    case FSTK_f:
    case FSTK_ff:
    case FSTK_fff:
    case FSTK_ffff:
    case FSTK_fffff:
    case FSTK_ffffff: // f����ƥ��һ�����֣�F����ƥ��ʡ�Ե���

        if (!IS_DIGIT(strTime[dwIndex]))
        {
            return false;
        }

        // F,FF,FFF,FFFF,FFFFF,FFFFFF
    case FSTK_F:
    case FSTK_FF:
    case FSTK_FFF:
    case FSTK_FFFF:
    case FSTK_FFFFF:
    case FSTK_FFFFFF:
        {
            dwNum = 0;
            int dwMode = 100000;

            while (dwIndex < dwLen && IS_DIGIT(strTime[dwIndex]) && dwMode > 0)
            {
                dwNum += (strTime[dwIndex] - '0') * dwMode;
                dwIndex++;
                dwMode /= 10;
            }

            date_info->microseconds = dwNum;
        }
        break;
        // g,gg���߸���g
    case FSTK_g_:
        dwPos = strTime.find(formatInfo.EraNames()[0], dwIndex);

        if (dwPos != string::npos &&
            (dwPos == dwIndex || IsSpaceChars(strTime, dwIndex, dwPos - dwIndex)))
        {
            dwIndex += formatInfo.EraNames()[0].length();
        }
        else
            return false;

        break;
        // h     1-12����
    case FSTK_h:
        // hh���߸���h��01-12����
    case FSTK_hh_:

        if (GetNumFromString(strTime, dwIndex, dwNum, 2))
        {
            if (dwNum >= 1 && dwNum <= 12)
            {
                // dwHour12 = dwNum;
                date_info->hour = dwNum;
                date_info->is_hour12 = true;
                break;
            }
        }

        return false;
        // H     0-23����
    case FSTK_H:
        // HH���߸���H��00-23����
    case FSTK_HH_:

        if (GetNumFromString(strTime, dwIndex, dwNum, 2))
        {
            if (dwNum >= 0 && dwNum <= 23)
            {
                date_info->hour = dwNum;
                date_info->is_hour12 = false;
                break;
            }
        }

        return false;
        // K,    ��ʾʱ����localtime��Ч��zzz��+08:00
    case FSTK_K:
        {
            int dwTimezoneSeconds = 0;

            while (IS_BLANK_SPACE_CHAR(strTime[dwIndex]) && dwIndex < dwLen)
            {
                dwIndex++;
            }

            if (dwIndex == dwLen)
            {
                return false;
            }

            if (strTime[dwIndex] == '+')
            {
                dwIndex++;

                if (!GetNumFromString(strTime, dwIndex, dwNum, 2))
                {
                    return false;
                }

                if (dwNum < 0 || dwNum > 23)
                {
                    return false;
                }
            }
            else if (strTime[dwIndex] == '-')
            {
                dwIndex++;

                if (!GetNumFromString(strTime, dwIndex, dwNum, 2))
                {
                    return false;
                }

                if (dwNum < 0 || dwNum > 23)
                {
                    return false;
                }

                dwNum = -dwNum;
            }

            dwTimezoneSeconds = dwNum * SECONDS_PER_HOUR;
            // ��ȡ:00
            dwPos = strTime.find(formatInfo.TimeSeparator(), dwIndex);

            if (dwPos == dwIndex)
            {
                dwIndex += formatInfo.TimeSeparator().length();

                if (GetNumFromString(strTime, dwIndex, dwNum, 2))
                {
                    if (dwNum >= 0 && dwNum < 60)
                    {
                        dwTimezoneSeconds += dwNum * 60;
                    }
                }
            }

            // K��Ҫ����ʱ��
            date_info->timezone_seconds = dwTimezoneSeconds;
        }
        return false;
        // m     0-59
    case FSTK_m:
        // mm,�����m��00-59
    case FSTK_mm_:

        if (GetNumFromString(strTime, dwIndex, dwNum, 2))
        {
            if (dwNum >= 0 && dwNum <= 59)
            {
                date_info->minute = dwNum;
                break;
            }
        }

        return false;
        // M     1-12
    case FSTK_M:
        // MM    01-12
    case FSTK_MM:

        if (GetNumFromString(strTime, dwIndex, dwNum, 2))
        {
            if (dwNum >= 1 && dwNum <= 12)
            {
                date_info->month = dwNum;
                break;
            }
        }

        return false;
        // MMM   �·����Դ�
    case FSTK_MMM:
        dwNum = 1;

        for (vector<string>::const_iterator itrNames = formatInfo.AbbreviatedMonthNames().begin();
             itrNames != formatInfo.AbbreviatedMonthNames().end(); ++itrNames)
        {
            dwPos = strTime.find(*itrNames, dwIndex);

            if (dwPos != string::npos &&
                (dwPos == dwIndex || IsSpaceChars(strTime, dwIndex, dwPos - dwIndex)))
            {
                dwIndex = dwPos + itrNames->length();
                date_info->month = dwNum;
                break;
            }

            dwNum++;
        }

        return false;
        // MMMM  �·�ȫ��
    case FSTK_MMMM:
        dwNum = 1;

        for (vector<string>::const_iterator itrNames = formatInfo.MonthNames().begin();
             itrNames != formatInfo.MonthNames().end(); ++itrNames)
        {
            dwPos = strTime.find(*itrNames, dwIndex);

            if (dwPos != string::npos &&
                (dwPos == dwIndex || IsSpaceChars(strTime, dwIndex, dwPos - dwIndex)))
            {
                dwIndex = dwPos + itrNames->length();
                date_info->month = dwNum;
                break;
            }

            dwNum++;
        }

        return false;
        break;
        // s     0-59
    case FSTK_s:
        // ss,���߸���s  00-59
    case FSTK_ss_:

        if (GetNumFromString(strTime, dwIndex, dwNum, 2))
        {
            if (dwNum >= 0 && dwNum <= 59)
            {
                date_info->second = dwNum;
                break;
            }
        }

        return false;
        // t     AM��PM��ʶ���ĵ�һ���ַ�
    case FSTK_t:
        {
            string strDst = formatInfo.AMDesignator()[0] < 0 ?
                formatInfo.AMDesignator().substr(0, 2) :
                formatInfo.AMDesignator().substr(0, 1);
            dwPos = strTime.find(strDst, dwIndex);

            if (dwPos != string::npos &&
                (dwPos == dwIndex || IsSpaceChars(strTime, dwIndex, dwPos - dwIndex)))
            {
                dwIndex = dwPos + strDst.length();
                date_info->is_set_am_or_pm = true;
                date_info->am_or_pm = true;
                break;
            }

            strDst = formatInfo.PMDesignator()[0] < 0 ?
                formatInfo.PMDesignator().substr(0, 2) :
                formatInfo.PMDesignator().substr(0, 1);
            dwPos = strTime.find(strDst, dwIndex);

            if (dwPos != string::npos &&
                (dwPos == dwIndex || IsSpaceChars(strTime, dwIndex, dwPos - dwIndex)))
            {
                dwIndex = dwPos + strDst.length();
                date_info->is_set_am_or_pm = true;
                date_info->am_or_pm = false;
                break;
            }
        }
        return false;
        // tt�������t   AM��PM��ʶ��ȫ��
    case FSTK_tt_:
        dwPos = strTime.find(formatInfo.AMDesignator(), dwIndex);

        if (dwPos != string::npos &&
            (dwPos == dwIndex || IsSpaceChars(strTime, dwIndex, dwPos - dwIndex)))
        {
            dwIndex = dwPos + formatInfo.AMDesignator().length();
            date_info->is_set_am_or_pm = true;
            date_info->am_or_pm = true;
            break;
        }

        dwPos = strTime.find(formatInfo.PMDesignator(), dwIndex);

        if (dwPos != string::npos &&
            (dwPos == dwIndex || IsSpaceChars(strTime, dwIndex, dwPos - dwIndex)))
        {
            dwIndex = dwPos + formatInfo.PMDesignator().length();
            date_info->is_set_am_or_pm = true;
            date_info->am_or_pm = false;
            break;
        }

        return false;
        // y     1-99,����ݱ�ʾΪ1λ������λ����
    case FSTK_y:
        // yy    ��λ����ǰ����0
    case FSTK_yy:

        if (GetNumFromString(strTime, dwIndex, dwNum, 2))
        {
            int dwCurYear = DateTime::Now().GetYear();
            dwNum += (dwCurYear - dwCurYear % 100);

            if (dwCurYear % 100 > dwNum)
            {
                dwNum -= 100;
            }

            date_info->year = dwNum;
            break;
        }

        return false;
        // yyy����λ����ǰ����0
    case FSTK_yyy:

        if (GetNumFromString(strTime, dwIndex, dwNum, 3))
        {
            date_info->year = dwNum;
            break;
        }

        return false;
        // yyyy����λ����ǰ����0
    case FSTK_yyyy:

        if (GetNumFromString(strTime, dwIndex, dwNum, 4))
        {
            date_info->year = dwNum;
            break;
        }

        return false;
        // yyyyy,�����y����ʾΪ5λ����ǰ����0
    case FSTK_yyyyy_:

        if (GetNumFromString(strTime, dwIndex, dwNum, 5))
        {
            date_info->year = dwNum;
            break;
        }

        return false;
        // z     +8
    case FSTK_z:
        // zz    +08
    case FSTK_zz:
        // zzz   +08:00
    case FSTK_zzz:
        {
            int dwTimezoneSeconds = 0;

            while (IS_BLANK_SPACE_CHAR(strTime[dwIndex]) && dwIndex < dwLen)
            {
                dwIndex++;
            }

            if (dwIndex == dwLen)
            {
                return false;
            }

            if (strTime[dwIndex] == '+')
            {
                dwIndex++;

                if (!GetNumFromString(strTime, dwIndex, dwNum, 2))
                {
                    return false;
                }

                if (dwNum < 0 || dwNum > 23)
                {
                    return false;
                }
            }
            else if (strTime[dwIndex] == '-')
            {
                dwIndex++;

                if (!GetNumFromString(strTime, dwIndex, dwNum, 2))
                {
                    return false;
                }

                if (dwNum < 0 || dwNum > 23)
                {
                    return false;
                }

                dwNum = -dwNum;
            }

            dwTimezoneSeconds = dwNum * SECONDS_PER_HOUR;
            // ��ȡ:00
            dwPos = strTime.find(formatInfo.TimeSeparator(), dwIndex);

            if (dwPos == dwIndex)
            {
                dwIndex += formatInfo.TimeSeparator().length();

                if (GetNumFromString(strTime, dwIndex, dwNum, 2))
                {
                    if (dwNum >= 0 && dwNum < 60)
                    {
                        dwTimezoneSeconds += dwNum * 60;
                    }
                }
            }

            date_info->timezone_seconds = dwTimezoneSeconds;
            break;
        }
        return false;
        // /
    case FSTK_DATESEP_CHAR:
        dwPos = strTime.find(formatInfo.DateSeparator(), dwIndex);

        if (dwIndex == dwPos)
        {
            dwIndex += formatInfo.DateSeparator().length();
            break;
        }

        return false;
        // :
    case FSTK_TIMESEP_CHAR:
        dwPos = strTime.find(formatInfo.TimeSeparator(), dwIndex);

        if (dwIndex == dwPos)
        {
            dwIndex += formatInfo.TimeSeparator().length();
            break;
        }

        return false;
    default:
        return false;
    }
    return true;
}

void DateTime::MakeDateTime(const DateTimeInfo& date_info, DateTime* dt)
{
    // ��������ʱ��
    if (date_info.timezone_seconds != -1) {
        dt->SetTimeZoneSeconds(date_info.timezone_seconds);
    } else {
        dt->SetTimeZoneSeconds(DateTime::Now().GetTimeZoneSeconds());
    }
    if (date_info.hour != -1) {
        if (date_info.is_hour12) {
            dt->SetHour12(date_info.hour, dt->IsAM());
        } else {
            dt->SetHour(date_info.hour);
        }
    }
    if (date_info.day_of_month != -1) {
        dt->SetDayOfMonth(date_info.day_of_month);
    }
    if (date_info.microseconds != -1) {
       dt->m_Time.m_usec = date_info.microseconds;
    }
    if (date_info.minute != -1) {
        dt->SetMinute(date_info.minute);
    }
    if (date_info.month != -1) {
        dt->SetMonth(date_info.month);
    }
    if (date_info.second != -1) {
        dt->SetSecond(date_info.second);
    }
    if (date_info.is_set_am_or_pm) {
        dt->SetAMOrPM(date_info.am_or_pm);
    }
    if (date_info.year != -1) {
        dt->SetYear(date_info.year);
    }
}

int DateTime::GetCurrentTimezoneSeconds()
{
    tzset();
    return -timezone;
}

// *************** TimeCounter *************************

TimeCounter::TimeCounter()
{
    m_TimeSpan = TimeSpan::Zero;
    m_LastTick = GetCurTick();
}

TimeCounter::TimeCounter(const TimeSpan& ts)
{
    m_TimeSpan = ts;
    m_LastTick = GetCurTick();
}

void TimeCounter::Reset()
{
    m_TimeSpan = TimeSpan::Zero;
    m_LastTick = GetCurTick();
}

void TimeCounter::Start()
{
    m_LastTick = GetCurTick();
}

void TimeCounter::Pause()
{
    TimeTick dtNow = GetCurTick();
    m_TimeSpan += TimeSpan(dtNow - m_LastTick);
    m_LastTick = dtNow;
}

TimeSpan TimeCounter::GetTimeSpan() const
{
    return m_TimeSpan;
}

std::string TimeCounter::ToString() const
{
    return m_TimeSpan.ToString();
}

TimeTick TimeCounter::GetCurTick() const
{
#ifdef _WIN32
    LARGE_INTEGER ddwTick;
    QueryPerformanceCounter(&ddwTick);
    LARGE_INTEGER ddwFreq;
    QueryPerformanceFrequency(&ddwFreq);
    TimeTick tt(ddwTick.QuadPart / ddwFreq.QuadPart,
                (int)((ddwTick.QuadPart % ddwFreq.QuadPart) * MICROSECONDS_PER_SECOND /
                      ddwFreq.QuadPart));
    return tt;
#else
    return DateTime::Now().m_Time;
#endif
}

// ************************** DateTimeFormatInfo **********************************************

// en-US��DateTimeFormatInfo
const DateTimeFormatInfo DateTimeFormatInfo::en_USInfo("en-US");
// zh-CN��DateTimeFormatInfo
const DateTimeFormatInfo DateTimeFormatInfo::zh_CNInfo("zh-CN");

DateTimeFormatInfo::DateTimeFormatInfo(const std::string& cultureStr /*= "zh-CN"*/)
{
    m_Name = cultureStr;

    if (cultureStr == "en-US")
    {
        _Creat_en_US();
        return;
    }
    else if (cultureStr == "zh-CN")
    {
        _Creat_zh_CN();
        return;
    }

    _Creat_zh_CN();
}

void DateTimeFormatInfo::_Creat_zh_CN()
{
    m_ShortDatePattern = "yyyy-M-d";
    m_LongDatePattern = "yyyy'��'M'��'d'��'";
    m_FullDateTimePattern = "yyyy'��'M'��'d'��' H:mm:ss";
    m_MonthDayPattern = "M'��'d'��'";
    m_RFC1123Pattern = "ddd, dd MMM yyyy HH':'mm':'ss 'GMT'";
    m_SortableDateTimePattern = "yyyy'-'MM'-'dd'T'HH':'mm':'ss";
    m_ShortTimePattern = "H:mm";
    m_LongTimePattern = "H:mm:ss";
    m_UniversalSortableDateTimePattern = "yyyy'-'MM'-'dd HH':'mm':'ss'Z'";
    m_YearMonthPattern = "yyyy'��'M'��'";
    m_RetureBackPattern = "yyyy'-'MM'-'dd'T'HH':'mm':'ss'.'fffffffzz";

    m_EraNames.assign(1, "��Ԫ");

    m_AbbreviatedDayNames.assign(7, "");
    m_AbbreviatedDayNames[0] = "��";
    m_AbbreviatedDayNames[1] = "һ";
    m_AbbreviatedDayNames[2] = "��";
    m_AbbreviatedDayNames[3] = "��";
    m_AbbreviatedDayNames[4] = "��";
    m_AbbreviatedDayNames[5] = "��";
    m_AbbreviatedDayNames[6] = "��";

    m_DayNames.assign(7, "");
    m_DayNames[0] = "������";
    m_DayNames[1] = "����һ";
    m_DayNames[2] = "���ڶ�";
    m_DayNames[3] = "������";
    m_DayNames[4] = "������";
    m_DayNames[5] = "������";
    m_DayNames[6] = "������";

    m_AbbreviatedMonthNames.assign(13, "");
    m_AbbreviatedMonthNames[0] = "һ��";
    m_AbbreviatedMonthNames[1] = "����";
    m_AbbreviatedMonthNames[2] = "����";
    m_AbbreviatedMonthNames[3] = "����";
    m_AbbreviatedMonthNames[4] = "����";
    m_AbbreviatedMonthNames[5] = "����";
    m_AbbreviatedMonthNames[6] = "����";
    m_AbbreviatedMonthNames[7] = "����";
    m_AbbreviatedMonthNames[8] = "����";
    m_AbbreviatedMonthNames[9] = "ʮ��";
    m_AbbreviatedMonthNames[10] = "ʮһ��";
    m_AbbreviatedMonthNames[11] = "ʮ����";

    m_MonthNames.assign(13, "");
    m_MonthNames[0] = "һ��";
    m_MonthNames[1] = "����";
    m_MonthNames[2] = "����";
    m_MonthNames[3] = "����";
    m_MonthNames[4] = "����";
    m_MonthNames[5] = "����";
    m_MonthNames[6] = "����";
    m_MonthNames[7] = "����";
    m_MonthNames[8] = "����";
    m_MonthNames[9] = "ʮ��";
    m_MonthNames[10] = "ʮһ��";
    m_MonthNames[11] = "ʮ����";

    m_AMDesignator = "����";
    m_PMDesignator = "����";
    m_TimeSeparator = ":";
    m_DateSeparator = "-";
}

void DateTimeFormatInfo::_Creat_en_US()
{
    m_ShortDatePattern = "M/d/yyyy";
    m_LongDatePattern = "dddd, MMMM dd, yyyy";
    m_FullDateTimePattern = "dddd, MMMM dd, yyyy h:mm:ss tt";
    m_MonthDayPattern = "MMMM dd";
    m_RFC1123Pattern = "ddd, dd MMM yyyy HH':'mm':'ss 'GMT'";
    m_SortableDateTimePattern = "yyyy'-'MM'-'dd'T'HH':'mm':'ss";
    m_ShortTimePattern = "h:mm tt";
    m_LongTimePattern = "h:mm:ss tt";
    m_UniversalSortableDateTimePattern = "yyyy'-'MM'-'dd HH':'mm':'ss'Z'";
    m_YearMonthPattern = "MMMM, yyyy";
    m_RetureBackPattern = "yyyy'-'MM'-'dd'T'HH':'mm':'ss'.'fffffffzz";
    m_EraNames.assign(1, "A.D.");

    m_AbbreviatedDayNames.assign(7, "");
    m_AbbreviatedDayNames[0] = "Sun";
    m_AbbreviatedDayNames[1] = "Mon";
    m_AbbreviatedDayNames[2] = "Tue";
    m_AbbreviatedDayNames[3] = "Wed";
    m_AbbreviatedDayNames[4] = "Thu";
    m_AbbreviatedDayNames[5] = "Fri";
    m_AbbreviatedDayNames[6] = "Sat";

    m_DayNames.assign(7, "");
    m_DayNames[0] = "Sunday";
    m_DayNames[1] = "Monday";
    m_DayNames[2] = "Tuesday";
    m_DayNames[3] = "Wednesday";
    m_DayNames[4] = "Thursday";
    m_DayNames[5] = "Friday";
    m_DayNames[6] = "Saturday";

    m_AbbreviatedMonthNames.assign(13, "");
    m_AbbreviatedMonthNames[0] = "Jan";
    m_AbbreviatedMonthNames[1] = "Feb";
    m_AbbreviatedMonthNames[2] = "Mar";
    m_AbbreviatedMonthNames[3] = "Apr";
    m_AbbreviatedMonthNames[4] = "May";
    m_AbbreviatedMonthNames[5] = "Jun";
    m_AbbreviatedMonthNames[6] = "Jul";
    m_AbbreviatedMonthNames[7] = "Aug";
    m_AbbreviatedMonthNames[8] = "Sep";
    m_AbbreviatedMonthNames[9] = "Oct";
    m_AbbreviatedMonthNames[10] = "Nov";
    m_AbbreviatedMonthNames[11] = "Dec";

    m_MonthNames.assign(13, "");
    m_MonthNames[0] = "January";
    m_MonthNames[1] = "February";
    m_MonthNames[2] = "March";
    m_MonthNames[3] = "April";
    m_MonthNames[4] = "May";
    m_MonthNames[5] = "June";
    m_MonthNames[6] = "July";
    m_MonthNames[7] = "August";
    m_MonthNames[8] = "September";
    m_MonthNames[9] = "October";
    m_MonthNames[10] = "November";
    m_MonthNames[11] = "December";

    m_AMDesignator = "AM";
    m_PMDesignator = "PM";
    m_TimeSeparator = ":";
    m_DateSeparator = "/";
}

const std::vector<std::string> &DateTimeFormatInfo::GetAllPatterns()const
{
    if (m_AllPatterns.size() == 0)
    {
        vector<string> *pVec = (vector<string> *)&m_AllPatterns;
        pVec->push_back(m_LongDatePattern + " " + m_LongTimePattern);
        pVec->push_back(m_LongDatePattern + " " + m_ShortTimePattern);
        pVec->push_back(m_ShortDatePattern + " " + m_LongTimePattern);
        pVec->push_back(m_ShortDatePattern + " " + m_ShortTimePattern);

        pVec->push_back(m_FullDateTimePattern);
        pVec->push_back(m_RFC1123Pattern);
        pVec->push_back(m_RetureBackPattern);
        pVec->push_back(m_SortableDateTimePattern);
        pVec->push_back(m_UniversalSortableDateTimePattern);
        pVec->push_back(m_LongDatePattern);
        pVec->push_back(m_ShortDatePattern);
        pVec->push_back(m_YearMonthPattern);
        pVec->push_back(m_MonthDayPattern);
        pVec->push_back(m_LongTimePattern);
        pVec->push_back(m_ShortTimePattern);
    }

    return m_AllPatterns;
}

} // end of namespace common
