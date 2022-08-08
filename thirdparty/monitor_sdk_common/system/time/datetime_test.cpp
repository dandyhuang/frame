//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 19:11
//  @file:      datetime_test.cpp
//  @brief:     
//
//********************************************************************

#include "thirdparty/monitor_sdk_common/system/time/datetime.h"

#include <math.h>
#include <locale.h>
#include <iostream>
#include <string>

#include "thirdparty/glog/logging.h"
#include "thirdparty/gtest/gtest.h"

#include "thirdparty/monitor_sdk_common/base/string/algorithm.h"

using namespace std;

namespace common {

const char* g_init_locale = setlocale(LC_ALL, "zh_CN.GBK");

TEST(TimeTick, TestTimeTickNormalize)
{
    const int kMicrosecondsPerSecond = 1000000;
    const int kTimeTickTestSec = 1000;
    const int kTimeTickTestUSec = 100;

    TimeTick time_tick1(kTimeTickTestSec, kMicrosecondsPerSecond + kTimeTickTestUSec);
    ASSERT_TRUE(TimeTick(kTimeTickTestSec + 1, kTimeTickTestUSec) == time_tick1);

    TimeTick time_tick2(kTimeTickTestSec, -kTimeTickTestUSec);
    ASSERT_EQ(time_tick2, TimeTick(kTimeTickTestSec - 1,
                                   kMicrosecondsPerSecond - kTimeTickTestUSec));

    TimeTick time_tick3(-kTimeTickTestSec, kTimeTickTestUSec);
    ASSERT_EQ(time_tick3, TimeTick(-kTimeTickTestSec + 1,
                                   kTimeTickTestUSec - kMicrosecondsPerSecond));
    time_tick3.Normalize();

    TimeTick time_tick4(-kTimeTickTestSec, -kTimeTickTestUSec - kMicrosecondsPerSecond);
    ASSERT_EQ(time_tick4, TimeTick(-kTimeTickTestSec - 1, -kTimeTickTestUSec));
}

TEST(TimeTick, TestTimeTickOperator)
{
    TimeTick lhs(1000, 100);
    TimeTick rhs(100, 10);

    ASSERT_TRUE(lhs > rhs);
    ASSERT_FALSE(lhs < rhs);
    ASSERT_TRUE(lhs >= rhs);
    ASSERT_FALSE(lhs <= rhs);
    ASSERT_FALSE(lhs == rhs);
    ASSERT_TRUE(lhs != rhs);

    TimeTick result = lhs + rhs;
    ASSERT_TRUE(TimeTick(1100, 110) == result);
    result = lhs - rhs;
    ASSERT_TRUE(TimeTick(900, 90) == result);
    lhs += rhs;
    ASSERT_TRUE(TimeTick(1100, 110) == lhs);
    lhs -= rhs;
    ASSERT_TRUE(TimeTick(1000, 100) == lhs);

    result = -lhs;
    ASSERT_TRUE(TimeTick(-lhs) == result);
}

TEST(TimeSpan, TestTimeSpanConstructor)
{
    TimeTick time_tick(3600, 0);
    ASSERT_EQ("00:00:00", TimeSpan().ToString());
    ASSERT_EQ("01:00:00", TimeSpan(time_tick).ToString());
    ASSERT_EQ("01:02:03", TimeSpan(1, 2, 3).ToString());
    ASSERT_EQ("4.01:02:03", TimeSpan(4, 1, 2, 3).ToString());
    ASSERT_EQ("4.01:02:03.010000", TimeSpan(4, 1, 2, 3, 10).ToString());
    ASSERT_EQ("4.01:02:03.010050", TimeSpan(4, 1, 2, 3, 10, 50).ToString());
    ASSERT_EQ(TimeSpan::FromTick(time_tick).ToString(), "01:00:00");
    ASSERT_EQ(TimeSpan::FromDays(1).ToString() , "1.00:00:00");
    ASSERT_EQ(TimeSpan::FromHours(2).ToString() , "02:00:00");
    ASSERT_EQ(TimeSpan::FromMinutes(3).ToString() , "00:03:00");
    ASSERT_EQ(TimeSpan::FromSeconds(4).ToString() , "00:00:04");
    ASSERT_EQ(TimeSpan::FromMilliSeconds(5).ToString() , "00:00:00.005000");
    ASSERT_EQ(TimeSpan::FromMicroSeconds(6).ToString() , "00:00:00.000006");
}

TEST(TimeSpan, TestTimeSpanAttributes)
{
    TimeSpan ts(1, 2, 3, 4, 5, 6);
    ASSERT_EQ(ts.GetDays(), 1);
    ASSERT_EQ(ts.GetHours(), 2);
    ASSERT_EQ(ts.GetMinutes(), 3);
    ASSERT_EQ(ts.GetSeconds(), 4);
    ASSERT_EQ(ts.GetMilliSeconds(), 5);
    ASSERT_EQ(ts.GetMicroSeconds(), 6);

    ASSERT_TRUE(fabs(ts.GetTotalDays() - 1.085463) < 0.000001);
    ASSERT_TRUE(fabs(ts.GetTotalHours() - 26.0511125017) < 0.000001);
    ASSERT_TRUE(fabs(ts.GetTotalMinutes() - 1563.0667501) < 0.000001);
    EXPECT_DOUBLE_EQ(ts.GetTotalSeconds(), 93784.005006);
    EXPECT_DOUBLE_EQ(ts.GetTotalMilliSeconds(), 93784005.006);
    EXPECT_DOUBLE_EQ(ts.GetTotalMicroSeconds(), 93784005006.0);
}

TEST(TimeSpan, TestTimeSpanOperator)
{
    TimeSpan ts1(1, 2, 3, 4, 5, 6);
    TimeSpan ts2(7, 8, 9, 10, 11, 12);

    ASSERT_TRUE(ts1 < ts2);
    ASSERT_FALSE(ts1 > ts2);
    ASSERT_TRUE(ts1 <= ts2);
    ASSERT_FALSE(ts1 >= ts2);
    ASSERT_FALSE(ts1 == ts2);
    ASSERT_TRUE(ts1 != ts2);

    ASSERT_TRUE(ts2 - ts1 == TimeSpan(6, 6, 6, 6, 6, 6));
    ASSERT_TRUE(ts2 + ts1 == TimeSpan(8, 10, 12, 14, 16, 18));
    TimeSpan ts3(1, 2, 3, 4, 5, 6);
    ts3 += TimeSpan(6, 6, 6, 6, 6, 6);
    ASSERT_TRUE(ts3 == ts2);
    ts3 -= ts2;
    ASSERT_TRUE(ts3 == TimeSpan::Zero);
    ASSERT_TRUE(-ts1 == TimeSpan(-1, -2, -3, -4, -5, -6));
    ASSERT_TRUE((-ts1).Abs() == TimeSpan(1, 2, 3, 4, 5, 6));
}

TEST(TimeSpan, TestTimeSpanParse)
{
    ASSERT_EQ("4.01:02:03.010050", TimeSpan::Parse(" 4.01:02:03.010050 ").ToString());
    ASSERT_EQ("-4.01:02:03.010050", TimeSpan::Parse(" -4.01:02:03.010050 ").ToString());
    ASSERT_EQ("4.00:00:00", TimeSpan::Parse("4").ToString());
    ASSERT_EQ("01:02:03", TimeSpan::Parse("01:02:03").ToString());
    ASSERT_EQ("11:22:33.010050", TimeSpan::Parse("11:22:33.010050").ToString());
}

TEST(DateTime, TestDateTimeConstructor)
{
    ASSERT_EQ("2010年5月3日 0:00:00 +08:00", DateTime(2010, 5, 3, 0, 0, 0, 0, 0, 28800).ToString());
    ASSERT_EQ("2010年5月3日 19:14:10.10001 +08:00",
              DateTime(2010, 5, 3, 19, 14, 10, 100, 10, 28800).ToString());
}

TEST(DateTime, TestDateTimeEpoch)
{
    EXPECT_EQ(-DateTime::Now().GetTimeZoneSeconds(), DateTime(1970, 1, 1).GetSecondsSinceEpoch());
    time_t t = time(NULL);
    EXPECT_EQ(t, DateTime(t).GetSecondsSinceEpoch());
}

TEST(DateTime, TestDateTimeAttributes)
{
    DateTime dt(2012, 5, 25, 18, 59, 59, 100, 888, 28800);
    ASSERT_EQ(2012, dt.GetYear());
    ASSERT_EQ(5, dt.GetMonth());
    ASSERT_EQ(25, dt.GetDayOfMonth());
    ASSERT_EQ(145, dt.GetDayOfYear());
    ASSERT_EQ(5, dt.GetDayOfWeek());
    ASSERT_EQ(18, dt.GetHour());
    ASSERT_EQ(6, dt.GetHour12());
    ASSERT_EQ(59, dt.GetMinute());
    ASSERT_EQ(59, dt.GetSecond());
    ASSERT_EQ(100, dt.GetMilliSecond());
    ASSERT_EQ(888, dt.GetMicroSecond());
    ASSERT_EQ(8, dt.GetTimeZone());
    ASSERT_EQ(28800, dt.GetTimeZoneSeconds());

    ASSERT_FALSE(dt.IsAM());
    ASSERT_TRUE(dt.IsPM());
    dt.SetAMOrPM(true);
    ASSERT_TRUE(dt.IsAM());
    dt.SetAMOrPM(true);
    ASSERT_TRUE(dt.IsAM());
    dt.SetAMOrPM(false);
    ASSERT_TRUE(dt.IsPM());

    ASSERT_TRUE(dt.IsLeapYear());
    dt.SetYear(2013);
    ASSERT_FALSE(dt.IsLeapYear());
    dt.SetYear(1900);
    ASSERT_FALSE(dt.IsLeapYear());
    dt.SetYear(2000);
    ASSERT_TRUE(dt.IsLeapYear());

    dt.SetHour12(8, true);
    ASSERT_EQ(8, dt.GetHour());
    dt.SetHour12(20, false);
    dt.SetTimeZone(9);
    ASSERT_EQ(9, dt.GetTimeZone());
    dt.SetTimeZoneSeconds(28800);
    ASSERT_EQ(8, dt.GetTimeZone());

    dt.SetHour(0);
    ASSERT_EQ(12, dt.GetHour12());

    DateTime dt2(2012, 5, 28);
    dt.SetDay(dt2);
    ASSERT_EQ(28, dt2.GetDayOfMonth());
}

TEST(DateTime, TestTodayAndNow)
{
    DateTime now;
    DateTime utc_now;
    DateTime today;
    TimeSpan time;
    time_t t0;
    do { // Make sure in same second.
        t0 = ::time(NULL);
        now = DateTime::Now();
        utc_now = DateTime::UTCNow();
        today = DateTime::Today();
        time = DateTime::Time();
    } while (::time(NULL) != t0);
    // EXPECT_NE(now, utc_now);
    EXPECT_EQ(now.ToString("yyyyMMdd"), today.ToString("yyyyMMdd"));
    EXPECT_EQ("000000", today.ToString("HHmmss"));
    EXPECT_PRED2(StringStartsWith, time.ToString(), now.ToString("HH:mm:ss"));
}

TEST(DateTime, TestDateTimeParse)
{
    DateTime dt = DateTime::Parse("2010年5月3日 15:59:10 +08:00");
    ASSERT_TRUE(dt == DateTime(2010, 5, 3, 15, 59, 10));
    dt = DateTime::Parse("2010-05-03 15:59");
    ASSERT_TRUE(dt == DateTime(2010, 5, 3, 15, 59));

    dt = DateTime::Parse("2010-11-03 15:59:10");
    ASSERT_TRUE(dt == DateTime(2010, 11, 3, 15, 59, 10));

    dt = DateTime::Parse("2010-05-03");
    ASSERT_TRUE(dt.GetDay() == DateTime(2010, 5, 3).GetDay());
    dt = DateTime::Parse("2010年5月");
    ASSERT_EQ(2010, dt.GetYear());
    ASSERT_EQ(5, dt.GetMonth());
    ASSERT_TRUE(DateTime::Parse("15:59:10").GetTime() ==
                DateTime(2010, 5, 1, 15, 59, 10).GetTime());

    dt = DateTime::Parse("2010年5月3日 15:59:10");
    ASSERT_TRUE(dt == DateTime(2010, 5, 3, 15, 59, 10));
    std::string s = "20100503 15:59:10.010019 +08:00";
    dt = DateTime::Parse(s, "yyyyMMdd HH:mm:ss.FFFFFF zzz");
    ASSERT_EQ("2010-5-3 15:59:10", dt.ToString("G"));

    dt = DateTime::Parse("2012-06-02T16:48:46.009674+08");
    ASSERT_EQ(9, dt.GetMilliSecond());
    ASSERT_EQ(674, dt.GetMicroSecond());
    // test error format.
    dt = DateTime::Parse("2010:05:03 15:59:10");
    dt = DateTime::Parse("2010:05:03 15:59:10", "yyyyMMdd HHmmss.FFFFFF zzz");
}

TEST(DateTime, TestTryParse)
{
    DateTime dt1;
    DateTime::TryParse("2012-03-14 09:59:58", dt1);
    ASSERT_EQ(dt1.ToString("G"), "2012-3-14 9:59:58");
    DateTime::TryParse("2012-03-14 10:00:58", dt1);
    ASSERT_EQ(dt1.ToString("G"), "2012-3-14 10:00:58");

    ASSERT_TRUE(DateTime::TryParse("2012年5月25日", "yyyy'年'M'月'd'日'", dt1));
    ASSERT_FALSE(DateTime::TryParse("2012年5月25日", "yyyy年'M'月'd'日'", dt1));
    ASSERT_TRUE(DateTime::TryParse("2012年5月25日", "yyyy\"年\"M\"月\"d\"日\"", dt1));
    ASSERT_FALSE(DateTime::TryParse("2012年5月25日", "yyyy年\"M月\"d\"日\"", dt1));
    ASSERT_TRUE(DateTime::TryParse("05/25/2012", "M/d/yyyy", dt1, DateTimeFormatInfo::en_USInfo));
    ASSERT_EQ("5/25/2012 10:00:58 AM", dt1.ToString("G", DateTimeFormatInfo::en_USInfo));
    ASSERT_FALSE(DateTime::TryParse("5/25\\2012", "M/d\\yyyy", dt1));
    ASSERT_TRUE(DateTime::TryParse("5/25/2012 10:00:58 AM", "M/d/yyyy HH:mm:ss t",
                                   dt1, DateTimeFormatInfo::en_USInfo));
    ASSERT_TRUE(DateTime::TryParse("5/25/2012 10:00:58 AM", "M/d/yyyy HH:mm:ss tt",
                                   dt1, DateTimeFormatInfo::en_USInfo));
    ASSERT_TRUE(DateTime::TryParse("5/25/2012 10:00:58 AM", "M/d/yyyy HH:mm:ss ttt",
                                   dt1, DateTimeFormatInfo::en_USInfo));
    ASSERT_TRUE(DateTime::TryParse("2012.3.25 10:00:58", "yyyyy.M.d hh:mm:ss", dt1));
    ASSERT_TRUE(DateTime::TryParse("2012.3.25 10:00:58", "yyyy.M.d hh:mm:ss", dt1));
    ASSERT_TRUE(DateTime::TryParse("12.3.25 10:00:58", "yyy.M.d hh:mm:ss", dt1));
    ASSERT_TRUE(DateTime::TryParse("12.3.25 10:00:58", "yy.M.d hh:mm:ss", dt1));
    ASSERT_TRUE(DateTime::TryParse("2012.03.25 10:00:58", "yyyy.MM.d hh:mm:ss", dt1));
    // different timezone.
    ASSERT_TRUE(DateTime::TryParse("2010-05-03T15:59:10.010019+10:00", dt1));
    ASSERT_EQ("2010-05-03T15:59:10.010019+10", dt1.ToString("o"));
    EXPECT_TRUE(DateTime::TryParse("20100503 15:59:10.010019 +10:00",
                                "yyyyMMdd HH:mm:ss.FFFFFF zzz", dt1));
    EXPECT_EQ("2010-05-03T15:59:10.010019+10", dt1.ToString("o"));
    EXPECT_TRUE(DateTime::TryParse("2010-05-03 15:59:10.010019 +10:00",
                                "yyyy-MM-dd HH:mm:ss.FFFFFF zzz", dt1));
    EXPECT_EQ("2010-05-03T15:59:10.010019+10", dt1.ToString("o"));

}

TEST(DateTime, TestDateTimeOperator)
{
    DateTime dt1(2010, 5, 3, 21, 3, 10, 100, 10);
    DateTime dt2(2010, 5, 3, 21, 4, 25, 400, 51);

    ASSERT_TRUE(dt1 < dt2);
    ASSERT_FALSE(dt1 > dt2);
    ASSERT_TRUE(dt1 <= dt2);
    ASSERT_FALSE(dt1 >= dt2);
    ASSERT_FALSE(dt1 == dt2);
    ASSERT_TRUE(dt1 != dt2);

    TimeSpan ts(0, 0, 1, 15, 300, 41);
    ASSERT_TRUE(dt2 - dt1 == ts);
    ASSERT_TRUE(dt1 + ts == dt2);
    ASSERT_TRUE(dt2 - ts == dt1);

    DateTime dt3 = dt2;
    dt3 -= ts;
    ASSERT_TRUE(dt1 == dt3);
    dt3 += ts;
    ASSERT_TRUE(dt2 == dt3);
}

#define ASSERT_EQ_REVERSE(x, y) ASSERT_EQ(y, x)

TEST(DateTime, TestDateTimeToString)
{
    DateTime dt(2010, 5, 3, 15, 59, 10, 100, 50, 28800);
    ASSERT_EQ_REVERSE("2010年5月3日 15:59:10.10005 +08:00", dt.ToString());
    ASSERT_EQ_REVERSE("2010-5-3", dt.ToString("d"));
    ASSERT_EQ_REVERSE(dt.ToString("D"), "2010年5月3日");
    ASSERT_EQ_REVERSE(dt.ToString("f"), "2010年5月3日 15:59");
    ASSERT_EQ_REVERSE(dt.ToString("F"), "2010年5月3日 15:59:10");
    ASSERT_EQ_REVERSE(dt.ToString("g"), "2010-5-3 15:59");
    ASSERT_EQ_REVERSE(dt.ToString("G"), "2010-5-3 15:59:10");
    ASSERT_EQ_REVERSE(dt.ToString("m"), "5月3日");
    ASSERT_EQ_REVERSE(dt.ToString("M"), "5月3日");
    ASSERT_EQ_REVERSE(dt.ToString("o"), "2010-05-03T15:59:10.10005+08");
    ASSERT_EQ_REVERSE(dt.ToString("O"), "2010-05-03T15:59:10.10005+08");
    ASSERT_EQ_REVERSE(dt.ToString("r"), "一, 03 五月 2010 15:59:10 GMT");
    ASSERT_EQ_REVERSE(dt.ToString("R"), "一, 03 五月 2010 15:59:10 GMT");
    ASSERT_EQ_REVERSE(dt.ToString("s"), "2010-05-03T15:59:10");
    ASSERT_EQ_REVERSE(dt.ToString("t"), "15:59");
    ASSERT_EQ_REVERSE(dt.ToString("T"), "15:59:10");
    ASSERT_EQ_REVERSE(dt.ToString("u"), "2010-05-03 07:59:10Z");
    ASSERT_EQ_REVERSE(dt.ToString("U"), "2010年5月3日 7:59:10");
    ASSERT_EQ_REVERSE(dt.ToString("y"), "2010年5月");
    ASSERT_EQ_REVERSE(dt.ToString("Y"), "2010年5月");

    ASSERT_EQ_REVERSE(dt.ToString("%d"), "3");
    ASSERT_EQ_REVERSE(dt.ToString("dd"), "03");
    ASSERT_EQ_REVERSE(dt.ToString("ddd"), "一");
    ASSERT_EQ_REVERSE(dt.ToString("dddd"), "星期一");
    ASSERT_EQ_REVERSE(dt.ToString("%f"), "1");
    ASSERT_EQ_REVERSE(dt.ToString("ff"), "10");
    ASSERT_EQ_REVERSE(dt.ToString("fff"), "100");
    ASSERT_EQ_REVERSE(dt.ToString("ffff"), "1000");
    ASSERT_EQ_REVERSE(dt.ToString("fffff"), "10005");
    ASSERT_EQ_REVERSE(dt.ToString("ffffff"), "100050");
    ASSERT_EQ_REVERSE(dt.ToString("%F"), "1");
    ASSERT_EQ_REVERSE(dt.ToString("FF"), "1");
    ASSERT_EQ_REVERSE(dt.ToString("FFF"), "1");
    ASSERT_EQ_REVERSE(dt.ToString("FFFF"), "1");
    ASSERT_EQ_REVERSE(dt.ToString("FFFFF"), "10005");
    ASSERT_EQ_REVERSE(dt.ToString("FFFFFF"), "10005");
    ASSERT_EQ_REVERSE(dt.ToString("gg"), "公元");
    ASSERT_EQ_REVERSE(dt.ToString("%h"), "3");
    ASSERT_EQ_REVERSE(dt.ToString("hh"), "03");
    ASSERT_EQ_REVERSE(dt.ToString("%H"), "15");
    ASSERT_EQ_REVERSE(dt.ToString("HH"), "15");
    ASSERT_EQ_REVERSE(dt.ToString("%m"), "59");
    ASSERT_EQ_REVERSE(dt.ToString("mm"), "59");
    ASSERT_EQ_REVERSE(dt.ToString("%M"), "5");
    ASSERT_EQ_REVERSE(dt.ToString("MM"), "05");
    ASSERT_EQ_REVERSE(dt.ToString("MMM"), "五月");
    ASSERT_EQ_REVERSE(dt.ToString("MMMM"), "五月");
    ASSERT_EQ_REVERSE(dt.ToString("%s"), "10");
    ASSERT_EQ_REVERSE(dt.ToString("ss"), "10");
    ASSERT_EQ_REVERSE(dt.ToString("%t"), "下");
    ASSERT_EQ_REVERSE(dt.ToString("tt"), "下午");
    ASSERT_EQ_REVERSE(dt.ToString("%y"), "10");
    ASSERT_EQ_REVERSE(dt.ToString("yy"), "10");
    ASSERT_EQ_REVERSE(dt.ToString("yyy"), "2010");
    ASSERT_EQ_REVERSE(dt.ToString("yyyy"), "2010");
    ASSERT_EQ_REVERSE(dt.ToString("yyyyy"), "02010");
    ASSERT_EQ_REVERSE(dt.ToString("yyyyyy"), "02010");
    ASSERT_EQ_REVERSE(dt.ToString("%z"), "+8");
    ASSERT_EQ_REVERSE(dt.ToString("zz"), "+08");
    ASSERT_EQ_REVERSE(dt.ToString("zzz"), "+08:00");
    ASSERT_EQ_REVERSE(dt.ToString("yyyyMM"), "201005");

    DateTime dt1;
    ASSERT_TRUE(DateTime::TryParse("2012.3.25", "yyyy.M.d", dt1));
    ASSERT_EQ("2012.3.25", dt1.ToString("yyyy.M.d"));
    ASSERT_EQ("Sunday", dt1.ToString("dddd", DateTimeFormatInfo::en_USInfo));
    ASSERT_EQ("A.D.2012.3.25", dt1.ToString("gyyyy.M.d", DateTimeFormatInfo::en_USInfo));
    ASSERT_EQ("", dt1.ToString(""));
}

TEST(DateTime, IsValidDate)
{
    ASSERT_TRUE(DateTime::IsValidDate(2000, 2, 29));
    ASSERT_FALSE(DateTime::IsValidDate(2001, 2, 29));
    ASSERT_FALSE(DateTime::IsValidDate(2100, 2, 29));
    ASSERT_TRUE(DateTime::IsValidDate(2400, 2, 29));
    ASSERT_FALSE(DateTime::IsValidDate(2100, 13, 1));
    ASSERT_FALSE(DateTime::IsValidDate(2100, 12, 32));
    ASSERT_FALSE(DateTime::IsValidDate(2012, 8, 32));
    ASSERT_FALSE(DateTime::IsValidDate(2012, 8, 0));
    ASSERT_FALSE(DateTime::IsValidDate(2012, 0, 12));
    ASSERT_FALSE(DateTime::IsValidDate(2012, -1, 12));
    ASSERT_FALSE(DateTime::IsValidDate(2012, 6, 31));
    ASSERT_FALSE(DateTime::IsValidDate(2012, 9, 31));
    ASSERT_TRUE(DateTime::IsValidDate(2012, 8, 31));
    ASSERT_TRUE(DateTime::IsValidDate(2012, 1, 31));
    ASSERT_TRUE(DateTime::IsValidDate(2012, 3, 31));
    ASSERT_FALSE(DateTime::IsValidDate(2012, 4, 31));
}

TEST(TimeCounter, TestTimeCounter)
{
    TimeCounter tc1;
    tc1.Start();
    sleep(1);
    tc1.Pause();
    TimeSpan ts = tc1.GetTimeSpan();
    TimeCounter tc2(ts);
    ASSERT_EQ(tc1.ToString(), tc2.ToString());
    tc1.Reset();
    ASSERT_EQ(tc1.ToString(), TimeSpan::Zero.ToString());
}

} // end of namespace common
