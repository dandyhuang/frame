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
#include <string.h>
#include "KafkaClient.h"
#include "concurrentqueue.h"
#include "stdio.h"

 
bool KafkaWriter::bRunFlag = true;
bool KafkaReader::bRunFlag = true;

KafkaWriter::KafkaWriter()
{
    timeout_ms = 1000;
    msg_num = 0;
}

KafkaWriter::~KafkaWriter()
{
    close();
}

void KafkaWriter::dr_msg_cb(rd_kafka_t *rk, const rd_kafka_message_t *rkmessage, void *opaque)
{
    if (rkmessage->err)
    {
		//cout << "the error msg: " << rd_kafka_err2str(rkmessage->err) << endl;
        //OpenFalconReport(0, NULL_AlgID, "rd_kafka_error", 1);
        printf("dr_msg_cb:Message delivery failed! error=[%s] \n", rd_kafka_err2str(rkmessage->err));
    }
    else
    {
		//cout << "msg get ok. len=" << rkmessage->len << ",partition=" << rkmessage->partition << endl;
        /* rkmessage被librdkafka自动销毁*/
        //OpenFalconReport(0, NULL_AlgID, "rd_kafka_success", 1);
        ////DEBUG_LOG("dr_msg_cb:Message delivery ok!len=[%u],partition=[%d]", rkmessage->len, rkmessage->partition);
    }    
}


rd_kafka_t* KafkaWriter::createServer(const char *brokers, const char *group, const char *msg_max_bytes,
                                      const char *mechanism, const char *user_name,const char* password)
{
    rd_kafka_t *rk;            /*Producer instance handle*/
	rd_kafka_conf_t *conf;     /*临时配置对象*/
    char errstr[512];

    /* 创建一个kafka配置占位 */
	conf = rd_kafka_conf_new();
 
    /*创建broker集群*/
    memset(errstr, '\0', sizeof(errstr));
	if (rd_kafka_conf_set(conf, "bootstrap.servers", brokers, errstr, sizeof(errstr)) != RD_KAFKA_CONF_OK)
    { 
        printf("createServer:rd_kafka_conf_set bootstrap.servers failed! error=[%s]", errstr);
        return NULL;
	}

    /*设置生产者单条消息的最大长度*/
    if (NULL != msg_max_bytes && strcmp(msg_max_bytes, "\0") != 0) {
        memset(errstr, '\0', sizeof(errstr));
	    if (rd_kafka_conf_set(conf, "message.max.bytes", msg_max_bytes, errstr, sizeof(errstr)) != RD_KAFKA_CONF_OK)
        { 
            printf("createServer:rd_kafka_conf_set message.max.bytes failed! error=[%s]", errstr);
            return NULL;
	}
    }

    /*设置发送报告回调函数，rd_kafka_produce()接收的每条消息都会调用一次该回调函数 应用程序需要定期调用rd_kafka_poll()来服务排队的发送报告回调函数*/
    rd_kafka_conf_set_dr_msg_cb(conf, dr_msg_cb);

    if (group == NULL)
    {
        group = "test";
    }
    /*设置group*/
    memset(errstr, '\0', sizeof(errstr));
    if (rd_kafka_conf_set(conf, "group.id", group, errstr, sizeof(errstr)) != RD_KAFKA_CONF_OK)
    {
        printf("createServer:rd_kafka_conf_set failed, error=[%s]", errstr);
        return NULL;
    }

    /*设置安全参数*/
    if (mechanism != "")
    {
       if(rd_kafka_conf_set(conf, "security.protocol", "sasl_plaintext", errstr, sizeof(errstr)) != RD_KAFKA_CONF_OK){
           printf("createServer:rd_kafka_conf_set failed, error=[%s]", errstr);
           return NULL;
       }
       
       if(rd_kafka_conf_set(conf, "sasl.mechanism", mechanism, errstr, sizeof(errstr)) != RD_KAFKA_CONF_OK){
           printf("createServer:rd_kafka_conf_set failed, error=[%s]", errstr);
           return NULL;
       }
    }

    if (user_name != "" && rd_kafka_conf_set(conf, "sasl.username", user_name, errstr, sizeof(errstr)) != RD_KAFKA_CONF_OK)
    {
        printf("createServer:rd_kafka_conf_set failed, error=[%s]", errstr);
        return NULL;
    }

    if (password != "" && rd_kafka_conf_set(conf, "sasl.password", password, errstr, sizeof(errstr)) != RD_KAFKA_CONF_OK)
    {
        printf("createServer:rd_kafka_conf_set failed, error=[%s]", errstr);
        return NULL;
    }

    if (rd_kafka_conf_set(conf, "queue.buffering.max.messages", to_string(_max_queue_size + 10000).c_str(), NULL, 0) != RD_KAFKA_CONF_OK)
    {
        printf("createServer:rd_kafka_conf_set failed, error=[%s]", errstr);
        return NULL;
    }

    /*创建producer实例 rd_kafka_new()获取conf对象的所有权,应用程序在此调用之后不得再次引用它*/
    memset(errstr, '\0', sizeof(errstr));
    rk = rd_kafka_new(RD_KAFKA_PRODUCER, conf,  errstr, sizeof(errstr));
    if(rk == NULL)
    {
		printf("createServer:rd_kafka_new failed! error=[%s]", errstr);
        return NULL;
	}

    return rk;
}

