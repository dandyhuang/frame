//
// Created by 吴婷 on 2020-01-07.
//
#include <memory>
#include <map>

#include "thirdparty/monitor_sdk_common/libvep/vep_scene_related_client.h"

#include "thirdparty/gtest/gtest.h"

using namespace common;

/**
 * VepSceneRelatedClient 客户端功能测试
 */
class ClientFunctionTest : public testing::Test{
protected:
    virtual void SetUp(){
        std::string scene_code = "zzz";
        std::string layer_code = "zLayer1220Seg_Tag";

        //客户端直接启动，有定时调度
        vep_client_ptr = std::make_shared<VepSceneRelatedClient>(scene_code, layer_code);
        vep_client_ptr->initialize(false);
    }

    virtual void TearDown(){
        vep_client_ptr->close();
    }

    std::shared_ptr<VepSceneRelatedClient> vep_client_ptr;
};

TEST_F(ClientFunctionTest, testAbValuesSTWhiteWithoutLayer){
    //生效中无流量
    ResultVo<VepLabHitInfo> result_vo = vep_client_ptr->abValueWithSegAndDiscardTag("zLayer1220Seg_Tag", "a");
    EXPECT_EQ(255, result_vo.data.lab_group_id);
    EXPECT_EQ(-1, result_vo.data.launch_layer_id);

    //生效中无流量
    ResultVo<VepLabHitInfo> result_vo1 = vep_client_ptr->abValueWithSegAndDiscardTag("zLayer1220Seg_Tag",  "b");
    EXPECT_EQ(256, result_vo1.data.lab_group_id);
    EXPECT_EQ(-1, result_vo1.data.launch_layer_id);

    //申请中
    ResultVo<VepLabHitInfo> result_vo2 = vep_client_ptr->abValueWithSegAndDiscardTag("zLayer1220Seg_Tag", "260");
    EXPECT_EQ(260, result_vo2.data.lab_group_id);
    EXPECT_EQ(-1, result_vo2.data.launch_layer_id);

    //申请中
    ResultVo<VepLabHitInfo> result_vo3 = vep_client_ptr->abValueWithSegAndDiscardTag("zLayer1220Seg_Tag", "259");
    EXPECT_EQ(259, result_vo3.data.lab_group_id);
    EXPECT_EQ(-1, result_vo3.data.launch_layer_id);

    //生效中
    ResultVo<VepLabHitInfo> result_vo4 = vep_client_ptr->abValueWithSegAndDiscardTag("zLayer1220Seg_Tag", "bbbb");
    EXPECT_EQ(262, result_vo4.data.lab_group_id);
    EXPECT_EQ(-1, result_vo4.data.launch_layer_id);

    //生效中
    ResultVo<VepLabHitInfo> result_vo5 = vep_client_ptr->abValueWithSegAndDiscardTag("zLayer1220Seg_Tag", "tttt");
    EXPECT_EQ(263, result_vo5.data.lab_group_id);
    EXPECT_EQ(-1, result_vo5.data.launch_layer_id);

    //未开始
    ResultVo<VepLabHitInfo> result_vo6 = vep_client_ptr->abValueWithSegAndDiscardTag("zLayer1220Seg_Tag", "ccc");
    EXPECT_EQ(264, result_vo6.data.lab_group_id);
    EXPECT_EQ(-1, result_vo6.data.launch_layer_id);

    //生效中无流量(忽略TAG）
    ResultVo<VepLabHitInfo> result_vo8 = vep_client_ptr->abValueWithSegAndTag("zLayer1220Seg_Tag",  "a", "a");
    EXPECT_EQ(255, result_vo8.data.lab_group_id);
    EXPECT_EQ(-1, result_vo8.data.launch_layer_id);

    //生效中无流量(忽略TAG）
    ResultVo<VepLabHitInfo> result_vo9 = vep_client_ptr->abValueWithSegAndTag("zLayer1220Seg_Tag",  "b", "a");
    EXPECT_EQ(256, result_vo9.data.lab_group_id);
    EXPECT_EQ(-1, result_vo9.data.launch_layer_id);

    //申请中(忽略TAG）
    ResultVo<VepLabHitInfo> result_vo10 = vep_client_ptr->abValueWithSegAndTag("zLayer1220Seg_Tag", "260", "a");
    EXPECT_EQ(260, result_vo10.data.lab_group_id);
    EXPECT_EQ(-1, result_vo10.data.launch_layer_id);

    //申请中(忽略TAG）
    ResultVo<VepLabHitInfo> result_vo11 = vep_client_ptr->abValueWithSegAndTag("zLayer1220Seg_Tag",  "259", "a");
    EXPECT_EQ(259, result_vo11.data.lab_group_id);
    EXPECT_EQ(-1, result_vo11.data.launch_layer_id);

    //生效中
    ResultVo<VepLabHitInfo> result_vo12 = vep_client_ptr->abValueWithSegAndTag("zLayer1220Seg_Tag",  "bbbb", "a");
    EXPECT_EQ(262, result_vo12.data.lab_group_id);
    EXPECT_EQ(-1, result_vo12.data.launch_layer_id);

    // 生效中
    ResultVo<VepLabHitInfo> result_vo13 = vep_client_ptr->abValueWithSegAndTag("zLayer1220Seg_Tag", "tttt", "a");
    EXPECT_EQ(263, result_vo13.data.lab_group_id);
    EXPECT_EQ(-1, result_vo13.data.launch_layer_id);

    //未开始
    ResultVo<VepLabHitInfo> result_vo14 = vep_client_ptr->abValueWithSegAndTag("zLayer1220Seg_Tag",  "ccc","a");
    EXPECT_EQ(264, result_vo14.data.lab_group_id);
    EXPECT_EQ(-1, result_vo14.data.launch_layer_id);
}


