//
// Created by 吴婷 on 2020-01-07.
//
#include <memory>
#include <map>

#include "thirdparty/monitor_sdk_common/libvep/vep_client.h"

#include "thirdparty/gtest/gtest.h"

using namespace common;

/**
 * VepClient 客户端功能测试
 */
class ClientFunctionTest : public testing::Test{
protected:
    virtual void SetUp(){
        std::string scene_code = "zzz";

        //客户端直接启动，有定时调度
        vep_client_ptr = std::make_shared<VepClient>(scene_code, "");
        vep_client_ptr->initialize(false);
    }

    virtual void TearDown(){
        vep_client_ptr->close();
    }

    std::shared_ptr<VepClient> vep_client_ptr;
};

TEST_F(ClientFunctionTest, testAbValuesSTWhiteV1){
    //abValueWithSegAndDiscardTag
    //进行中
    ResultVo<VepLabParamVo> result_vo1 = vep_client_ptr->abValueWithSegAndDiscardTag("zLab1220seg_tagA_S", "ccc");
    EXPECT_EQ(269, result_vo1.data.lab_version_id);

    //进行中
    ResultVo<VepLabParamVo> result_vo2 = vep_client_ptr->abValueWithSegAndDiscardTag("zLab1220seg_tagA_S", "aaa");
    EXPECT_EQ(267, result_vo2.data.lab_version_id);

    //申请中
    ResultVo<VepLabParamVo> result_vo3 = vep_client_ptr->abValueWithSegAndDiscardTag("zLab1220seg_tagB_S", "q");
    EXPECT_EQ(270, result_vo3.data.lab_version_id);

    //申请中
    ResultVo<VepLabParamVo> result_vo4 = vep_client_ptr->abValueWithSegAndDiscardTag("zLab1220seg_tagB_S", "w");
    EXPECT_EQ(271, result_vo4.data.lab_version_id);

    //未开始
    ResultVo<VepLabParamVo> result_vo5 = vep_client_ptr->abValueWithSegAndDiscardTag("zLab1220seg_tagC_S", "qqq");
    EXPECT_EQ(266, result_vo5.data.lab_version_id);

    //进行中未配置流量
    ResultVo<VepLabParamVo> result_vo6 = vep_client_ptr->abValueWithSegAndDiscardTag("zLab1220seg_tagD_S", "r");
    EXPECT_EQ(273, result_vo6.data.lab_version_id);

    //进行中未配置流量
    ResultVo<VepLabParamVo> result_vo7 = vep_client_ptr->abValueWithSegAndDiscardTag("zLab1220seg_tagD_S", "s");
    EXPECT_EQ("null\n", result_vo7.data.ToString());

    //abValueWithSegAndTag
    //进行中
    ResultVo<VepLabParamVo> result_vo11 = vep_client_ptr->abValueWithSegAndTag("zLab1220seg_tagA_S", "ccc", "a");
    EXPECT_EQ(269, result_vo11.data.lab_version_id);

    //进行中
    ResultVo<VepLabParamVo> result_vo12 = vep_client_ptr->abValueWithSegAndTag("zLab1220seg_tagA_S", "aaa", "a");
    EXPECT_EQ(267, result_vo12.data.lab_version_id);

    //申请中
    ResultVo<VepLabParamVo> result_vo13 = vep_client_ptr->abValueWithSegAndTag("zLab1220seg_tagB_S", "q", "a");
    EXPECT_EQ(270, result_vo13.data.lab_version_id);

    //申请中
    ResultVo<VepLabParamVo> result_vo14 = vep_client_ptr->abValueWithSegAndTag("zLab1220seg_tagB_S", "w", "a");
    EXPECT_EQ(271, result_vo14.data.lab_version_id);

    //未开始
    ResultVo<VepLabParamVo> result_vo15 = vep_client_ptr->abValueWithSegAndTag("zLab1220seg_tagC_S", "qqq", "a");
    EXPECT_EQ(266, result_vo15.data.lab_version_id);

    //进行中未配置流量
    ResultVo<VepLabParamVo> result_vo16 = vep_client_ptr->abValueWithSegAndTag("zLab1220seg_tagD_S", "r", "a");
    EXPECT_EQ(273, result_vo16.data.lab_version_id);

    //进行中未配置流量
    ResultVo<VepLabParamVo> result_vo17 = vep_client_ptr->abValueWithSegAndTag("zLab1220seg_tagD_S", "s", "a");
    EXPECT_EQ("null\n", result_vo17.data.ToString());
}

