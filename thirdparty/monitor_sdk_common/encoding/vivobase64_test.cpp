//
// Created by 王锦涛 on 2019-11-13.
//
#include <iostream>
#include <string>
#include <vector>

#include "thirdparty/monitor_sdk_common/encoding/vivobase64.h"
#include "thirdparty/gtest/gtest.h"
#include "thirdparty/gmock/gmock.h"

using namespace std;

namespace common {

TEST(VivoBase64, EncodeTest1)
{
    VivoBase64 vb;
    string str = "Qv4QQQQQQJC-xKQ--MTP-e-CWe-CWNclT9k0WeE0T9ZCOQQvNKQQQQQQQQQQDdCh3ZBWGz6Ke_8Oh2uwjQ";
    int tmp[] = {0, 45, 0, 0, 0, 0, 0, -99, 68, 109, 0, 4, 17, 51, 49, 18, 49, 53, 54, 49, 53, 52, 55, 55, 51, 53, 54, 54, 50, 54, 51, 54, 53, 56, 0, 2, 13, 0, 0, 0, 0, 0, 0, 0, 0, -54, 13, 85, 125, -113, -115, 42, 38, 80, -115, 16, 78, 86, 105, 111, -96};
    vector<char> data;
    for (int i = 0; i < 61; ++i) {
        data.push_back((char)tmp[i]);
    }
    ASSERT_EQ(vb.Encode(data), str);
}

TEST(VivoBase64, EncodeTest2)
{
    VivoBase64 vb;
    string str = "Qv4QQQQQa9hbfQQ--MTP-e-CW9E9TMHDTMc0WNK0TNc9TQQvNKQQQQQQQQQQFGasNbT1";
    int tmp[51] = {0, 45, 0, 0, 0, 0, 31, 53, 106, -64, 0, 4, 17, 51, 49, 18, 49, 53, 55, 50, 51, 49, 57, 50, 49, 55, 54, 52, 52, 54, 48, 55, 51, 48, 0, 2, 13, 0, 0, 0, 0, 0, 0, 0, 0, -76, -95, -20, 14, -93, 57};
    vector<char> data;
    for (int i = 0; i < 51; ++i) {
        data.push_back((char)tmp[i]);
    }
    ASSERT_EQ(vb.Encode(data), str);
}

TEST(VivoBase64, EncodeTest3)
{
    VivoBase64 vb;
    string str = "Qv4QQQQQX9FihKQ--MTP-e-CWeT4WMd0WMQCTMTfTMEmWfQvNKQQQQQQQQQQWf4trK";
    int tmp[49] = {0, 45, 0, 0, 0, 0, -97, 59, 127, 85, 0, 4, 17, 51, 49, 18, 49, 53, 54, 51, 52, 53, 56, 54, 53, 48, 53, 49, 51, 48, 49, 50, 56, 55, 0, 2, 13, 0, 0, 0, 0, 0, 0, 0, 0, 55, 13, 15, 21};
    vector<char> data;
    for (int i = 0; i < 49; ++i) {
        data.push_back((char)tmp[i]);
    }
    ASSERT_EQ(vb.Encode(data), str);
}

TEST(VivoBase64, DecodeTest1)
{
    VivoBase64 vb;
    string str = "Qv4QQQQQQJC-xKQ--MTP-e-CWe-CWNclT9k0WeE0T9ZCOQQvNKQQQQQQQQQQDdCh3ZBWGz6Ke_8Oh2uwjQ";
    vector<char> data = vb.Decode(str);
    string predict = "0,45,0,0,0,0,0,-99,68,109,0,4,17,51,49,18,49,53,54,49,53,52,55,55,51,53,54,54,50,54,51,54,53,56,0,2,13,0,0,0,0,0,0,0,0,-54,13,85,125,-113,-115,42,38,80,-115,16,78,86,105,111,-96,";
    string output = "";
    vector<char>::iterator iter;
    for (iter = data.begin(); iter != data.end(); iter++) {
        output += to_string((int) * iter) + ",";
    }
    ASSERT_EQ(predict, output);
}

TEST(VivoBase64, DecodeTest2)
{
    VivoBase64 vb;
    string str = "Qv4QQQQQa9hbfQQ--MTP-e-CW9E9TMHDTMc0WNK0TNc9TQQvNKQQQQQQQQQQFGasNbT1";
    vector<char> data = vb.Decode(str);
    string predict = "0,45,0,0,0,0,31,53,106,-64,0,4,17,51,49,18,49,53,55,50,51,49,57,50,49,55,54,52,52,54,48,55,51,48,0,2,13,0,0,0,0,0,0,0,0,-76,-95,-20,14,-93,57,";
    string output = "";
    vector<char>::iterator iter;
    for (iter = data.begin(); iter != data.end(); iter++) {
        output += to_string((int) * iter) + ",";
    }
    ASSERT_EQ(predict, output);
}

TEST(VivoBase64, DecodeTest3)
{
    VivoBase64 vb;
    string str = "Qv4QQQQQX9FihKQ--MTP-e-CWeT4WMd0WMQCTMTfTMEmWfQvNKQQQQQQQQQQWf4trK";
    vector<char> data = vb.Decode(str);
    string predict = "0,45,0,0,0,0,-97,59,127,85,0,4,17,51,49,18,49,53,54,51,52,53,56,54,53,48,53,49,51,48,49,50,56,55,0,2,13,0,0,0,0,0,0,0,0,55,13,15,21,";
    string output = "";
    vector<char>::iterator iter;
    for (iter = data.begin(); iter != data.end(); iter++) {
        output += to_string((int) * iter) + ",";
    }
    ASSERT_EQ(predict, output);
}

} // namespace common
