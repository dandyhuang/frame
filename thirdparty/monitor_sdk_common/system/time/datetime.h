//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 19:09
//  @file:      datetime.h
//  @brief:
//
//  TimeSpan��              ʱ�����
//  DateTime��              ����ʱ���࣬windows�Ͼ�ȷ�����룬linux�Ͼ�ȷ��΢��
//  TimeCounter��           ��ʱ���࣬��ʱ��ȷ��΢��
//  DateTimeFormatInfo��    ʱ���ʽ��Ϣ�֧࣬��zh-CN,en-US��������ʱ���ʽ
//
//  @since��        2010.04.13
//  @modify history:
//      2010.04.20      ���DateTime�ı�׼��ʽģʽ���Զ����ʽģʽ֧�֣���дToString��������дTimeSpan��ToString��Parse����
//      2010.04.21      ��дDateTime��Parse����
//      2010.11.23      ����DateTime::Today()�� Datetime::Time()��bug
//********************************************************************


#ifndef COMMON_SYSTEM_TIME_DATETIME_H
#define COMMON_SYSTEM_TIME_DATETIME_H
#pragma once

/**
���ڱ�׼��ʽģʽ���Զ����ʽģʽ:
1.��׼��ʽģʽ����׼��ʽģʽ����һ���ַ���ʾ���μ�MSDN�ĵ���ms-help://MS.MSDNQTR.v90.chs/dv_fxfund/html/bb79761a-ca08-44ee-b142-b06b3e2fc22b.htm
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
s           ʹ�ñ���ʱ��� SortableDateTimePattern������ ISO 8601���������ָ�ʽģʽ�£���ʽ���û��������ʼ��ʹ�ù̶�������
t           ShortTimePattern
T           LongTimePattern
u           ʹ��ͨ��ʱ����ʾ��ʽ�� UniversalSortableDateTimePattern�������ָ�ʽģʽ�£���ʽ���û��������ʼ��ʹ�ù̶�������
U           ʹ��ͨ��ʱ����������ں�ʱ�䣨�����ںͳ�ʱ�䣩
y��Y        YearMonthPattern

2.�Զ����ʽģʽ��������ģʽ�ַ�Ӧ����������������һ���ַ����Զ���ģʽ��Ҫ��ǰ�����%���������ڱ�׼��ʽģʽ
�μ�MSDN�ĵ���ms-help://MS.MSDNQTR.v90.chs/dv_fxfund/html/98b374e3-0cc2-4c78-ab44-efb671d71984.htm
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
K       Kind ���ԵĲ�ֵͬ�������ء�Utc ��δָ����
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
\ c     ���� c �������ַ�����ԭ����ʾ�ַ�����Ҫ��ʾ��б���ַ���Ӧ�ó���Ӧʹ�á�\\����         */

#include <time.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include "thirdparty/monitor_sdk_common/base/deprecate.h"

namespace common {

class TimeSpan;
//
///// ���ص�ǰTimeSpan�ľ���ֵ
//DEPRECATED_BY(TimeSpan::Abs)
//TimeSpan abs(const TimeSpan &ts);
//

/// ��ʾʱ�����
class TimeTick
{
    time_t m_sec;   // ����,ȡֵ��Χ[0x8000000000000000,0x7FFFFFFFFFFFFFFF]
    int m_usec;     // ΢����,ȡֵ��Χ(-1000000,1000000),������time��ͬ
public:
    friend class TimeSpan;
    friend class DateTime;

    TimeTick(time_t tm, int us);
    TimeTick(const TimeTick &tt);
    TimeTick();
    void Normalize();
    bool IsNormalized()const;

    TimeTick operator+(const TimeTick &tt)const;
    TimeTick &operator+=(const TimeTick &tt);
    TimeTick operator-(const TimeTick &tt)const;
    TimeTick &operator-=(const TimeTick &tt);
    TimeTick operator-()const;

