/***************************************************************************
 *
 * Copyright (c) 2021 Vivo.com, Inc. All Rights Reserved
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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "KafkaDistribution.h"
#include "common/config_xml.h"


bool KafkaDistribution::Connect()
{
    if (m_conf.kafka_user_type == "writer") {
        m_is_reader = false;
    } else if (m_conf.kafka_user_type == "reader") {
        m_is_reader = true;
    } else {
        return false;
    }

    kafka_params_t kafka_param;
    kafka_param.broker = m_conf.kafka_brokers;
    kafka_param.group = m_conf.kafka_group_name;
    kafka_param.topic = m_conf.kafka_topic;
    kafka_param.msg_max_bytes = m_conf.msg_max_bytes;
    kafka_param.thread_num = m_conf.thread_num;
    if(m_conf.mechanism == "SCRAM-SHA-256"){
        kafka_param.mechanism = m_conf.mechanism;
        kafka_param.user_name = m_conf.user_name;
        kafka_param.password = m_conf.password;
    }
    if (m_is_reader) {
        m_reader.add_conf(kafka_param);
    } else {
        m_writer.add_conf(kafka_param);
        m_writer.set_max_queue_size(m_conf.max_queue_size);
    }

    return true;
}

bool KafkaDistribution::Run()
{
    int ret = 0;
    if (m_is_reader) {
        ret = m_reader.run();
    } else {
        ret = m_writer.run();
    }
    
    return ret == 0;
}

bool KafkaDistribution::ReadMessage(std::string& msg) 
{
    if (!m_is_reader) {
        return false;
    }

    return m_reader.read_message(msg) == 0 ? true : false;
}
    
bool KafkaDistribution::WriteMessage(const std::string& msg)
{
    if (m_is_reader) {
        return false;
    }
    
    return m_writer.write_message(msg) == 0 ? true : false;
}
bool KafkaDistribution::IsFull()
{
    return m_writer.is_full();
}
bool KafkaDistribution::Stop()
{
    if (m_is_reader) {
        m_reader.close();
    } else {
        m_writer.close();
    }
    
    return true;
}

KafkaDistributionMgr* KafkaDistributionMgr::m_instance = NULL;


KafkaDistributionMgr* KafkaDistributionMgr::Instance()
{
    if (m_instance == NULL) {
        m_instance = new KafkaDistributionMgr();
    }
    return m_instance;
}

bool KafkaDistributionMgr::Initialize(const std::string& conf_path)
{
    m_configFile = conf_path;
    int fd = open(m_configFile.c_str(), O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "open kafka distribution config file %s failed\n", m_configFile.c_str());
        return false;
    }
    struct stat s;
    if(0 != fstat(fd, &s)) {
        close(fd);
        fprintf(stderr, "fstat kafka distribution config file %s failed\n", m_configFile.c_str());
        return false;
    }
    close(fd);
    

    std::map<std::string, KafkaDistributionConf> moduleConf;
    if (false == LoadConfigFile(moduleConf)) {
        return false;
    }

    for (const auto &moduleIter : moduleConf) {
        KafkaDistribution * kafkaDistribution = new KafkaDistribution(moduleIter.second);
        if(kafkaDistribution){
            kafkaDistribution->Connect();
            kafkaDistribution->Run();
        }else{
            return false;
        }
        m_onlineKafkaDistribution[moduleIter.second.module_name] = kafkaDistribution;
    }

    return true;
}

bool KafkaDistributionMgr::LoadConfigFile(std::map<std::string, KafkaDistributionConf> &moduleConf)
{
    
    vv_feed::ConfigXml xml_conf;
    xml_conf.init(m_configFile);
		
    vv_feed::ConfigXml node;
    std::string node_name = "module";

    moduleConf.clear();
	bool has_next = false;
    if (!xml_conf.Find(node_name, node)) {
        LOG(FATAL) << "fetcher not found in file ";
        return false;
    }
    
    do {
        
        KafkaDistributionConf moduleInfo;
        node.Attr<std::string>("module_name", moduleInfo.module_name);
        node.Attr<int32_t>("heartbeat_interval", moduleInfo.heartbeat_interval);
        node.Attr<std::string>("user_type", moduleInfo.kafka_user_type);
        node.Attr<std::string>("brokers", moduleInfo.kafka_brokers);
        node.Attr<std::string>("topic", moduleInfo.kafka_topic);
        node.Attr<std::string>("group_name", moduleInfo.kafka_group_name);
        node.Attr<std::string>("msg_max_bytes", moduleInfo.msg_max_bytes);
        node.Attr<std::string>("sasl_mechanism", moduleInfo.mechanism);
        node.Attr<std::string>("sasl_username", moduleInfo.user_name);
        node.Attr<std::string>("sasl_pwssword", moduleInfo.password);        
        node.Attr<uint32_t>("thread_num", moduleInfo.thread_num);
        node.Attr<int>("max_queue_size", moduleInfo.max_queue_size);
        
        has_next = node.Next(node_name, node);


        moduleConf[moduleInfo.module_name] = moduleInfo;
    } while(has_next);


    return true;
}


KafkaDistribution* KafkaDistributionMgr::GetKafkaByModuleId(std::string module_name)
{
    auto pos = m_onlineKafkaDistribution.find(module_name);
    if (pos != m_onlineKafkaDistribution.end() ) {
        return pos->second;
    }

    return NULL;
}