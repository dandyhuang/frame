#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <hircluster.h>

int main(int argc, char **argv) {    
    int j=0;   
    redisClusterContext *cc;
    redisReply * reply = NULL;

    cc = redisClusterContextInit();
    redisClusterSetOptionAddNodes(cc, "127.0.0.1:7001,127.0.0.1:7002,127.0.0.1:7003");
    redisClusterSetOptionRouteUseSlots(cc);
    redisClusterConnect2(cc);
    if(cc == NULL || cc->err)
    {
        printf("connect error : %s\n", cc == NULL ? "NULL" : cc->errstr);
        return -1;
    }

    /* Set a key */
    reply = (redisReply*)redisClusterCommand(cc,"SET %s %s", "foo", "hello world");
    printf("SET: %s\n", reply->str);
    freeReplyObject(reply);

    /* Set a key using binary safe API */
    reply = (redisReply*)redisClusterCommand(cc,"SET %b %b", "bar", (size_t) 3, "hello", (size_t) 5);
    printf("SET (binary API): %s\n", reply->str);
    freeReplyObject(reply);

    /* Try a GET and two INCR */
    reply = (redisReply*)redisClusterCommand(cc,"GET foo");
    printf("GET foo: %s\n", reply->str);
    freeReplyObject(reply);

    reply = (redisReply*)redisClusterCommand(cc,"INCR counter");
    printf("INCR counter: %lld\n", reply->integer);
    freeReplyObject(reply);
    /* again ... */
    reply = (redisReply*)redisClusterCommand(cc,"INCR counter");
    printf("INCR counter: %lld\n", reply->integer);
    freeReplyObject(reply);

    /* Create a list of numbers, from 0 to 9 */
    reply = (redisReply*)redisClusterCommand(cc,"DEL mylist");
    freeReplyObject(reply);
    for (j = 0; j < 10; j++) {
        char buf[64];

        snprintf(buf,64,"%d",j);
        reply = (redisReply*)redisClusterCommand(cc,"LPUSH mylist element-%s", buf);
        freeReplyObject(reply);
    }

    /* Let's check what we have inside the list */
    reply = (redisReply*)redisClusterCommand(cc,"LRANGE mylist 0 -1");
    if (reply->type == REDIS_REPLY_ARRAY) {
        for (j = 0; j < reply->elements; j++) {
            printf("%u) %s\n", j, reply->element[j]->str);
        }
    }
    freeReplyObject(reply);

    /* Disconnects and frees the context */
    redisClusterFree(cc);

    return 0;
}
