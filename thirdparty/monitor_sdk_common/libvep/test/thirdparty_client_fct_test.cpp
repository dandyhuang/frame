//
// Created by 吴婷 on 2020-01-07.
//
#include <memory>
#include <map>

#include "thirdparty/monitor_sdk_common/libvep/vep_thirdparty_related_client.h"

#include "thirdparty/gtest/gtest.h"

using namespace common;

/**
 * VepThirdPartyRelatedClient 客户端功能测试
 */
class ClientFunctionTest : public testing::Test{
protected:
    virtual void SetUp(){
        std::string position_type = TrafficPositionTypeEnum::AD;
        std::string layer_code = "zLayer1220Seg_Tag";

        //客户端直接启动，有定时调度
        vep_client_ptr = std::make_shared<VepThirdPartyRelatedClient>(position_type, layer_code);
        vep_client_ptr->initialize(false);
    }

    virtual void TearDown(){
        vep_client_ptr->close();
    }

    std::shared_ptr<VepThirdPartyRelatedClient> vep_client_ptr;
};

TEST_F(ClientFunctionTest, testAbValuesSTV2TLWhite){
    //进行中
    ResultVo<VepLabHitInfo> result_vo = vep_client_ptr->abValueWithSegAndDiscardTag("zLayer1220Seg_Tag", "1", "3b!");
    EXPECT_EQ(245, result_vo.data.lab_group_id);
    EXPECT_EQ(-1, result_vo.data.launch_layer_id);

    //进行中
    ResultVo<VepLabHitInfo> result_vo1 = vep_client_ptr->abValueWithSegAndDiscardTag("zLayer1220Seg_Tag", "1", "^#");
    EXPECT_EQ(248, result_vo1.data.lab_group_id);
    EXPECT_EQ(-1, result_vo1.data.launch_layer_id);

    //进行中
    ResultVo<VepLabHitInfo> result_vo2 = vep_client_ptr->abValueWithSegAndDiscardTag("zLayer1220Seg_Tag", "1", "$%");
    EXPECT_EQ(248, result_vo2.data.lab_group_id);
    EXPECT_EQ(-1, result_vo2.data.launch_layer_id);

    //进行中
    ResultVo<VepLabHitInfo> result_vo3 = vep_client_ptr->abValueWithSegAndDiscardTag("zLayer1220Seg_Tag", "1", "1b!");
    EXPECT_EQ(243, result_vo3.data.lab_group_id);
    EXPECT_EQ(-1, result_vo3.data.launch_layer_id);

    //申请中
    ResultVo<VepLabHitInfo> result_vo4 = vep_client_ptr->abValueWithSegAndDiscardTag("zLayer1220Seg_Tag", "1", "@");
    EXPECT_EQ(247, result_vo4.data.lab_group_id);
    EXPECT_EQ(-1, result_vo4.data.launch_layer_id);

    //申请中
    ResultVo<VepLabHitInfo> result_vo5 = vep_client_ptr->abValueWithSegAndDiscardTag("zLayer1220Seg_Tag", "1", ")(");
    EXPECT_EQ(249, result_vo5.data.lab_group_id);
    EXPECT_EQ(-1, result_vo5.data.launch_layer_id);

    //申请中
    ResultVo<VepLabHitInfo> result_vo6 = vep_client_ptr->abValueWithSegAndDiscardTag("zLayer1220Seg_Tag", "1", "~");
    EXPECT_EQ(249, result_vo6.data.lab_group_id);
    EXPECT_EQ(-1, result_vo6.data.launch_layer_id);

    //未开始
    ResultVo<VepLabHitInfo> result_vo7 = vep_client_ptr->abValueWithSegAndDiscardTag("zLayer1220Seg_Tag", "1", "9527");
    EXPECT_EQ(253, result_vo7.data.lab_group_id);
    EXPECT_EQ(-1, result_vo7.data.launch_layer_id);

    //不存在
    ResultVo<VepLabHitInfo> result_vo8 = vep_client_ptr->abValueWithSegAndDiscardTag("zLayer1220Seg_Tag", "1", "sdas");
    EXPECT_EQ("has not match any experiment", result_vo8.msg);

    //进行中
    ResultVo<VepLabHitInfo> result_vo9 = vep_client_ptr->abValueWithSegAndTag("zLayer1220Seg_Tag", "1", "3b!", "a");
    EXPECT_EQ(245, result_vo9.data.lab_group_id);
    EXPECT_EQ(-1, result_vo9.data.launch_layer_id);

    //进行中
    ResultVo<VepLabHitInfo> result_vo10 = vep_client_ptr->abValueWithSegAndTag("zLayer1220Seg_Tag", "1", "^#", "a");
    EXPECT_EQ(248, result_vo10.data.lab_group_id);
    EXPECT_EQ(-1, result_vo10.data.launch_layer_id);

    //申请中
    ResultVo<VepLabHitInfo> result_vo11 = vep_client_ptr->abValueWithSegAndTag("zLayer1220Seg_Tag", "1", ")(","a");
    EXPECT_EQ(249, result_vo11.data.lab_group_id);
    EXPECT_EQ(-1, result_vo11.data.launch_layer_id);

    //申请中
    ResultVo<VepLabHitInfo> result_vo12 = vep_client_ptr->abValueWithSegAndTag("zLayer1220Seg_Tag", "1", "~","a");
    EXPECT_EQ(249, result_vo12.data.lab_group_id);
    EXPECT_EQ(-1, result_vo12.data.launch_layer_id);

    //未开始
    ResultVo<VepLabHitInfo> result_vo13 = vep_client_ptr->abValueWithSegAndTag("zLayer1220Seg_Tag", "1", "8574","a");
    EXPECT_EQ(253, result_vo13.data.lab_group_id);
    EXPECT_EQ(-1, result_vo13.data.launch_layer_id);

    //不存在
    ResultVo<VepLabHitInfo> result_vo14 = vep_client_ptr->abValueWithSegAndTag("zLayer1220Seg_Tag", "1", "sdas","a");
    EXPECT_EQ("has not match any experiment", result_vo14.msg);

}