rd_kafka_topic_t* KafkaWriter::createTopic(/*const*/ rd_kafka_t* rk,const char *topic)
{
    rd_kafka_topic_t *rkt;     /*topic对象*/

    rkt = rd_kafka_topic_new(rk, topic, NULL);
	if (rkt == NULL)
    {
        printf("createTopic:rd_kafka_topic_new failed! error=[%s]", rd_kafka_err2str(rd_kafka_last_error()));
		rd_kafka_destroy(rk);
		return NULL;
	}

    return rkt;
}

void KafkaWriter::close()
{
    KafkaWriter::setExitFlag(false);
    //INFO_LOG("KafkaWriter close begin.vecPrks.size=[%d],vecPrkts.size=[%d]", vecPrks.size(), vecPrkts.size());
    rd_kafka_resp_err_t err;
   
    if (vecPrks.size() > 0)
    {
        auto iter=vecPrks.begin();
        for (;iter!=vecPrks.end(); iter++)
        {
            rd_kafka_flush(*iter, 10*1000);
        }
        vecPrks.clear();
        //INFO_LOG("KafkaWriter wait for all produce finished OK!");
    }

    if (vecPrkts.size() > 0)
    {
        auto iter=vecPrkts.begin();
        for (;iter!=vecPrkts.end(); iter++)
        {
            rd_kafka_topic_destroy(*iter);
        }
        vecPrkts.clear();
        //INFO_LOG("KafkaWriter close vecPrkts OK!");
    }

    if (vecPrks.size() > 0)
    {
        auto iter=vecPrks.begin();
        for (;iter!=vecPrks.end(); iter++)
        {
            rd_kafka_destroy(*iter); 
        }
        vecPrks.clear();
        //INFO_LOG("KafkaWriter close vecPrks OK!");
    }

    //INFO_LOG("KafkaWriter close OK.");
}

std::string KafkaWriter::toString()
{
    ostringstream oss;

    auto iter = this->kafka_params.begin();
    oss << "KafkaWriter params(" << this->kafka_params.size() 
        << "):[" << endl;
    for (;iter != this->kafka_params.end(); iter++)
    {
        oss << "{desc:[" << iter->desc 
            << "],broker:[" << iter->broker
            << "],group:[" << iter->group
            << "],topic:[" << iter->topic
            << "],msg_max_bytes:[" << iter->msg_max_bytes
            << "],thread_num:[" << iter->thread_num
            << "]}" << endl;
    }
    oss << "]" << endl;
    return oss.str();
}

void KafkaWriter::add_conf(kafka_params_t param)
{
    this->kafka_params.push_back(param);
}


int KafkaWriter::run()
{
    return init(this->kafka_params);
}

void KafkaWriter::setExitFlag(bool bFlag)
{
    KafkaWriter::bRunFlag = bFlag;
}