TEST_F(ClientFunctionTest, testAbValuesSTV2TLGroup_Launch){
    //申请中
    ResultVo<VepLabHitInfo> result_vo = vep_client_ptr->abValueWithSegAndDiscardTag("zLayer1220Seg_Tag", "613");
    EXPECT_EQ(-1, result_vo.data.lab_group_id);
    EXPECT_EQ(54, result_vo.data.launch_layer_id);

    //申请中
    ResultVo<VepLabHitInfo> result_vo1 = vep_client_ptr->abValueWithSegAndDiscardTag("zLayer1220Seg_Tag", "011");
    EXPECT_EQ(-1, result_vo1.data.lab_group_id);
    EXPECT_EQ(54, result_vo1.data.launch_layer_id);

    //生效中
    ResultVo<VepLabHitInfo> result_vo2 = vep_client_ptr->abValueWithSegAndDiscardTag("zLayer1220Seg_Tag", "424");
    EXPECT_EQ(262, result_vo2.data.lab_group_id);
    EXPECT_EQ(-1, result_vo2.data.launch_layer_id);

    //生效中
    ResultVo<VepLabHitInfo> result_vo3 = vep_client_ptr->abValueWithSegAndDiscardTag("zLayer1220Seg_Tag", "675");
    EXPECT_EQ(261, result_vo3.data.lab_group_id);
    EXPECT_EQ(-1, result_vo3.data.launch_layer_id);

    //未开始
    ResultVo<VepLabHitInfo> result_vo4 = vep_client_ptr->abValueWithSegAndDiscardTag("zLayer1220Seg_Tag", "90");
    EXPECT_EQ(-1, result_vo4.data.lab_group_id);
    EXPECT_EQ(54, result_vo4.data.launch_layer_id);

    //申请中
    ResultVo<VepLabHitInfo> result_vo10 = vep_client_ptr->abValueWithSegAndTag("zLayer1220Seg_Tag", "613", "a");
    EXPECT_EQ(-1, result_vo10.data.lab_group_id);
    EXPECT_EQ(54, result_vo10.data.launch_layer_id);

    //申请中
    ResultVo<VepLabHitInfo> result_vo11 = vep_client_ptr->abValueWithSegAndTag("zLayer1220Seg_Tag", "011","a");
    EXPECT_EQ(-1, result_vo11.data.lab_group_id);
    EXPECT_EQ(54, result_vo11.data.launch_layer_id);

    //生效中
    ResultVo<VepLabHitInfo> result_vo12 = vep_client_ptr->abValueWithSegAndTag("zLayer1220Seg_Tag", "424", "a-//1");
    EXPECT_EQ(262, result_vo12.data.lab_group_id);
    EXPECT_EQ(-1, result_vo12.data.launch_layer_id);

    //生效中
    ResultVo<VepLabHitInfo> result_vo13 = vep_client_ptr->abValueWithSegAndTag("zLayer1220Seg_Tag", "675","KKKK");
    EXPECT_EQ(-1, result_vo13.data.lab_group_id);
    EXPECT_EQ(54, result_vo13.data.launch_layer_id);

    //未开始
    ResultVo<VepLabHitInfo> result_vo14 = vep_client_ptr->abValueWithSegAndTag("zLayer1220Seg_Tag", "90", "a");
    EXPECT_EQ(-1, result_vo14.data.lab_group_id);
    EXPECT_EQ(54, result_vo14.data.launch_layer_id);

}


TEST_F(ClientFunctionTest, testAbValuesSTV2SLWrongCase){
    ResultVo<VepLabHitInfo> result_vo0 = vep_client_ptr->abValues("zLayer1220Seg_Tag", "1");
    EXPECT_EQ("使用api与实验配置方式不匹配", result_vo0.msg);
    EXPECT_EQ("null\n", result_vo0.data.ToString());

    ResultVo<VepLabHitInfo> result_vo1 = vep_client_ptr->abValuesWithSeg("zLayer1220Seg_Tag", "a");
    EXPECT_EQ("使用api与实验配置方式不匹配", result_vo1.msg);
    EXPECT_EQ("null\n", result_vo1.data.ToString());

    ResultVo<VepLabHitInfo> result_vo2 = vep_client_ptr->abValueWithTag("zLayer1220Seg_Tag",  "a");
    EXPECT_EQ("使用api与实验配置方式不匹配", result_vo2.msg);
    EXPECT_EQ("null\n", result_vo2.data.ToString());

    ResultVo<VepLabHitInfo> result_vo3 = vep_client_ptr->abValueWithSegAndDiscardTag("zLayer1220Seg_Tag", "");
    EXPECT_EQ("shunt is empty", result_vo3.msg);
    EXPECT_EQ("null\n", result_vo3.data.ToString());

    ResultVo<VepLabHitInfo> result_vo4 = vep_client_ptr->abValueWithSegAndTag("zLayer1220Seg_Tag", "", "");
    EXPECT_EQ("shunt is empty", result_vo4.msg);
    EXPECT_EQ("null\n", result_vo4.data.ToString());

    ResultVo<VepLabHitInfo> result_vo5 = vep_client_ptr->abValueWithSegAndTag("zLayer1220Seg_Tag",  "", "a");
    EXPECT_EQ("shunt is empty", result_vo5.msg);
    EXPECT_EQ("null\n", result_vo5.data.ToString());

    ResultVo<VepLabHitInfo> result_vo6 = vep_client_ptr->abValueWithSegAndTag("zLayer1220Seg_Tag", "1", "a");
    EXPECT_EQ(-1, result_vo6.data.lab_group_id);
    EXPECT_EQ(54, result_vo6.data.launch_layer_id);
}