    bool operator >(const TimeTick &tt)const;
    bool operator <(const TimeTick &tt)const;
    bool operator >=(const TimeTick &tt)const;
    bool operator <=(const TimeTick &tt)const;
    bool operator ==(const TimeTick &tt)const;
    bool operator !=(const TimeTick &tt)const;
};

/// ��ʾһ��ʱ�����
class TimeSpan
{
protected:
    TimeTick m_Time;
public:
    friend class DateTime;
    TimeSpan();

    TimeSpan(const TimeTick &tmTick);
    /// ���µ� TimeSpan ��ʼ��Ϊָ����Сʱ����������������
    TimeSpan(int dwHour, int dwMinute, int dwSecond);
    /// ���µ� TimeSpan ��ʼ��Ϊָ����������Сʱ����������������
    TimeSpan(int dwDay, int dwHour, int dwMinute, int dwSecond);
    /// ���µ� TimeSpan ��ʼ��Ϊָ����������Сʱ�����������������ͺ�����
    TimeSpan(int dwDay, int dwHour, int dwMinute, int dwSecond, int dwMilliSecond);
    /// ���µ� TimeSpan ��ʼ��Ϊָ����������Сʱ��������������������������΢����
    TimeSpan(int dwDay, int dwHour, int dwMinute, int dwSecond,
             int dwMilliSecond, int dwMicroSecond);

    static const TimeSpan Zero;
    static const TimeSpan MaxValue;
    static const TimeSpan MinValue;

    int GetDays()const;
    int GetHours()const;
    int GetMinutes()const;
    int GetSeconds()const;
    int GetMilliSeconds()const;
    int GetMicroSeconds()const;

    double GetTotalDays()const;
    double GetTotalHours()const;
    double GetTotalMinutes()const;
    double GetTotalSeconds()const;
    double GetTotalMilliSeconds()const;
    double GetTotalMicroSeconds()const;


    /** ��ʾ��ʵ����ֵ���ַ�����
    ����ֵ��ʽ���£�
    [-][d.]hh:mm:ss[.ff]
    �����ţ���[���͡�]�����е����ǿ�ѡ�ģ�ð�ź;�ţ���:���͡�.������ԭ���ַ������������¡�
    ��      ˵��
    ��-�� ��ѡ�ĸ��ţ�ָʾ��ʱ��
    ��d��     ��ѡ������
    ��hh��    Сʱ����Χ�� 0 �� 23
    ��mm��    ���ӣ���Χ�� 0 �� 59
    ��ss��    �룬��Χ�� 0 �� 59
    ��ff��    ��ѡ�����С�����֣��� 1 �� 7 ��С��λ      */
    std::string ToString()const;
    /** ���ַ�����ָ����ʱ���������µ�TimeSpan�������ת��ʧ�ܷ���TimeSpan::Zero����
    strTime ��������һ��������ʽ��ʱ�����淶��
    [ws][-]{ d | [d.]hh:mm[:ss[.ff]] }[ws]
    �����ţ���[���͡�]�����е����ǿ�ѡ�ģ���Ҫ�Ӵ����ţ���{���͡�}������������ (|) �ָ����滻ѡ���б���ѡ��һ�ð�ź;�ţ���:���͡�.�����Ǳ����ԭ���ַ������������¡�
    ��      ˵��
    ws      ��ѡ�Ŀհ�
    ��-��     ��ѡ�ļ��ţ�ָʾ�� TimeSpan
    d       �죬��Χ�� 0 �� 10675199
    hh      Сʱ����Χ�� 0 �� 23
    mm      ���ӣ���Χ�� 0 �� 59
    ss      ��ѡ���룬��Χ�� 0 �� 59
    ff      ��ѡ�����С�����֣��� 1 �� 7 ��С��λ

    strTime �ķ�����������ָ�����ڻ���� MinValue ��С�ڻ���� MaxValue ��ʱ������        */
    static TimeSpan Parse(const std::string &strTime);
    /// ����ָ��TimeTick��TimeSpan
    static TimeSpan FromTick(const TimeTick &tt);
    /// ����ָ�����TimeSpan
    static TimeSpan FromDays(double dwDay);
    /// ����ָ��Сʱ��TimeSpan
    static TimeSpan FromHours(double dwHour);
    /// ����ָ�����ӵ�TimeSpan
    static TimeSpan FromMinutes(double dwMinute);
    /// ����ָ�����TimeSpan
    static TimeSpan FromSeconds(double dwSecond);
    /// ����ָ�������TimeSpan
    static TimeSpan FromMilliSeconds(double dwMilliSecond);
    /// ����ָ��΢���TimeSpan
    static TimeSpan FromMicroSeconds(double dwMicroSecond);