TEST_F(ClientFunctionTest, testAbValuesSTV2TLGroup){
    //abValueWithSegAndDiscardTag
    //进行中
    ResultVo<VepLabHitInfo> result_vo = vep_client_ptr->abValueWithSegAndDiscardTag("zLayer1220Seg_Tag", "1", "613");
    EXPECT_EQ(245, result_vo.data.lab_group_id);
    EXPECT_EQ(-1, result_vo.data.launch_layer_id);

    //进行中
    ResultVo<VepLabHitInfo> result_vo1 = vep_client_ptr->abValueWithSegAndDiscardTag("zLayer1220Seg_Tag", "1", "011");
    EXPECT_EQ(243, result_vo1.data.lab_group_id);
    EXPECT_EQ(-1, result_vo1.data.launch_layer_id);

    //进行中
    ResultVo<VepLabHitInfo> result_vo2 = vep_client_ptr->abValueWithSegAndDiscardTag("zLayer1220Seg_Tag", "1", "00");
    EXPECT_EQ(251, result_vo2.data.lab_group_id);
    EXPECT_EQ(-1, result_vo2.data.launch_layer_id);

    //申请中
    ResultVo<VepLabHitInfo> result_vo3 = vep_client_ptr->abValueWithSegAndDiscardTag("zLayer1220Seg_Tag", "1", "35");
    EXPECT_EQ("has not match any experiment", result_vo3.msg);
    EXPECT_EQ("null\n", result_vo3.data.ToString());

    //申请中
    ResultVo<VepLabHitInfo> result_vo4 = vep_client_ptr->abValueWithSegAndDiscardTag("zLayer1220Seg_Tag", "1", "656");
    EXPECT_EQ("has not match any experiment", result_vo4.msg);
    EXPECT_EQ("null\n", result_vo4.data.ToString());

    //未开始
    ResultVo<VepLabHitInfo> result_vo5 = vep_client_ptr->abValueWithSegAndDiscardTag("zLayer1220Seg_Tag", "1", "598");
    EXPECT_EQ("has not match any experiment", result_vo5.msg);
    EXPECT_EQ("null\n", result_vo5.data.ToString());

    //未开始
    ResultVo<VepLabHitInfo> result_vo6 = vep_client_ptr->abValueWithSegAndDiscardTag("zLayer1220Seg_Tag", "1", "868");
    EXPECT_EQ("has not match any experiment", result_vo6.msg);
    EXPECT_EQ("null\n", result_vo6.data.ToString());

    //申请中
    ResultVo<VepLabHitInfo> result_vo7 = vep_client_ptr->abValueWithSegAndDiscardTag("zLayer1220Seg_Tag", "1", "56");
    EXPECT_EQ("has not match any experiment", result_vo7.msg);
    EXPECT_EQ("null\n", result_vo7.data.ToString());

    //申请中
    ResultVo<VepLabHitInfo> result_vo8 = vep_client_ptr->abValueWithSegAndDiscardTag("zLayer1220Seg_Tag", "1", "75");
    EXPECT_EQ("has not match any experiment", result_vo8.msg);
    EXPECT_EQ("null\n", result_vo8.data.ToString());


    //abValueWithSegAndTag
    //进行中 ST
    ResultVo<VepLabHitInfo> result_vo10 = vep_client_ptr->abValueWithSegAndTag("zLayer1220Seg_Tag", "1", "613", "3-23a");
    EXPECT_EQ(245, result_vo10.data.lab_group_id);
    EXPECT_EQ(-1, result_vo10.data.launch_layer_id);

    //进行中 S
    ResultVo<VepLabHitInfo> result_vo11 = vep_client_ptr->abValueWithSegAndTag("zLayer1220Seg_Tag", "1", "011", "kkkk");
    EXPECT_EQ("has not match any experiment", result_vo11.msg);
    EXPECT_EQ("null\n", result_vo11.data.ToString());

    //进行中 ST
    ResultVo<VepLabHitInfo> result_vo12 = vep_client_ptr->abValueWithSegAndTag("zLayer1220Seg_Tag", "1", "00", "");
    EXPECT_EQ(251, result_vo12.data.lab_group_id);
    EXPECT_EQ(-1, result_vo12.data.launch_layer_id);

    //进行中 S
    ResultVo<VepLabHitInfo> result_vo13 = vep_client_ptr->abValueWithSegAndTag("zLayer1220Seg_Tag", "1", "00", "KKKK");
    EXPECT_EQ("has not match any experiment", result_vo13.msg);
    EXPECT_EQ("null\n", result_vo13.data.ToString());

    //申请中  ST
    ResultVo<VepLabHitInfo> result_vo14 = vep_client_ptr->abValueWithSegAndTag("zLayer1220Seg_Tag", "1", "656", "a");
    EXPECT_EQ("has not match any experiment", result_vo14.msg);
    EXPECT_EQ("null\n", result_vo14.data.ToString());

    //未开始 ST
    ResultVo<VepLabHitInfo> result_vo15 = vep_client_ptr->abValueWithSegAndTag("zLayer1220Seg_Tag", "1", "598","a");
    EXPECT_EQ("has not match any experiment", result_vo15.msg);
    EXPECT_EQ("null\n", result_vo15.data.ToString());

    //未开始 ST
    ResultVo<VepLabHitInfo> result_vo16 = vep_client_ptr->abValueWithSegAndTag("zLayer1220Seg_Tag", "1", "868","a");
    EXPECT_EQ("has not match any experiment", result_vo16.msg);
    EXPECT_EQ("null\n", result_vo16.data.ToString());

    //申请中 st
    ResultVo<VepLabHitInfo> result_vo17 = vep_client_ptr->abValueWithSegAndTag("zLayer1220Seg_Tag", "1", "56","a");
    EXPECT_EQ("has not match any experiment", result_vo17.msg);
    EXPECT_EQ("null\n", result_vo17.data.ToString());

    //申请中 st
    ResultVo<VepLabHitInfo> result_vo18 = vep_client_ptr->abValueWithSegAndTag("zLayer1220Seg_Tag", "1", "75","a");
    EXPECT_EQ("has not match any experiment", result_vo18.msg);
    EXPECT_EQ("null\n", result_vo18.data.ToString());

}

