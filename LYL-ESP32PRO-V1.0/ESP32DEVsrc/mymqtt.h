#ifndef MYQTT_H
#define MYQTT_H
#include "esp_log.h"
#include "mqtt_client.h"
#include "string.h"


#define ONENET_PID    "83bcAx554p"
#define ONENET_DN     "Linux_my_client_start"
#define ONENET_TOKEN  "version=2018-10-31&res=products%2F83bcAx554p&et=1797929077&method=sha1&sign=GWTUG3SkNpqmWc1wsQmDtLJGCqw%3D"
#define MQTT_BROKER   "mqtt://mqtts.heclouds.com:1883" // 服务器地址+端口

// 订阅/发布主题
#define PUB_TOPIC "$sys/83bcAx554p/Linux_my_client_start/thing/property/set_reply"//回复主题
#define SUB_TOPIC "$sys/83bcAx554p/Linux_my_client_start/thing/property/set" //订阅主题

#define PUB_TOPIC1 "$sys/83bcAx554p/Linux_my_client_start/thing/property/post" //上报主题
#define MQTT_TAG "MQTT_CLIENT"

int mymqttconnect();
int mqtt_publish_data(const char* data, int len);
#endif