    TimeSpan Abs() const {return *this > TimeSpan::Zero ? *this : -(*this);}

    TimeSpan operator -()const;
    TimeSpan operator +(const TimeSpan &ts)const;
    TimeSpan &operator +=(const TimeSpan &ts);
    TimeSpan operator -(const TimeSpan &ts)const;
    TimeSpan &operator -=(const TimeSpan &ts);
    bool operator >(const TimeSpan &ts)const;
    bool operator <(const TimeSpan &ts)const;
    bool operator >=(const TimeSpan &ts)const;
    bool operator <=(const TimeSpan &ts)const;
    bool operator ==(const TimeSpan &ts)const;
    bool operator !=(const TimeSpan &ts)const;
};

/// ģʽ����һ��ԭ��Ƭ�ε����ͣ���ϸ�μ��Զ����ʽģʽ
enum FormatSegmentTokenKind
{
    FSTK_String = 0, // �����ַ�������
    FSTK_d,      // 1-31����
    FSTK_dd,     // 01-31����
    FSTK_ddd,    // һ����ĳһ�����д
    FSTK_dddd_,  // һ����ĳһ���ȫ��
    FSTK_f,      // �뾫ȷ��һλС���������Ժ�׺��
    FSTK_ff,     // �뾫ȷ����λС���������Ժ�׺��
    FSTK_fff,    // �뾫ȷ����λС���������Ժ�׺��
    FSTK_ffff,   // �뾫ȷ����λС���������Ժ�׺��
    FSTK_fffff,  // �뾫ȷ����λС���������Ժ�׺��
    FSTK_ffffff, // �뾫ȷ����λС���������Ժ�׺��
    FSTK_F,      // �뾫ȷ��һλС�������Ժ�׺��
    FSTK_FF,     // �뾫ȷ����λС�������Ժ�׺��
    FSTK_FFF,    // �뾫ȷ����λС�������Ժ�׺��
    FSTK_FFFF,   // �뾫ȷ����λС�������Ժ�׺��
    FSTK_FFFFF,  // �뾫ȷ����λС�������Ժ�׺��
    FSTK_FFFFFF, // �뾫ȷ����λС�������Ժ�׺��
    FSTK_g_,     // ��Ԫ��A.D.
    FSTK_h,      // 1-12
    FSTK_hh_,    // 01-12
    FSTK_H,      // 0-23
    FSTK_HH_,    // 00-23
    FSTK_K,      // ʱ����localtime�൱��zzz
    FSTK_m,      // 0-59
    FSTK_mm_,    // 00-59
    FSTK_M,      // 1-12
    FSTK_MM,     // 01-12
    FSTK_MMM,    // �·ݼ�д
    FSTK_MMMM,   // �·�ȫ��
    FSTK_s,      // 0-59
    FSTK_ss_,    // 00-59
    FSTK_t,      // AM��PMDesigner�ĵ�һ���ַ�
    FSTK_tt_,    // AM��PMDesigner��ȫ��
    FSTK_y,      // һλ������λ��
    FSTK_yy,     // ��λ��
    FSTK_yyy,    // ��λ��
    FSTK_yyyy,   // ��λ��
    FSTK_yyyyy_, // ��λ��
    FSTK_z,      // +8
    FSTK_zz,     // +08
    FSTK_zzz,    // +08:00
    FSTK_TIMESEP_CHAR,  // ʱ��ָ�����һ����:
    FSTK_DATESEP_CHAR,  // ���ڷָ�����һ����/
};

struct DateTimeInfo
{
    int year;
    int month;
    int day_of_month;
    int hour;
    int minute;
    int second;
    int microseconds;
    int timezone_seconds;

