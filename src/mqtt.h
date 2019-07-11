

#ifndef E4MQTT_H
#define E4MQTT_H

void mqtt_init(e4client* client, const char *broker);
void mqtt_msg_delivery(void *context, MQTTClient_deliveryToken dt);
void mqtt_conn_lost(void *context, char *cause);
int mqtt_msg_publish(e4client* client, const uint8_t *payload, size_t len, const char *topic);
int mqtt_msg_recvd(void *context, char *topicName, int topicLen, MQTTClient_message *message);


#endif