TEST_F(ClientFunctionTest, testAbValuesSTV2TLLanuch){
    //进行中 + 配置流量位
    ResultVo<VepLabHitInfo> result_vo = vep_client_ptr->abValueWithSegAndDiscardTag("zLayer1220Seg_Tag", "4", "613");
    EXPECT_EQ(-1, result_vo.data.lab_group_id);
    EXPECT_EQ(53, result_vo.data.launch_layer_id);

    //进行中 + 未配置流量位
    ResultVo<VepLabHitInfo> result_vo1 = vep_client_ptr->abValueWithSegAndDiscardTag("zLayer1220Seg_Tag", "5", "011");
    EXPECT_EQ("null\n", result_vo1.data.ToString());

    //进行中 + 未配置流量位
    ResultVo<VepLabHitInfo> result_vo2 = vep_client_ptr->abValueWithSegAndDiscardTag("zLayer1220Seg_Tag", "5", "00");
    EXPECT_EQ("null\n", result_vo2.data.ToString());

    //申请中 + 配置流量位
    ResultVo<VepLabHitInfo> result_vo3 = vep_client_ptr->abValueWithSegAndDiscardTag("zLayer1220Seg_Tag", "2", "35");
    EXPECT_EQ(-1, result_vo3.data.lab_group_id);
    EXPECT_EQ(53, result_vo3.data.launch_layer_id);

    //申请中 + 配置流量位
    ResultVo<VepLabHitInfo> result_vo4 = vep_client_ptr->abValueWithSegAndDiscardTag("zLayer1220Seg_Tag", "2", "656");
    EXPECT_EQ(-1, result_vo4.data.lab_group_id);
    EXPECT_EQ(53, result_vo4.data.launch_layer_id);

    //未开始 + 配置流量位
    ResultVo<VepLabHitInfo> result_vo5 = vep_client_ptr->abValueWithSegAndDiscardTag("zLayer1220Seg_Tag", "2", "598");
    EXPECT_EQ(-1, result_vo5.data.lab_group_id);
    EXPECT_EQ(53, result_vo5.data.launch_layer_id);

    //未开始 + 配置流量位
    ResultVo<VepLabHitInfo> result_vo6 = vep_client_ptr->abValueWithSegAndDiscardTag("zLayer1220Seg_Tag", "2", "868");
    EXPECT_EQ(-1, result_vo6.data.lab_group_id);
    EXPECT_EQ(53, result_vo6.data.launch_layer_id);

    //申请中 + 配置流量位
    ResultVo<VepLabHitInfo> result_vo7 = vep_client_ptr->abValueWithSegAndDiscardTag("zLayer1220Seg_Tag", "8", "56");
    EXPECT_EQ("null\n", result_vo7.data.ToString());

    //申请中 + 配置流量位
    ResultVo<VepLabHitInfo> result_vo8 = vep_client_ptr->abValueWithSegAndDiscardTag("zLayer1220Seg_Tag", "4", "75");
    EXPECT_EQ(-1, result_vo8.data.lab_group_id);
    EXPECT_EQ(53, result_vo8.data.launch_layer_id);

    //生效中命中
    ResultVo<VepLabHitInfo> result_vo10 = vep_client_ptr->abValueWithSegAndTag("zLayer1220Seg_Tag", "2", "613", "3-23a");
    EXPECT_EQ(245, result_vo10.data.lab_group_id);
    EXPECT_EQ(-1, result_vo10.data.launch_layer_id);

    //生效中 S 兜底
    ResultVo<VepLabHitInfo> result_vo11 = vep_client_ptr->abValueWithSegAndTag("zLayer1220Seg_Tag", "2", "011", "kkkk");
    EXPECT_EQ(-1, result_vo11.data.lab_group_id);
    EXPECT_EQ(53, result_vo11.data.launch_layer_id);

    //进行中 ST
    ResultVo<VepLabHitInfo> result_vo12 = vep_client_ptr->abValueWithSegAndTag("zLayer1220Seg_Tag", "2", "0", "");
    EXPECT_EQ(251, result_vo12.data.lab_group_id);
    EXPECT_EQ(-1, result_vo12.data.launch_layer_id);

    //生效中 S 兜底
    ResultVo<VepLabHitInfo> result_vo13 = vep_client_ptr->abValueWithSegAndTag("zLayer1220Seg_Tag", "2", "00", "KKKK");
    EXPECT_EQ(-1, result_vo13.data.lab_group_id);
    EXPECT_EQ(53, result_vo13.data.launch_layer_id);

    //申请中
    ResultVo<VepLabHitInfo> result_vo14 = vep_client_ptr->abValueWithSegAndTag("zLayer1220Seg_Tag", "2", "656", "a");
    EXPECT_EQ(-1, result_vo14.data.lab_group_id);
    EXPECT_EQ(53, result_vo14.data.launch_layer_id);

    //未开始
    ResultVo<VepLabHitInfo> result_vo15 = vep_client_ptr->abValueWithSegAndTag("zLayer1220Seg_Tag", "2", "598","a");
    EXPECT_EQ(-1, result_vo15.data.lab_group_id);
    EXPECT_EQ(53, result_vo15.data.launch_layer_id);

    //未开始 s
    ResultVo<VepLabHitInfo> result_vo16 = vep_client_ptr->abValueWithSegAndTag("zLayer1220Seg_Tag", "2", "868","a");
    EXPECT_EQ(-1, result_vo16.data.lab_group_id);
    EXPECT_EQ(53, result_vo16.data.launch_layer_id);

    //申请中 + 配置流量位 S T
    ResultVo<VepLabHitInfo> result_vo17 = vep_client_ptr->abValueWithSegAndTag("zLayer1220Seg_Tag", "2", "56","a");
    EXPECT_EQ(-1, result_vo17.data.lab_group_id);
    EXPECT_EQ(53, result_vo17.data.launch_layer_id);

    //申请中 + 未配置流量位
    ResultVo<VepLabHitInfo> result_vo18 = vep_client_ptr->abValueWithSegAndTag("zLayer1220Seg_Tag", "5", "75","a");
    EXPECT_EQ("null\n", result_vo18.data.ToString());

}