    bool is_hour12;         // ��ʾ�Ƿ���12Сʱ��
    bool is_set_am_or_pm;   // ��ʾ�Ƿ�����am����pm
    bool am_or_pm;          // ��is_set_am_or_pm��Чʱ������am����pm

    DateTimeInfo()
    {
        year = -1;
        month = -1;
        day_of_month = -1;
        hour = -1;
        minute = -1;
        second = -1;
        microseconds = -1;
        timezone_seconds = -1;
        is_hour12 = false;
        is_set_am_or_pm = false;
        am_or_pm = false;
    }
};

/// DateTime��ʽ��Ƭ��
struct FormatSegment
{
    std::string strSeg;
    FormatSegmentTokenKind dwKind;
};

/// DateTimeFormatInfo
class DateTimeFormatInfo
{
    // ��׼ģʽ
    std::string m_ShortDatePattern;
    std::string m_LongDatePattern;
    std::string m_FullDateTimePattern;
    std::string m_MonthDayPattern;
    std::string m_RFC1123Pattern;
    std::string m_SortableDateTimePattern;
    std::string m_ShortTimePattern;
    std::string m_LongTimePattern;
    std::string m_UniversalSortableDateTimePattern;
    std::string m_YearMonthPattern;
    std::string m_RetureBackPattern;

    std::vector<std::string> m_AbbreviatedDayNames;
    std::vector<std::string> m_DayNames;
    std::vector<std::string> m_AbbreviatedMonthNames;
    std::vector<std::string> m_MonthNames;

    std::vector<std::string> m_EraNames;

    std::vector<std::string> m_AllPatterns;

    std::string m_AMDesignator;
    std::string m_PMDesignator;
    std::string m_TimeSeparator;
    std::string m_DateSeparator;

    std::string m_Name;
    // ����pattern
    std::vector<std::string> m_vecGeneralPattern;
    // zh-CN,en-US��
    explicit DateTimeFormatInfo(const std::string &cultureStr = "zh-CN");
public:
    const std::string &ShortDatePattern()const {
        return m_ShortDatePattern;
    }
    const std::string &RetureBackPattern()const {
        return m_RetureBackPattern;
    }
    const std::string &LongDatePattern()const {
        return m_LongDatePattern;
    }
    const std::string &FullDateTimePattern()const {
        return m_FullDateTimePattern;
    }
    const std::string &MonthDayPattern()const {
        return m_MonthDayPattern;
    }
    const std::string &RFC1123Pattern()const {
        return m_RFC1123Pattern;
    }
    const std::string &SortableDateTimePattern()const {
        return m_SortableDateTimePattern;
    }
    const std::string &ShortTimePattern()const {
        return m_ShortTimePattern;
    }
    const std::string &LongTimePattern()const {
        return m_LongTimePattern;
    }
    const std::string &UniversalSortableDateTimePattern()const {
        return m_UniversalSortableDateTimePattern;
    }
    const std::string &YearMonthPattern()const {
        return m_YearMonthPattern;
    }

    /// ��ȡ����ģʽ����
    const std::vector<std::string> &GeneralPatterns()const {
        return m_vecGeneralPattern;
    }
    /// ����ĳ����д
    const std::vector<std::string> &AbbreviatedDayNames()const {
        return m_AbbreviatedDayNames;
    }
    /// ����ĳ��ȫ��
    const std::vector<std::string> &DayNames()const {
        return m_DayNames;
    }
    /// �·ݵ���д
    const std::vector<std::string> &AbbreviatedMonthNames()const {
        return m_AbbreviatedMonthNames;
    }
    /// �·ݵ�ȫ��
    const std::vector<std::string> &MonthNames()const {
        return m_MonthNames;
    }
    /// ��Ԫ�����磺A.D.�� ��Ԫ��
    const std::vector<std::string> &EraNames()const {
        return m_EraNames;
    }
    /// AM������
    const std::string &AMDesignator()const {
        return m_AMDesignator;
    }
    /// PM������
    const std::string &PMDesignator()const {
        return m_PMDesignator;
    }
    /// ʱ����֮��ķָ�����Ĭ��Ϊ':'
    const std::string &TimeSeparator()const {
        return m_TimeSeparator;
    }
    /// ������֮��ķָ�����Ĭ��Ϊ'/'
    const std::string &DateSeparator()const {
        return m_DateSeparator;
    }

