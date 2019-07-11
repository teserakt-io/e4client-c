/* E4 Client Code in C

  (C) 2018-2019 Copyright Teserakt AG, Lausanne, Switzerland.

  2018-05-01  Markku-Juhani O. Saarinen <markku@teserakt.io
  2019-01-01  Antony Vennard <antony@teserakt.io>
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "MQTTClient.h"

/* Use the file storage command */
#define E4_STORE_FILE
#include "e4/e4.h"
#include "e4cli.h"

void mqtt_init(e4client* client, const char *broker)
{

    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;

    MQTTClient_setCallbacks(client->mqttclient, (void*)client, mqtt_conn_lost, mqtt_msg_recvd, mqtt_msg_delivery);
    MQTTClient_create(client->mqttclient, broker, clientid, MQTTCLIENT_PERSISTENCE_NONE, NULL);
}

void mqtt_msg_delivery(void *context, MQTTClient_deliveryToken dt)
{
    e4client* client = (e4client*) context;
    client->deliveredtoken = dt;
}

void mqtt_conn_lost(void *context, char *cause)
{
    printf("\nConnection lost: %s\n", cause);
}

int mqtt_msg_publish(e4client* client, const uint8_t *payload, size_t len, const char *topic)
{
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;

    int r = 0;
    size_t blen = 0;
    uint8_t buf[1024];

    pubmsg.qos = CLIC_QOS;
    pubmsg.retained = 0;
    client->deliveredtoken = 0;

    r = e4c_protect_message(buf, sizeof(buf), &blen, payload, len, topic, &client->store);

    if (r == 0)
    {
        pubmsg.payload = buf;
        pubmsg.payloadlen = blen;
        printf(">>> (ok) %s:\n", topic);
    }
    else
    {
        pubmsg.payload = (char *)payload;
        pubmsg.payloadlen = len;
        printf(">>> (!%d) %s:\n", r, topic);
    }
    dump_hex(pubmsg.payload, pubmsg.payloadlen);

    MQTTClient_publishMessage(client->mqttclient, topic, &pubmsg, &token);

    //  Blocks on QoS = 0
    //  while(deliveredtoken != token)
    //      ;

    return r;
}

// message received callback

int mqtt_msg_recvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    e4client* client = (e4client*) context;
    // Pass the message into a receive function that can be used with
    // any protocol
    recv_message(client, topicName, message->payload, message->payloadlen);

    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

// e4clic main()
