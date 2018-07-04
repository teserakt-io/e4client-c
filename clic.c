// 	clic.c
// 	2018-05-01  Markku-Juhani O. Saarinen <markku@teserakt.io
//	(c) 2018 Copyright Teserakt AG

// 	An interactive "shell" to test E4 clients. May be used for scripting.
// 	Uses Paho MQTT C Client Library https://www.eclipse.org/paho/clients/c/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "MQTTClient.h"
#include "e4client.h"
#include "sha3.h"

#define CLIC_QOS	0

const char clic_usage[] =
"--- e4clic has single-letter commands prefixed with '!'\n"
"--- <message>           Broadcast message to current topic.\n"
"--- !t <topic>          Change current topic.\n"
"--- !s <topics>         Subscribe (and change current) to topics.\n"
"--- !u <topics>         Unsubscribe from topics.\n"
"--- !z <clients>        Locally zeroise (set to default) Client's IdKey.\n"
"--- !0 <clients>        Send RemoveTopic(current) command.\n"
"--- !1 <clients>        Send ResetTopics() command.\n"
"--- !2 <key> <clients>  Send SetIdKey(key) command.\n"
"--- !3 <key> <clients>  Send SetTopicKey(current, key).\n"
"--- !l                  Dump all topic keys.\n"
"--- !x  or  !q          Disconnect and quit.\n";


// dump a message on stdout

void dump_hex(const void *data, size_t len)
{
	size_t i, j;
	uint8_t ch;

	for (i = 0; i < len; i += 16) {

		printf("%03X ", (int) i);

		for (j = 0; j < 16; j++) {
			if (i + j >= len) {
				putchar(' ');
			} else {
				ch = ((const uint8_t *) data)[i + j];
				if (ch >= 32 && ch < 127)
					putchar(ch);
				else
					putchar('.');
			}
		}
		printf("  ");
		for (j = i; j < i + 16 && j < len; j++) {
			ch = ((const uint8_t *) data)[j];
			printf(" %02X", ch);
		}
		printf("\n");
	}
}

// delivery note for higher QoS

volatile MQTTClient_deliveryToken deliveredtoken;

void msg_delivery(void *context, MQTTClient_deliveryToken dt)
{
	deliveredtoken = dt;
}

void conn_lost(void *context, char *cause)
{
	printf("\nConnection lost: %s\n", cause);
}

// protect and publish a simple text message

int msg_publish(MQTTClient client,
	const uint8_t *payload, size_t len, const char *topic)
{
	MQTTClient_message pubmsg = MQTTClient_message_initializer;
	MQTTClient_deliveryToken token;

	int r = 0;
	size_t blen = 0;
	uint8_t buf[1024];

	pubmsg.qos = CLIC_QOS;
	pubmsg.retained = 0;
	deliveredtoken = 0;

	r = e4c_protect_message(buf, sizeof(buf), &blen, payload, len, topic);

	if (r == 0) {
		pubmsg.payload = buf;
		pubmsg.payloadlen = blen;
		printf(">>> (ok) %s:\n", topic);
	} else {
		pubmsg.payload = (char *) payload;
		pubmsg.payloadlen = len;
		printf(">>> (!%d) %s:\n", r, topic);
	}
	dump_hex(pubmsg.payload, pubmsg.payloadlen);

	MQTTClient_publishMessage(client, topic, &pubmsg, &token);

//	Blocks on QoS = 0
//	while(deliveredtoken != token)
//		;

	return r;
}

// message received callback

int msg_recvd(void *context, char *topicName,
	int topicLen, MQTTClient_message *message)
{
	int r = 0;
	size_t len = 0;
	uint8_t buf[1024];

	r = e4c_unprotect_message(buf, sizeof(buf) - 1, &len,
		message->payload, message->payloadlen, topicName);

	if (r == 0) {
		buf[len] = 0;
		printf("<<< (ok) %s:\n", topicName);
		dump_hex(buf, len);
	} else {
		printf("<<< (!%d) %s:\n", r, topicName);
		dump_hex(message->payload, message->payloadlen);
	}

	MQTTClient_freeMessage(&message);
	MQTTClient_free(topicName);
	return 1;
}

