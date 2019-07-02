
#ifndef RECV_H
#define RECV_H

void recv_message(e4client *client, const char *topic_name, const char *payload, size_t payload_len);

#endif
