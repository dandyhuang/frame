//
// Created by 吴婷 on 2019-11-25.
//

#include "layer_report_param.h"

#include "thirdparty/gtest/gtest.h"

namespace common{
TEST(LayerReportParam, constructor){
    LayerReportParam layer_report("", "2.2.0.20191220-SNAPSHOT", "10.13.213.138",
                                1573089880267, 1573185788238, -1l, 1573185645932);

    EXPECT_EQ("2.2.0.20191220-SNAPSHOT", layer_report.client_version);
    EXPECT_EQ(1573089880267, layer_report.last_layer_changing_time);
    EXPECT_EQ(-1l, layer_report.last_watched_changing_time);

    LayerReportParam layer_report2(1573089880267, -1l, 1573185645932);

    EXPECT_EQ("2.2.0.20191220-SNAPSHOT", layer_report2.client_version);
    EXPECT_EQ(1573089880267, layer_report2.last_layer_changing_time);
    EXPECT_EQ(-1l, layer_report2.last_watched_changing_time);
}

}//namespace common


