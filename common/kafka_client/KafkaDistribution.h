/***************************************************************************
 *
 * Copyright (c) 2021 Dandyhuang.com, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/

 /**
 * @author rtrs(yangyiyan)
 * @date 2021/05/20 15:32:07
 * @version $Revision$
 * @brief
 *
 **/
#ifndef KAFKA_DISTRIBUTION_H
#define KAFKA_DISTRIBUTION_H

#include "common/kafka_client/KafkaClient.h"
#include <map>

#define Default_thread_num 4

struct KafkaDistributionConf {
    int32_t moduleid;
    std::string module_name;
    int32_t heartbeat_interval;

    std::string kafka_user_type;
    std::string kafka_brokers;
    std::string kafka_topic;
    std::string kafka_group_name;
    std::string msg_max_bytes;
    int need_security;
    std::string mechanism;
    std::string user_name;
    std::string password;
    uint32_t thread_num;
    int max_queue_size;
};

class KafkaDistribution {
public:
    KafkaDistribution(KafkaDistributionConf conf): m_conf(conf) {}
    ~KafkaDistribution() {}

    bool Connect();
    bool Run();
    bool ReadMessage(std::string& msg);     // 备注：此处的read message是异步接口，不进行阻塞
    bool WriteMessage(const std::string& msg);
    bool IsFull();
    bool Stop();

private:
    KafkaDistributionConf m_conf;

    KafkaReader m_reader;
    KafkaWriter m_writer;
    bool m_is_reader;

};

class KafkaDistributionMgr {
public:
    KafkaDistributionMgr() {}
    ~KafkaDistributionMgr() {}

    static KafkaDistributionMgr* Instance();

    bool Initialize(const std::string & conf_path);
    bool LoadConfigFile(std::map<std::string, KafkaDistributionConf> &moduleConf);
    KafkaDistribution* GetKafkaByModuleId(std::string  module_name);

private:
    std::string m_configFile;
    time_t m_lastUpdateTime;
    std::mutex m_mutex;
    std::map<std::string, KafkaDistribution*> m_onlineKafkaDistribution;
    std::map<time_t, std::vector<KafkaDistribution*>> m_deferReleaseDistribution;

    static KafkaDistributionMgr* m_instance;
};

#endif