//int KafkaWriter::write_message(kafka_message_t& kafka_msg)
bool KafkaWriter::is_full(){
    if(_max_queue_size > 0 && getQueueSize() > _max_queue_size){
        return true;
    }

    return false;

}
int KafkaWriter::write_message(const std::string& kafka_msg)
{
   //printf("_max_queue_size is %d queue size is %d \n", _max_queue_size, getQueueSize());
   
   if(is_full()){
       return -1;
   }
    
    if (!message_queue.enqueue(kafka_msg)) {
        printf("KafkaWriter Push message to queue error, msg=[%s]", kafka_msg.c_str());
        return -1;
    }
    _size.fetch_add(1);

    ////Attr_Set(POP_KAFKA_MSG_FROM_QUEUE, 1);
    //DEBUG_LOG("KafkaWriter Push message to queue, After push queue size=[%d]", message_queue.size_approx());
    uint64_t num = msg_num++;
    if (num % 100 == 0) {
        //INFO_LOG("KafkaWriter Push message to queue, After push queue size=[%d]", message_queue.size_approx());
    }

    return 0;
}

void KafkaWriter::send_message_thread(KafkaWriter *pthis, rd_kafka_t* rk, rd_kafka_topic_t* rkt)
{
//    kafka_message_t kafka_message;
	std::string kafka_message;
    bool bGetFlag = false;
    int iRetryCnt = 0;
    string sMessage = "";
    while (bRunFlag)
    {
        bGetFlag = false;
        kafka_message.clear();
        iRetryCnt = 0;
        sMessage = "";
        //usleep(200000);
        {
           /* 
            std::unique_lock<std::mutex> lock(pthis->queue_mutex);
            if (pthis->message_queue.size() > 0)
            {
                bGetFlag = true;
                kafka_message = pthis->message_queue.front();
                pthis->message_queue.pop();
                ////DEBUG_LOG("KafkaWriter After pop queue size=[%d].read message=[%s]", 
                    pthis->message_queue.size(),kafka_message.c_str());
                ////Attr_Set(PUSH_KAFKA_MSG_TO_QUEUE, 1);
            }
            */
           if (pthis->message_queue.try_dequeue(kafka_message)) {
               bGetFlag = true;
               pthis->_size.fetch_sub(1);
               ////DEBUG_LOG("KafkaWriter After pop queue size=[%d].read message=[%s]", 
                    //pthis->message_queue.size_approx(),kafka_message.c_str());
           }
        }

        if (!bGetFlag)
        {
            rd_kafka_poll(rk, 0);
            //std::cout <<"no data in message queue "<< std::endl;
            //INFO_LOG("KafkaWriter No Message in queue,sleep 200ms to wait!");
            usleep(2000000);
            continue;
        }
        else
        {
            sMessage = kafka_message;            
        retry:
        
	     int key_idx = sMessage.find("message_key:") + 12;
	     int end_key_idx = sMessage.find_first_of(",");
         std::string message_key = sMessage.substr(key_idx, end_key_idx - key_idx);
	     int value_idx = sMessage.find("message_value:") + 14;
	     std::string message_value =  sMessage.substr(value_idx);
         
     
             //DEBUG_LOG("Message Key:%s, Message Value size:%d", message_key.c_str(), message_value.length());
	    /*Send/Produce message.
            这是一个异步调用，在成功的情况下，只会将消息排入内部producer队列，
            对broker的实际传递尝试由后台线程处理，之前注册的传递回调函数(dr_msg_cb)
            用于在消息传递成功或失败时向应用程序发回信号*/
            if (rd_kafka_produce(rkt, RD_KAFKA_PARTITION_UA, RD_KAFKA_MSG_F_COPY, (void *)message_value.c_str(), message_value.length(), (void*)message_key.c_str(), message_key.length(), NULL) == -1)            
            {
                /*fprintf(stderr, 
                    "%% Failed to produce to topic %s: %s\n", 
                    rd_kafka_topic_name(rkt),
                    rd_kafka_err2str(rd_kafka_last_error()));*/
                printf("Failed to produce to topic %s: %s\n", rd_kafka_topic_name(rkt), rd_kafka_err2str(rd_kafka_last_error()));
    
                if (rd_kafka_last_error() == RD_KAFKA_RESP_ERR__QUEUE_FULL)
                {
                    /*如果内部队列满，等待消息传输完成并retry,
                    内部队列表示要发送的消息和已发送或失败的消息，
                    内部队列受限于queue.buffering.max.messages配置项*/
                    usleep(10000);
                    rd_kafka_poll(rk, pthis->timeout_ms);
                    if (iRetryCnt < 5)
                    {
                        iRetryCnt++;
                        //WARN_LOG("KafkaWriter Enqueued message failed due to queue full,retry at [%d] times.", iRetryCnt);
                        //goto retry;
                         printf("KafkaWriter Enqueued message failed After retry [%d] times.topic=[%s],error=[%s]", 
                            iRetryCnt, rd_kafka_topic_name(rkt), rd_kafka_err2str(rd_kafka_last_error()));
                        
                    }
                    else
                    {
                        printf("KafkaWriter Enqueued message failed After retry [%d] times.topic=[%s],error=[%s]", 
                            iRetryCnt, rd_kafka_topic_name(rkt), rd_kafka_err2str(rd_kafka_last_error()));
                    }
                    
                }
                else
                {
                    printf("KafkaWriter Enqueued message failed.topic=[%s],error=[%s]", 
                        rd_kafka_topic_name(rkt), rd_kafka_err2str(rd_kafka_last_error()));
                }
                
            }
            else
            {
                //DEBUG_LOG("KafkaWriter Enqueued message ok,topic=[%s],msg=[%s](len=[%d])", 
                    //rd_kafka_topic_name(rkt), sMessage.c_str(), sMessage.length());
            }
    
            /*producer应用程序应不断地通过以频繁的间隔调用rd_kafka_poll()来为
            传送报告队列提供服务。在没有生成消息以确定先前生成的消息已发送了其
            发送报告回调函数(和其他注册过的回调函数)期间，要确保rd_kafka_poll()
            仍然被调用*/
            rd_kafka_poll(rk, 0);
        }
    }
}

