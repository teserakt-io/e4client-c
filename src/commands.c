
#include "MQTTClient.h"

/* Use the file storage command */
#define E4_STORE_FILE
#include "e4/e4.h"
#include "e4/strlcpy.h"
#include "e4/util.h"

/* local header includes */
#include "e4cli.h"


void client_setid(e4client *client, const char *arg)
{
    uint8_t id[E4_ID_LEN];
    size_t arglen = strlen(arg);
    int result = 0;

    memset(id, 0, sizeof id);

    if (arglen == 0 || arglen != E4_ID_LEN * 2)
    {
        printf("Expected client ID of length %d, not received.\n", E4_ID_LEN * 2);
        return;
    }

    result = e4c_hex_decode((char*)id, sizeof(id), (char*)arg, arglen);
    if (result == 0)
    {
        printf("Unable to decode ID: invalid data.\n");
        return;
    }

    e4c_set_id(&client->store, id);
    return;
}

void client_genkey(e4client* client, const char *arg)
{

    // do not generate but ask the C2 to do it?
    // can clients do this?
    // TODO: resolve above quesitons

    printf("ERROR: not implemented in this client.\n");
}


void client_setalias(e4client *client, const char *arg)
{
    char id[E4_ID_LEN] = { 0 };
    size_t arglen = strlen(arg);

    e4c_derive_clientid(id, sizeof id, arg, arglen);
    memcpy(client->store.id, id, E4_ID_LEN);
    e4c_sync(&client->store);
}

// sets the client key to a given value when supplied on the
// command line.
void client_setkey(e4client *client, const char *arg)
{
    unsigned char keybuffer[E4_KEY_LEN];
    size_t keylen = strlen(arg);
    if (keylen != E4_KEY_LEN * 2)
    {
        printf("ERROR: key length must be %d characters, encoded in hex.\n", E4_KEY_LEN * 2);
        return;
    }
    e4c_hex_decode((char*)keybuffer, E4_KEY_LEN, arg, keylen);
    e4c_set_idkey(&client->store, keybuffer);
}

void client_settopickey(e4client* client, char *arg)
{
    char keybuffer[E4_KEY_LEN];
    //char *topic = arg;
    char *key = arg;

    key = strchr(arg, ' ');
    if (key == NULL || key == arg)
    {
        printf("ERROR: must specify a topic and key in the format:\n");
        printf("       <topicname> <key>\n");
        printf("       key must be %d hexadecimal characters.", 2 * E4_KEY_LEN);
        return;
    }

    // tokenize the strings.
    key[0] = '\0';
    key += 1;

    size_t keylen = strlen(key);
    if (keylen != E4_KEY_LEN * 2)
    {
        printf("ERROR: key length must be %d characters, encoded in hex.\n", E4_KEY_LEN * 2);
        return;
    }
    e4c_hex_decode(keybuffer, E4_KEY_LEN, key, keylen);
    /*
    TODO: topichash.
    e4c_set_topic_key(&client->store, topichash, (const uint8_t*)keybuffer);
    */
}

void client_setpwd(e4client* client, const char *arg)
{
    printf("ERROR: The C client does not yet implement password-derivation.\n");
    printf("       Please use the setkey command.\n");
}


void client_subscribe(e4client* client, const char *arg)
{

    size_t arglen = strlen(arg);
    if (arglen == 0) 
    {
        printf("client_subscribe: Invalid topic");
        return;
    }

    MQTTClient_subscribe(client->mqttclient, arg, 0);
}


void client_unsubscribe(e4client* client, const char *arg)
{
    size_t arglen = strlen(arg);
    if (arglen == 0)
    {
        printf("client_subscribe: Invalid topic");
        return;
    }

    MQTTClient_unsubscribe(client->mqttclient, arg);
}

void client_changetopic(e4client* client, const char *arg) {}

void client_e4msg(e4client* client, const char *arg) {}

void client_clearmsg(e4client* client, char *arg) {

    char *topic = arg;
    char *msg = arg;

    msg = strchr(arg, ' ');
    if (msg == NULL || msg == arg)
    {
        printf("ERROR: must specify a topic and key in the format:\n");
        printf("       <topicname> <key>\n");
        printf("       key must be %d hexadecimal characters.", 2 * E4_KEY_LEN);
        return;
    }

    // tokenize the strings.
    msg[0] = '\0';
    msg += 1;

    size_t msglen = strlen(msg);

    mqtt_msg_publish(client->mqttclient, 
            (uint8_t*) msg, 
            msglen, 
            (const char*) topic);
}

void client_list(e4client* client, const char *arg) {}

void client_zero(e4client* client, const char *arg) {}


