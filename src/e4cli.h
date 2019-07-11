
#ifndef E4CLI_H
#define E4CLI_H

#define TOPICLEN_MAX 1025

typedef struct _e4client_tag
{
    MQTTClient mqttclient;
    e4storage store;
    char current_topic[TOPICLEN_MAX];
    volatile MQTTClient_deliveryToken deliveredtoken;
} e4client;

void dump_hex(const void *data, size_t len);

#include "commands.h"
#include "mqtt.h"
#include "recv.h"

#endif