int KafkaWriter::init(const std::vector<kafka_params_t>& kafkas_params)
{
    vecPrks.clear();
    vecPrkts.clear();
    for (size_t i = 0; i < kafkas_params.size(); ++i)
    {
        uint32_t thread_num = kafkas_params[i].thread_num;
        if (thread_num > 20) {
            thread_num = 20;        // 最大20个线程
        }
        for (uint32_t j = 0; j < thread_num; ++j) {
            //INFO_LOG("init kafka client with broker=%s, group=%s, topic=%s, desc=%s, msg_max_bytes=%s, thread_pos=%d.",
                //kafkas_params[i].broker.c_str(), kafkas_params[i].group.c_str(), kafkas_params[i].topic.c_str(), 
                //kafkas_params[i].desc.c_str(),  kafkas_params[i].msg_max_bytes.c_str(), j);

            rd_kafka_t *rk = this->createServer(kafkas_params[i].broker.c_str(),kafkas_params[i].group.c_str(),
                                                kafkas_params[i].msg_max_bytes.c_str(), kafkas_params[i].mechanism.c_str(),
                                                kafka_params[i].user_name.c_str(), kafkas_params[i].password.c_str());
            if (NULL == rk)
            {
                ////Attr_Set(INIT_KAFKA_FAILED, 1);
                printf("init kafka client createServer failed, broker=%s, group=%s, topic=%s",
                    kafkas_params[i].broker.c_str(), kafkas_params[i].group.c_str(), kafkas_params[i].topic.c_str());
            }
            else
            {
                rd_kafka_topic_t* rkt = createTopic(rk, kafkas_params[i].topic.c_str());
                if (NULL == rkt)
                {
                    //Attr_Set(INIT_KAFKA_FAILED, 1);
                    printf("init kafka client createTopic failed, broker=%s, group=%s, topic=%s",
                        kafkas_params[i].broker.c_str(), kafkas_params[i].group.c_str(), kafkas_params[i].topic.c_str());
                }
                else
                {
                    vecPrks.push_back(rk);
                    vecPrkts.push_back(rkt);
                    std::thread td = std::thread(KafkaWriter::send_message_thread, this, rk, rkt);
                    td.detach();
                }
            }
        }
    }

    return 0;
}

KafkaReader::KafkaReader()
{
    timeout_ms = 5000;
}

KafkaReader::~KafkaReader()
{
    close();
}
void KafkaReader::setType(string sType)
{
    this->sType = sType;
}