TEST_F(ClientFunctionTest, testAbValuesSTV1S){
    //abValueWithSegAndDiscardTag
    //进行中
    ResultVo<VepLabParamVo> result_vo75 = vep_client_ptr->abValueWithSegAndDiscardTag("zLab1220seg_tagA_S", "9");
    EXPECT_EQ("null\n", result_vo75.data.ToString());

    //申请中
    ResultVo<VepLabParamVo> result_vo3 = vep_client_ptr->abValueWithSegAndDiscardTag("zLab1220seg_tagB_S", "16");
    EXPECT_EQ("null\n", result_vo3.data.ToString());

    //申请中
    ResultVo<VepLabParamVo> result_vo4 = vep_client_ptr->abValueWithSegAndDiscardTag("zLab1220seg_tagB_S", "4");
    EXPECT_EQ("null\n", result_vo4.data.ToString());

    //未开始
    ResultVo<VepLabParamVo> result_vo5 = vep_client_ptr->abValueWithSegAndDiscardTag("zLab1220seg_tagC_S", "13");
    EXPECT_EQ("null\n", result_vo5.data.ToString());

    //进行中未配置流量
    ResultVo<VepLabParamVo> result_vo6 = vep_client_ptr->abValueWithSegAndDiscardTag("zLab1220seg_tagD_S", "");
    EXPECT_EQ("null\n", result_vo6.data.ToString());
}

TEST_F(ClientFunctionTest, testAbValuesSTV1ST){
    //abValueWithSegAndTag
    //进行中 S
    ResultVo<VepLabParamVo> result_vo0 = vep_client_ptr->abValueWithSegAndTag("zLab1220seg_tagA_S", "22","adas");
    EXPECT_EQ("null\n", result_vo0.data.ToString());

    //进行中 S T
    ResultVo<VepLabParamVo> result_vo1 = vep_client_ptr->abValueWithSegAndTag("zLab1220seg_tagA_S", "78", "2-bd!");
    EXPECT_EQ(269, result_vo1.data.lab_version_id);

    //进行中 S T
    ResultVo<VepLabParamVo> result_vo2 = vep_client_ptr->abValueWithSegAndTag("zLab1220seg_tagA_S", "22","2-23a");
    EXPECT_EQ(267, result_vo2.data.lab_version_id);

    //申请中
    ResultVo<VepLabParamVo> result_vo3 = vep_client_ptr->abValueWithSegAndTag("zLab1220seg_tagB_S", "16","b");
    EXPECT_EQ("null\n", result_vo3.data.ToString());

    //申请中
    ResultVo<VepLabParamVo> result_vo4 = vep_client_ptr->abValueWithSegAndTag("zLab1220seg_tagB_S", "4","b-43%");
    EXPECT_EQ("null\n", result_vo4.data.ToString());

    //未开始
    ResultVo<VepLabParamVo> result_vo5 = vep_client_ptr->abValueWithSegAndTag("zLab1220seg_tagC_S", "13", "b-bd!");
    EXPECT_EQ("null\n", result_vo5.data.ToString());

    //进行中未配置流量
    ResultVo<VepLabParamVo> result_vo6 = vep_client_ptr->abValueWithSegAndTag("zLab1220seg_tagD_S", "", "b-bd!");
    EXPECT_EQ("null\n", result_vo6.data.ToString());

}

TEST_F(ClientFunctionTest, testAbValuesSTV1WrongCase){
    ResultVo<VepLabParamVo> result_vo0 = vep_client_ptr->abValues("zLab1220seg_tagA_S", "ccc");
    EXPECT_EQ("使用api与实验配置方式不匹配", result_vo0.msg);
    EXPECT_EQ("null\n", result_vo0.data.ToString());

    ResultVo<VepLabParamVo> result_vo1 = vep_client_ptr->abValuesWithSeg("zLab1220seg_tagA_S", "ccc");
    EXPECT_EQ("使用api与实验配置方式不匹配", result_vo1.msg);
    EXPECT_EQ("null\n", result_vo1.data.ToString());

    ResultVo<VepLabParamVo> result_vo2 = vep_client_ptr->abValueWithTag("zLab1220seg_tagA_S", "a");
    EXPECT_EQ("使用api与实验配置方式不匹配", result_vo2.msg);
    EXPECT_EQ("null\n", result_vo2.data.ToString());
}