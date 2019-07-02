
#ifndef E4CLI_H
#define E4CLI_H

#define TOPICLEN_MAX 1025

typedef struct _e4client_tag
{
    MQTTClient Client;
    e4storage store;
    char current_topic[TOPICLEN_MAX];
} e4client;

#include "commands.h"
#include "mqtt.h"
#include "recv.h"

#endif
