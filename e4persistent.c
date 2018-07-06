//  e4cersistent.c
//  2018-07-06  Markku-Juhani O. Saarinen <markku@teserakt.io>

//  (c) 2018 Copyright Teserakt AG

//  Persistent key storage.

#include <string.h>

#ifdef __AVR__
#define E4C_TOPICS_MAX 5
#else
#include <stdio.h>
#include <sys/mman.h>
#define E4C_TOPICS_MAX 100
#endif

#include "e4client.h"
#include "e4persistent.h"
#include "sha3.h"

// number of topic keys
static int topic_keys_no = 0;

// This is the topic-key structure

typedef struct {
    uint8_t topic[E4C_TOPIC_LEN];
    uint8_t key[E4C_KEY_LEN];
} topic_key_t;

topic_key_t topic_keys[E4C_TOPICS_MAX];


// Initialize and check if persistent storage is valid. The path is 
// optional -- default filename is used if set to NULL. Ignored with EEPROM.

int e4c_init(const char *path)
{
    topic_keys_no = 0;
    return 0;
}

// Free all resources

int e4c_free()
{
    return 0;
}

// Fetch an index of a key of given hash. The command channel is 0.
// Returns a negative on failure.

int e4c_getindex(const char *topic)
{
    int i;
    uint8_t hash[E4C_TOPIC_LEN];

    // hash the topic
    sha3(topic, strlen(topic), hash, E4C_TOPIC_LEN);

    // look for it
    for (i = 0; i < topic_keys_no; i++) {   // find the key
        if (memcmp(topic_keys[i].topic, hash, E4C_TOPIC_LEN) == 0) {
            break;
        }
    }
    if (i >= topic_keys_no)
        return E4ERR_TopicKeyMissing;

    return i;
}

// get a key by index

int e4c_getkey(uint8_t * key, int index)
{
    if (index < 0 || index >= topic_keys_no)
        return E4ERR_TopicKeyMissing;

    memcpy(key, topic_keys[index].key, E4C_KEY_LEN);

    return 0;
}

// Remove topic (hash)

int e4c_remove_topic(const uint8_t *topic_hash)
{
    int i, j;

    for (i = 0; i < topic_keys_no; i++) {
        if (memcmp(topic_keys[i].topic, topic_hash, E4C_TOPIC_LEN) == 0) {
            // remove this item and move list up
            for (j = i + 1; j < topic_keys_no; j++) {
                memcpy(&topic_keys[j - 1], &topic_keys[j],
                    sizeof(topic_key_t));
            }
            topic_keys_no--;
            return 0;
        }
    }

    return E4ERR_TopicKeyMissing;
}

// Clear all topics except ID key

int e4c_reset_topics()
{
    if (topic_keys_no < 1)
        return E4ERR_TopicKeyMissing;
    topic_keys_no = 1;

    return 0;
}

// set id key for this instance index 0

int e4c_set_id_key(const uint8_t *cmd_topic_hash, const uint8_t *key)
{
    if (cmd_topic_hash != NULL)
        memcpy(topic_keys[0].topic, cmd_topic_hash, E4C_TOPIC_LEN);
    if (key != NULL)    
        memcpy(topic_keys[0].key, key, E4C_KEY_LEN);

    if (cmd_topic_hash != NULL && key != NULL) {
        if (topic_keys_no < 1)
            topic_keys_no = 1;
    }

    return 0;
}

// set key for given topic (hash)

int e4c_set_topic_key(const uint8_t *topic_hash, const uint8_t *key)
{
    int i;

    for (i = 0; i < topic_keys_no; i++) {
        if (memcmp(topic_keys[i].topic, topic_hash, E4C_TOPIC_LEN) == 0)
            break;
    }
    if (i >= E4C_TOPICS_MAX)                // out of space
        return E4ERR_TopicKeyMissing;

    memcpy(topic_keys[i].topic, topic_hash, E4C_TOPIC_LEN);
    memcpy(topic_keys[i].key, key, E4C_KEY_LEN);

    if (i == topic_keys_no) {               // new topic
        topic_keys_no++;
    }

    return 0;
}

#ifndef __AVR__

void e4c_debug_dumpkeys()
{
    int i, j;

    for (i = 0; i < topic_keys_no; i++) {
        printf("!!! %2d topic hash = ", i);
        for (j = 0; j < E4C_TOPIC_LEN; j++)
            printf("%02X", topic_keys[i].topic[j]);
        printf("\n!!! %2d  topic key = ", i);
        for (j = 0; j < E4C_KEY_LEN; j++)
            printf("%02X", topic_keys[i].key[j]);
        printf("\n");
    }
}

#endif