TEST_F(ClientFunctionTest, testAbValuesSTV2TLWrongCase){
    ResultVo<VepLabHitInfo> result_vo0 = vep_client_ptr->abValues("zLayer1220Seg_Tag", "1","a");
    EXPECT_EQ("使用api与实验配置方式不匹配", result_vo0.msg);
    EXPECT_EQ("null\n", result_vo0.data.ToString());

    ResultVo<VepLabHitInfo> result_vo1 = vep_client_ptr->abValuesWithSeg("zLayer1220Seg_Tag", "1", "a");
    EXPECT_EQ("使用api与实验配置方式不匹配", result_vo1.msg);
    EXPECT_EQ("null\n", result_vo1.data.ToString());

    ResultVo<VepLabHitInfo> result_vo2 = vep_client_ptr->abValueWithTag("zLayer1220Seg_Tag", "1", "a");
    EXPECT_EQ("使用api与实验配置方式不匹配", result_vo2.msg);
    EXPECT_EQ("null\n", result_vo2.data.ToString());

    ResultVo<VepLabHitInfo> result_vo3 = vep_client_ptr->abValueWithSegAndDiscardTag("zLayer1220Seg_Tag", "1", "");
    EXPECT_EQ("shunt is empty", result_vo3.msg);
    EXPECT_EQ("null\n", result_vo3.data.ToString());

    ResultVo<VepLabHitInfo> result_vo4 = vep_client_ptr->abValueWithSegAndTag("zLayer1220Seg_Tag", "1", "", "");
    EXPECT_EQ("shunt is empty", result_vo4.msg);
    EXPECT_EQ("null\n", result_vo4.data.ToString());

    ResultVo<VepLabHitInfo> result_vo5 = vep_client_ptr->abValueWithSegAndTag("zLayer1220Seg_Tag", "$%#^^*", "", "a");
    EXPECT_EQ("shunt is empty", result_vo5.msg);
    EXPECT_EQ("null\n", result_vo5.data.ToString());

    ResultVo<VepLabHitInfo> result_vo6 = vep_client_ptr->abValueWithSegAndTag("zLayer1220Seg_Tag", "$%#^^*", "1", "a");
    EXPECT_EQ("has not match any experiment", result_vo6.msg);
    EXPECT_EQ("null\n", result_vo6.data.ToString());
}