// generate an increasing sequence with time-dependant start byte


#define MAX_PARAM 16

int main(int argc, char* argv[])
{
	MQTTClient client;
	MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;

	uint8_t buf[256];
	uint8_t hash[E4C_TOPIC_LEN], key[E4C_KEY_LEN];
	char line[256];
	char e4topic[256];
	char cmdtopic[256];
	char topic[256] = "/discard";
	char *parm[MAX_PARAM];
	int parn;
	char *broker, *clientid;

	int i, j, l;

	// Do we have a broker on command line ?
	if (argc < 3) {
		printf("Usage: e4clic <Broker> <ClientId>\n");
		return -1;
	}
	broker = argv[1];
	clientid = argv[2];

	printf("!!! broker = %s\n", broker);
	printf("!!! my client id = %s\n", clientid);

	MQTTClient_create(&client, broker, clientid,
		MQTTCLIENT_PERSISTENCE_NONE, NULL);

	conn_opts.keepAliveInterval = 20;
	conn_opts.cleansession = 1;

	MQTTClient_setCallbacks(client, NULL, conn_lost, msg_recvd, msg_delivery);

	if (MQTTClient_connect(client, &conn_opts) != MQTTCLIENT_SUCCESS) {
		printf("!!! Failed to connect.\n");
		return -1;
	}

	// set the command topic and zeroize key 
	snprintf(cmdtopic, sizeof(cmdtopic), "e4/%s", clientid);
	memset(key, 0, E4C_KEY_LEN);
	sha3(cmdtopic, strlen(cmdtopic), hash, E4C_TOPIC_LEN);
	e4c_set_id_key(hash, key);

	// subscribe to what ever is the zero key
	MQTTClient_subscribe(client, cmdtopic , CLIC_QOS);
	printf("!!! command topic = %s\n", cmdtopic);

	while (fgets(line, sizeof(line), stdin) != NULL) {

		// remove trailing new line from fgets()
		l = strlen(line);
		if (l >= 1 && line[l - 1] == '\n') {
			line[--l] = 0;
		}
		if (l == 0 || line[0] == '?') {
			printf("%s", clic_usage);
			continue;
		}

		// publish something without parsing
		if (line[0] != '!') {
			msg_publish(client, (const uint8_t *) line, l, topic);
			continue;
		}

		// quitting
		if (line[1] == 'x' || line[1] == 'q')
			break;

		// generate a list of arguments
		parn = 0;
		for (i = 2; i < l; i++) {

			if (line[i] == ' ') {			// skip space
				line[i] = 0;				// make null terminated
				continue;
			}
			parm[parn++] = &line[i];
			if (parn >= MAX_PARAM)
				break;
			// include all non space
			while (i < l - 1 && line[i + 1] != ' ') {
				i++;
			}
		}

		switch (line[1]) {

			// change current topic
			case 't':
				if (parn != 1) {
					printf("??? need one topic name\n");
					break;
				}
				strncpy(topic, parm[0], sizeof(topic) - 1);
				printf("!!! Topic is %s\n", topic);
				break;

			// subscribe new topics
			case 's':
				if (parn < 1) {
					printf("??? need topic names\n");
					break;
				}
				for (i = 0; i < parn; i++) {
					MQTTClient_subscribe(client, parm[i], CLIC_QOS);
					strncpy(topic, parm[i], sizeof(topic) - 1);
					printf("!!! Subscribed to %s\n", parm[i]);
				}
				break;

			// unsubsribe from topics
			case 'u':
				if (parn < 1) {
					printf("??? need topic names\n");
					break;
				}
				for (i = 0; i < parn; i++) {
					MQTTClient_unsubscribe(client, parm[i]);
					printf("!!! Unsubscribed from %s\n", parm[i]);
				}
				break;

			// dump keys
			case 'l':
				e4c_debug_dumpkeys();
				break;

			// locally reset clientids to zero (default)
			case 'z':
				memset(buf, 0, E4C_KEY_LEN);
				for (i = 0; i < parn; i++) {
					snprintf(e4topic, sizeof(e4topic), "e4/%s", parm[i]);

					sha3(e4topic, strlen(e4topic), hash, E4C_TOPIC_LEN);
					e4c_set_topic_key(hash, buf);
				}
				break;

			// send remove topic command
			case '0':
				buf[0] = 0x00;				// RemoveTopic
					
				sha3(topic, strlen(topic), hash, E4C_TOPIC_LEN);
				memcpy(&buf[1], hash, E4C_TOPIC_LEN);

				for (i = 0; i < parn; i++) {
					snprintf(e4topic, sizeof(e4topic), "e4/%s", parm[i]);
					printf("!!! RemoveTopic(%s) -> %s\n", topic, e4topic);
					msg_publish(client, buf, 1 + E4C_TOPIC_LEN, e4topic);
				}
				e4c_remove_topic(hash);		// do it locally

				break;


			// send reset topics command
			case '1':
				if (parn < 1) {
					printf("??? need clients\n");
					break;
				}
				buf[0] = 0x01;				// ResetTopics
				for (i = 0; i < parn; i++) {
					snprintf(e4topic, sizeof(e4topic), "e4/%s", parm[i]);
					printf("!!! ResetTopics() -> %s\n", e4topic);
					msg_publish(client, buf, 1, e4topic);
				}
				break;

			// send change client id key commands
			case '2':
				if (parn < 2) {
					printf("??? need a key and client(s)\n");
					break;
				}

				// hash the input key
				sha3(parm[0], strlen(parm[0]), key, E4C_KEY_LEN);

				for (i = 1; i < parn; i++) {

					// construct message
					snprintf(e4topic, sizeof(e4topic), "e4/%s", parm[i]);
					buf[0] = 0x02;			// SetIdKey
					memcpy(buf + 1, key, E4C_KEY_LEN);
					printf("!!! SetIdKey(");
					for (j = 0; j < E4C_KEY_LEN; j++)
						printf("%02X", buf[j + 1] & 0xFF);
					printf(") -> %s\n", e4topic);
					msg_publish(client, buf, E4C_KEY_LEN + 1, e4topic);

					// store locally
					sha3(e4topic, strlen(e4topic), hash, E4C_TOPIC_LEN);
					e4c_set_topic_key(hash, buf + 1);
				}
				break;

			// send change topic key command to client
			case '3':
				if (parn < 2) {
					printf("??? need a key and client(s)\n");
					break;
				}

				// hash the input key
				sha3(parm[0], strlen(parm[0]), key, E4C_KEY_LEN);

				buf[0] = 0x03;				// SetTopicKey

				memcpy(buf + 1, key, E4C_KEY_LEN);

				sha3(topic, strlen(topic), hash, E4C_TOPIC_LEN);
				memcpy(buf + E4C_KEY_LEN + 1, hash, E4C_TOPIC_LEN);

				// store locally
				e4c_set_topic_key(hash, key);

				// send same message to listed clients
				for (i = 1; i < parn; i++) {
					snprintf(e4topic, sizeof(e4topic), "e4/%s", parm[i]);
					printf("!!! SetTopicKey(%s, ", topic);
					for (j = 0; j < E4C_KEY_LEN; j++)
						printf("%02X", buf[j + 1] & 0xFF);
					printf(") -> %s\n", e4topic);
					msg_publish(client, buf, 
						1 + E4C_KEY_LEN + E4C_TOPIC_LEN, e4topic);
				}
				break;

			default:
				printf("??? command '%c' not known.\n%s",
					line[0], clic_usage);
		}
	};
	printf("!!! Disconnecting.\n");

	MQTTClient_disconnect(client, 10000);
	MQTTClient_destroy(&client);

	return 0;
}