    /// datetimeformatinfo�����֣�zh-CN,en-US��
    const std::string &Name()const {
        return m_Name;
    }

    /// ��ȡ���е�ʱ��ģʽ
    const std::vector<std::string> &GetAllPatterns()const;

    /// en-US��DateTimeFormatInfo
    static const DateTimeFormatInfo en_USInfo;
    /// zh-CN��DateTimeFormatInfo
    static const DateTimeFormatInfo zh_CNInfo;

private:
    void _Creat_zh_CN();
    void _Creat_en_US();
};


/// ��ʾ����ʱ�����
class DateTime
{
    // ��1970��1��1����ҹ(00:00:00)���ۼƵ�ʱ��
    TimeTick m_Time;
    // UTCʱ��Ĳ�ֵ,�룬local = utc + dw
    int m_dwTimezoneSeconds;
public:
    friend class TimeCounter;
    explicit DateTime(int dwTimezoneSeconds = -1);
    explicit DateTime(const TimeTick &tmTick, int dwTimezoneSeconds = -1);
    explicit DateTime(const time_t &tm, int usec = 0,  int dwTimezoneSeconds = -1);
    DateTime(int dwYear, int dwMonth , int dwDay ,
             int dwHour = 0, int dwMinute = 0, int dwSecond = 0,
             int dwMilliSecond = 0, int dwMicroSecond = 0,
             int dwTimezoneSeconds = -1);

    time_t GetSecondsSinceEpoch() const;

    /// ��ȡ��
    int GetYear()const;
    /// ��ȡ�·�
    int GetMonth()const;
    /// ��ȡһ���µĵڼ��죬1-31
    int GetDayOfMonth()const;
    /// ��ȡһ��ĵڼ���
    int GetDayOfYear()const;
    /// ��ȡһ�ܵ�ĳ�죬0-6
    int GetDayOfWeek()const;
    /// ��ȡСʱ��24Сʱ��
    int GetHour()const;
    /// ��ȡСʱ��12Сʱ��
    int GetHour12()const;
    /// ��ȡ���Ӳ���
    int GetMinute()const;
    /// ��ȡ�벿��
    int GetSecond()const;
    /// ��ȡ���벿��
    int GetMilliSecond()const;
    /// ��ȡ΢�벿��
    int GetMicroSecond()const;
    /// ��ȡʱ��������ʱ����+8
    int GetTimeZone()const;
    /// ��ȡʱ��������UTCʱ������������ʾ
    int GetTimeZoneSeconds()const;
    /// ��ȡ����
    DateTime GetDay()const;
    /// ��ȡʱ��
    TimeSpan GetTime()const;
    /// ��ȡUTCʱ��
    DateTime GetUTCDateTime()const;
    /// ������
    void SetYear(int dwYear);
    /// �����·�
    void SetMonth(int dwMonth);
    /// ����һ���µĵڼ��죬1-31
    void SetDayOfMonth(int dwDay);
    /// ����Сʱ��24Сʱ��
    void SetHour(int dwHour);
    /// ����Сʱ��12Сʱ��
    void SetHour12(int dwHour12, bool bAM);
    /// ���÷��Ӳ���
    void SetMinute(int dwMinute);
    /// �����벿��
    void SetSecond(int dwSecond);
    /// ���ú��벿��
    void SetMilliSecond(int dwMilliSecond);
    /// ����΢�벿��
    void SetMicroSecond(int dwMicroSecond);
    /// ����ʱ��������ʱ����+8
    void SetTimeZone(int dwTimeZone);
    /// ����ʱ��������UTCʱ������������ʾ
    void SetTimeZoneSeconds(int dwTimezoneSeconds);
    /// ��������
    void SetDay(const DateTime &dt);
    /// ����ʱ��
    void SetTime(const TimeSpan &ts);
    /// ����AM����PM
    void SetAMOrPM(bool bAM);
    /// �Ƿ���AM
    bool IsAM()const;
    /// �Ƿ���PM
    bool IsPM()const;
    /// �Ƿ�������
    bool IsLeapYear()const;
    /// �Ƿ�������
    static bool IsLeapYear(int dwYear);
    /// �Ƿ���һ����Ч��������
    static bool IsValidDate(int dwYear, int dwMonth , int dwDay);