rd_kafka_t* KafkaReader::createServer(const char *brokers, const char *group, 
const char* topic, const char *msg_max_bytes)
{
    printf("createServer reader");
    rd_kafka_conf_t *conf;
    rd_kafka_topic_conf_t *topic_conf;
    rd_kafka_resp_err_t err;
    char errstr[512];

    /* Kafka configuration */
    conf = rd_kafka_conf_new();

    //quick termination
    rd_kafka_conf_set(conf, "internal.termination.signal", std::to_string(SIGIO).c_str(), NULL, 0);

    /*设置消费者单条消息的最大长度*/
    if (NULL != msg_max_bytes && strcmp(msg_max_bytes, "\0") != 0) {
        memset(errstr, '\0', sizeof(errstr));
	    if (rd_kafka_conf_set(conf, "fetch.message.max.bytes", msg_max_bytes, errstr, sizeof(errstr)) != RD_KAFKA_CONF_OK)
        { 
            printf("createServer:rd_kafka_conf_set fetch.message.max.bytes failed! error=[%s] \n", errstr);
            return NULL;
	    }
    }

    //topic configuration
    topic_conf = rd_kafka_topic_conf_new();

    /* Consumer groups require a group id */
    if (group == NULL)
    {
        group = "test";
    }

    if (rd_kafka_conf_set(conf, "group.id", group, errstr, sizeof(errstr)) != RD_KAFKA_CONF_OK)
    {
        printf("kafka set group id failed, error=%s \n", errstr);
        return NULL;
    }

    /* Consumer groups always use broker based offset storage */
    if (rd_kafka_topic_conf_set(topic_conf, "offset.store.method", "broker", errstr, sizeof(errstr)) != RD_KAFKA_CONF_OK)
    {
        printf("kafka set topic fauled, error=%s \n", errstr);
        return NULL;
    }

    /* Set default topic config for pattern-matched topics. */
    rd_kafka_conf_set_default_topic_conf(conf, topic_conf);

    //实例化一个顶级对象rd_kafka_t作为基础容器，提供全局配置和共享状态
    rd_kafka_t *rk = rd_kafka_new(RD_KAFKA_CONSUMER, conf, errstr, sizeof(errstr));
    if (rk == NULL)
    {
        printf("kafka create consumer faile, error=%s \n", errstr);
        return NULL;
    }

    //Librdkafka需要至少一个brokers的初始化list
    if (rd_kafka_brokers_add(rk, brokers) == 0)
    {
       printf("kafka add brokers failed, brokers=%s \n", brokers);
       return NULL;
    }

    //重定向 rd_kafka_poll()队列到consumer_poll()队列
    rd_kafka_poll_set_consumer(rk);

    //创建一个Topic+Partition的存储空间(list/vector)
    pTopics = rd_kafka_topic_partition_list_new(1);
    //把Topic+Partition加入list
    rd_kafka_topic_partition_list_add(pTopics, topic, -1);
    //开启consumer订阅，匹配的topic将被添加到订阅列表中
    if ((err = rd_kafka_subscribe(rk, pTopics)))
    {
       printf("kafka start consuming topic failed, error=%s \n", rd_kafka_err2str(err));
       return NULL;
    }

    return rk;
}

void KafkaReader::close()
{
    KafkaReader::setExitFlag(false);
    ////INFO_LOG("KafkaReader close begin.vecPrks.size=[%d]", vecPrks.size());
    if (vecPrks.size() > 0)
    {
        rd_kafka_resp_err_t err;
        auto iter=vecPrks.begin();
        for (;iter!=vecPrks.end(); iter++)
        {
            err = rd_kafka_consumer_close(*iter);
            if (err)
            {
                printf("Failed to close consumer: %s \n", rd_kafka_err2str(err));
            }
            else
            {
                //INFO_LOG("Consumer closed");
            } 
            rd_kafka_destroy(*iter);
        } 
        vecPrks.clear();
        //INFO_LOG("KafkaClient close vecPrks OK!");
    }

    if (pTopics != NULL)
    {
        rd_kafka_topic_partition_list_destroy(pTopics);
        pTopics = NULL;
        //INFO_LOG("KafkaClient close Topics OK!");
    }
    //INFO_LOG("KafkaClient close OK.");
}

