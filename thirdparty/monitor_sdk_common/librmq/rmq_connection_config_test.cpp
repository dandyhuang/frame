
#include "thirdparty/monitor_sdk_common/librmq/rmq_connection_config.h"

#include "thirdparty/gtest/gtest.h"


namespace common {

TEST(RmqConnectionConfigTest, TestDeserializeConnectionConfigs) {
  std::string s = "[{\n"
                "    \"producerAuth\": false,\n"
                "    \"nexTime\": 11000,\n"
                "    \"restart\": false,\n"
                "    \"topicKey\": \"f178c5b54df445a0963313afb1b79305\",\n"
                "    \"producer\": true,\n"
                "    \"exchange\": \"rmq-test-vivocfg-agent-test-exchange\",\n"
                "    \"exchangeType\": \"fanout\",\n"
                "    \"rate\": 20000000,\n"
                "    \"queue\": \"rmq-test-vivocfg-api-test-queue\",\n"
                "    \"routingKey\": \"\",\n"
                "    \"threadNum\": 32,\n"
                "    \"prefetch\": 128,\n"
                "    \"priority\": false,\n"
                "    \"username\": \"guest\",\n"
                "    \"password\": \"guest\",\n"
                "    \"port\": 5672,\n"
                "    \"configs\": [\n"
                "      {\n"
                "        \"vhost\": \"vivo\",\n"
                "        \"nodes\": [\n"
                "          \"10.101.33.26\",\n"
                "          \"10.101.33.27\",\n"
                "          \"10.101.33.25\"\n"
                "        ]\n"
                "      },\n"
                "      {\n"
                "        \"vhost\": \"vivo$$1\",\n"
                "        \"nodes\": [\n"
                "          \"10.101.33.26\",\n"
                "          \"10.101.33.25\",\n"
                "          \"10.101.33.27\"\n"
                "        ]\n"
                "      },\n"
                "      {\n"
                "        \"vhost\": \"vivo$$2\",\n"
                "        \"nodes\": [\n"
                "          \"10.101.33.27\",\n"
                "          \"10.101.33.26\",\n"
                "          \"10.101.33.25\"\n"
                "        ]\n"
                "      },\n"
                "      {\n"
                "        \"vhost\": \"vivo$$3\",\n"
                "        \"nodes\": [\n"
                "          \"10.101.33.25\",\n"
                "          \"10.101.33.26\",\n"
                "          \"10.101.33.27\"\n"
                "        ]\n"
                "      },\n"
                "      {\n"
                "        \"vhost\": \"vivo$$4\",\n"
                "        \"nodes\": [\n"
                "          \"10.101.33.27\",\n"
                "          \"10.101.33.26\",\n"
                "          \"10.101.33.25\"\n"
                "        ]\n"
                "      },\n"
                "      {\n"
                "        \"vhost\": \"vivo$$5\",\n"
                "        \"nodes\": [\n"
                "          \"10.101.33.26\",\n"
                "          \"10.101.33.27\",\n"
                "          \"10.101.33.25\"\n"
                "        ]\n"
                "      }\n"
                "    ]\n"
                "  }]";

  std::vector<RmqConnectionConfig> rcc;
  RmqConnectionConfig::DeserializeConnectionConfigs(s, &rcc);
  RmqConnectionConfig config = rcc[0];
  EXPECT_EQ(false, config.priority);
  EXPECT_EQ(false, config.producer_auth);
  EXPECT_EQ("f178c5b54df445a0963313afb1b79305", config.topic_key);
  EXPECT_EQ(6, int(config.configs.size()));
  EXPECT_EQ("", config.routing_key);
  EXPECT_EQ("guest", config.username);
}

} // namespace common