    /// ��ʱ��ת��Ϊʱ�䴮,Ĭ��ʹ��ģʽ����׼ģʽ"F"���Զ���ģʽ"FFFFFF"����ϣ��м���'.'������zh-CN
    std::string ToString(const DateTimeFormatInfo &formatInfo = DateTimeFormatInfo::zh_CNInfo)const;

    /// ��ʱ��ת��Ϊ�ַ���������ָ���ĸ�ʽ
    /// ��ʽ������μ�MSDN�ĵ� ms-help://MS.MSDNQTR.v90.chs/dv_fxfund/html/bb79761a-ca08-44ee-b142-b06b3e2fc22b.htm
    /// ��ms-help://MS.MSDNQTR.v90.chs/dv_fxfund/html/98b374e3-0cc2-4c78-ab44-efb671d71984.htm
    std::string ToString(
        const std::string &strFormat,
        const DateTimeFormatInfo &formatInfo = DateTimeFormatInfo::zh_CNInfo)const;

    /// ��ʱ�䴮��ȡʱ��,����ָ���ĸ�ʽ������ʧ�ܻ᷵�ص�ǰʱ�䣬
    /// Ĭ��Ϊ����ʱ�䣬ȱʧdate�õ�ǰ���ڴ���
    /// ��ʽ������μ�MSDN�ĵ� ms-help://MS.MSDNQTR.v90.chs/dv_fxfund/html/bb79761a-ca08-44ee-b142-b06b3e2fc22b.htm
    /// ��ms-help://MS.MSDNQTR.v90.chs/dv_fxfund/html/98b374e3-0cc2-4c78-ab44-efb671d71984.htm
    static DateTime Parse(
        const std::string &strTime,
        const std::string &strFormat,
        const DateTimeFormatInfo &formatInfo = DateTimeFormatInfo::zh_CNInfo);

    /// ��ʱ�䴮��ȡʱ��,����ʧ�ܻ᷵�ص�ǰʱ��,�����ܵİ��ַ���ת��ΪDateTime,
    /// ȱʧ���������õ�ǰ�����մ��棬û��time��12:00:00.000000����,Ĭ��Ϊ����ʱ��
    /// �ú���û���Ż����ܣ����������ʹ��ָ��strFormat�ĺ����汾
    static DateTime Parse(
        const std::string &strTime,
        const DateTimeFormatInfo &formatInfo = DateTimeFormatInfo::zh_CNInfo);

    /// ��ʱ�䴮��ȡʱ��,����ָ���ĸ�ʽ������ʧ�ܻ᷵�ص�ǰʱ�䡣
    /// Ĭ��Ϊ����ʱ�䣬ȱʧdate�õ�ǰ���ڴ���
    /// ��ʽ������μ�MSDN�ĵ� ms-help://MS.MSDNQTR.v90.chs/dv_fxfund/html/bb79761a-ca08-44ee-b142-b06b3e2fc22b.htm
    /// ��ms-help://MS.MSDNQTR.v90.chs/dv_fxfund/html/98b374e3-0cc2-4c78-ab44-efb671d71984.htm
    static bool TryParse(
        const std::string &strTime,
        const std::string &strFormat,
        DateTime & dt,
        const DateTimeFormatInfo &formatInfo = DateTimeFormatInfo::zh_CNInfo);

