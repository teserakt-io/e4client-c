/* E4 Client Code in C
  
  (C) 2018-2019 Copyright Teserakt AG, Lausanne, Switzerland.

  2018-05-01  Markku-Juhani O. Saarinen <markku@teserakt.io
  2019-01-01  Antony Vennard <antony@teserakt.io>
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "MQTTClient.h"

/* Use the file storage command */
#define E4_STORE_FILE
#include "e4/e4.h"

void mqtt_init(MQTTClient* client, const char* broker) {

    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;

    MQTTClient_setCallbacks(client, NULL, conn_lost, msg_recvd, msg_delivery);
    MQTTClient_create(client, broker, clientid,
        MQTTCLIENT_PERSISTENCE_NONE, NULL);
}
