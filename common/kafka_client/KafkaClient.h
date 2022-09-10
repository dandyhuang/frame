#ifndef READ_KAFKA_H_
#define READ_KAFKA_H_
#include <iostream>
#include <sstream>
#include <string>
#include <mutex>
#include <queue>
#include <thread>
#include <cstdlib>
#include <syslog.h>
#include <signal.h>
#include <error.h>
#include <unistd.h>
#include "thirdparty/rdkafka/include/rdkafka.h"
#include "concurrentqueue.h"

//#include "log_util.h"
//#include "xml_util.h"
//#include "push_monitor.h"

//#define _TEST_ atoi(DandyhuangTinyxml::getValueFromXmlFile("test_flag").c_str())

using namespace std;


typedef struct kafka_params_s
{
    kafka_params_s()
    {
    }
    kafka_params_s(const std::string &broker, const std::string &group, const std::string& topic,
        const std::string &desc = "", const std::string &msg_max_bytes = "1000000", uint32_t thread_num = 10)
    {
        this->broker = broker;
        this->group = group;
        this->topic = topic;
        this->desc = desc;
        this->msg_max_bytes = msg_max_bytes;
        this->thread_num = thread_num;
    }

    std::string broker;
    std::string group;
    std::string topic;
    std::string desc;
    std::string msg_max_bytes;
    std::string mechanism;
    std::string user_name;
    std::string password;
    uint32_t thread_num;

} kafka_params_t;


class KafkaWriter
{
public:
    KafkaWriter();
    ~KafkaWriter();

    rd_kafka_t* createServer(const char *brokers, const char *group, const char *msg_max_bytes="1000000",
                             const char *mechanism = "", const char *user_name = "", const char* password = "");
    rd_kafka_topic_t* createTopic(/*const */rd_kafka_t* rk,const char *topic);
    void close();
    std::string toString();
    void add_conf(kafka_params_t param);
    static void setExitFlag(bool bFlag);
    int init(const std::vector<kafka_params_t>&  kafka_params);
    int run();
    //int write_message(kafka_message_t& kafka_msg);
    int write_message(const std::string& kafka_msg);
    int getQueueSize(){ return _size.load();}
    void set_max_queue_size(int max_queue_size){_max_queue_size = max_queue_size;}
    bool is_full();
protected:
    static void send_message_thread(KafkaWriter *pthis, rd_kafka_t* rk, rd_kafka_topic_t* rkt);
    static void dr_msg_cb(rd_kafka_t *rk, const rd_kafka_message_t *rkmessage, void *opaque);
private:
    static bool bRunFlag;
    uint32_t timeout_ms;
    std::mutex queue_mutex;
    std::vector<kafka_params_t> kafka_params;
    std::vector<rd_kafka_t*> vecPrks;
    std::vector<rd_kafka_topic_t*> vecPrkts;
    std::atomic<int> _size{0};

    moodycamel::ConcurrentQueue<std::string> message_queue;
    volatile uint64_t msg_num;
    int _max_queue_size;

};

class KafkaReader
{
public:
    KafkaReader();
    ~KafkaReader();
    void setType(string sType);
    rd_kafka_t* createServer(const char *brokers, const char *group,
        const char* topic, const char *msg_max_bytes="1000000");
    void close();
    std::string toString();
    void add_conf(kafka_params_t param);
    static void setExitFlag(bool bFlag);
    int init(const std::vector<kafka_params_t>&  kafkas_params);
    int read_message(std::string& sMsg);
    int run();
    int getQueueSize(){ message_queue.size();}
protected:
    static void read_message_thread(KafkaReader *pthis, rd_kafka_t* rk);
private:
    static bool bRunFlag;
    uint32_t timeout_ms;
    std::mutex queue_mutex;
    string sType;//0-feed_user; 1-feed_news;
    std::vector<kafka_params_t> kafka_params;
    std::vector<rd_kafka_t*> vecPrks;
    rd_kafka_topic_partition_list_t *pTopics;
    std::queue<std::string> message_queue;
};

#endif