    /// ��ʱ�䴮��ȡʱ��,����ʧ�ܻ᷵�ص�ǰʱ��,�����ܵİ��ַ���ת��ΪDateTime,
    /// ȱʧ���������õ�ǰ�����մ��棬û��time��12:00:00.000000����,Ĭ��Ϊ����ʱ��
    static bool TryParse(
        const std::string &strTime,
        DateTime& dt,
        const DateTimeFormatInfo &formatInfo = DateTimeFormatInfo::zh_CNInfo);


    /// ���ص�ǰ���ں�ʱ��
    static DateTime Now();
    /// UTC��ǰʱ��
    static DateTime UTCNow();
    /// ���ص�ǰ����
    static DateTime Today();
    /// ���ص�ǰʱ��
    static TimeSpan Time();

    TimeSpan operator-(const DateTime &dt) const;
    DateTime operator-(const TimeSpan &ts) const;
    DateTime operator+(const TimeSpan &ts) const;

    DateTime &operator-=(const TimeSpan &ts);
    DateTime &operator+=(const TimeSpan &ts);

    bool operator >(const DateTime &ts)const;
    bool operator <(const DateTime &ts)const;
    bool operator >=(const DateTime &ts)const;
    bool operator <=(const DateTime &ts)const;
    bool operator ==(const DateTime &ts)const;
    bool operator !=(const DateTime &ts)const;

private:

    /// չ��Ԥ����ģʽΪ�Զ���ģʽ��
    std::string ExpandPredefinedFormat(
        const std::string &strFormat,
        const DateTimeFormatInfo &formatInfo,
        DateTime& dt) const;

    /// ���ݸ�ʽ��Ƭ�λ�ȡģʽ���
    static bool ParseFormatSegment(FormatSegment* seg);

    /// ��ʽ���ָ��һϵ��Ƭ��
    static bool SegmentFormatString(const std::string &strFormat,
                                    std::vector<FormatSegment> &vecSegment);

    /// ��dt�����Զ���ģʽ��ת��Ϊ�ַ���
    static std::string ToFormatString(
        const DateTime &dt,
        const std::string &strFormat,
        const DateTimeFormatInfo& formatInfo);

    /// ��dt�����Զ���ģʽ��Ƭ������ת��Ϊ�ַ���
    static std::string ToFormatString(
        const DateTime &dt,
        const std::vector<FormatSegment> &vecSegment,
        const DateTimeFormatInfo& formatInfo);

    /// ƥ��һ��segment�����سɹ����
    static bool TryParseSegment(
        const std::string &strTime,
        size_t &dwIndex, const std::vector<FormatSegment> &vecSegment,
        const size_t dwSegIndex,
        DateTimeInfo* dateInfo,
        const DateTimeFormatInfo &formatInfo);

    /// ����DateTimeInfo����������dt�ĸ�����
    static void MakeDateTime(const DateTimeInfo& dateInfo, DateTime* dt);

    /// �õ���ǰʱ����UTCʱ��Ĳ�ֵ��������
    static int GetCurrentTimezoneSeconds();
};

/// ��ʱ����
class TimeCounter
{
    TimeSpan m_TimeSpan;
    TimeTick m_LastTick;
public:
    TimeCounter();
    explicit TimeCounter(const TimeSpan &ts);

    /// ���¼�ʱ
    void Reset();
    /// ��ʼ��ʱ
    void Start();
    /// ��ͣ��ʱ
    void Pause();
    /// ��ȡ��ʱ���ۼ�ʱ��
    TimeSpan GetTimeSpan()const;
    /// ����ʱ��ʱ�䣬ת��Ϊ�ַ���
    std::string ToString()const;
private:
    TimeTick GetCurTick()const;
};

} // end of namespace common

#endif // COMMON_SYSTEM_TIME_DATETIME_H