std::string KafkaReader::toString()
{
    ostringstream oss;

    auto iter = this->kafka_params.begin();
    oss << "KafkaReader params(" << this->kafka_params.size() 
        << "):[" << endl;
    for (;iter != this->kafka_params.end(); iter++)
    {
        oss << "{desc:[" << iter->desc 
            << "],broker:[" << iter->broker
            << "],group:[" << iter->group
            << "],topic:[" << iter->topic
            << "],msg_max_bytes:[" << iter->msg_max_bytes
            << "],thread_num:[" << iter->thread_num
            << "]}" << endl;
    }
    oss << "]" << endl;
    return oss.str();
}

void KafkaReader::add_conf(kafka_params_t param)
{
    this->kafka_params.push_back(param);
}

void KafkaReader::setExitFlag(bool bFlag)
{
    KafkaReader::bRunFlag = bFlag;
}

int KafkaReader::init(const std::vector<kafka_params_t>&  kafkas_params)
{
    vecPrks.clear();
    for (size_t i = 0; i < kafkas_params.size(); ++i)
    {
        //INFO_LOG("init KafkaReader client with broker=%s, group=%s, topic=%s, desc=%s, msg_max_bytes=%s, user_name=%s, password=%s.",
            //kafkas_params[i].broker.c_str(), kafkas_params[i].group.c_str(), kafkas_params[i].topic.c_str(), 
            //kafkas_params[i].desc.c_str(), kafkas_params[i].msg_max_bytes.c_str(), kafka_params[i].user_name.c_str(),
            //kafka_params[i].password.c_str());

        rd_kafka_t *rk = this->createServer(kafkas_params[i].broker.c_str(),kafkas_params[i].group.c_str(),
                                            kafkas_params[i].topic.c_str(), kafkas_params[i].msg_max_bytes.c_str());
        if (NULL == rk)
        {
            return -1;
            //Attr_Set(INIT_KAFKA_FAILED, 1);
            printf("init KafkaReader client createServer failed, broker=%s, group=%s, topic=%s",
                kafkas_params[i].broker.c_str(), kafkas_params[i].group.c_str(), kafkas_params[i].topic.c_str());
        }
        else
        {
            vecPrks.push_back(rk);
            std::thread td = std::thread(KafkaReader::read_message_thread, this, rk);
            td.detach();
        }
    }

    return 0;
}

int KafkaReader::read_message(std::string& sMsg)
{
    std::unique_lock<std::mutex> lock(queue_mutex); 
    if (message_queue.empty())
    {
        sMsg = "";
        //Attr_Set(KAFKA_QUEUE_EMPTY, 1);
        return -1;
    }
    else
    {
        sMsg = message_queue.front();
        message_queue.pop();

        //Attr_Set(POP_KAFKA_MSG_FROM_QUEUE, 1);
        //INFO_LOG("pick A message from KafkaReader queue, after pick queue size=%d",  message_queue.size());
    }

    return 0;
}

int KafkaReader::run()
{
    return init(this->kafka_params);
}

void KafkaReader::read_message_thread(KafkaReader *pthis, rd_kafka_t* rk)
{
    std::string sMsg;
    while (bRunFlag)
    {
        sMsg = "";
        rd_kafka_message_t *rkmessage = NULL; 
        if ((rkmessage = rd_kafka_consumer_poll(rk, pthis->timeout_ms)) == NULL)
        {
            sleep(1);
            continue;
        }

        if (rkmessage->err != 0)
        {
            //Attr_Set(READ_KAFKA_ERROR, 1);
            continue;
        }
        else
        {
            sMsg.assign((char *)rkmessage->payload, (int)rkmessage->len);
            rd_kafka_message_destroy(rkmessage);

            std::unique_lock<std::mutex> lock(pthis->queue_mutex);
            pthis->message_queue.push(sMsg);
            //DEBUG_LOG("KafkaReader read A message:[%s], push into queue, after push queue size=%d",
                //sMsg.c_str(), pthis->message_queue.size());
            //Attr_Set(PUSH_KAFKA_MSG_TO_QUEUE, 1);
        }
    }
}
