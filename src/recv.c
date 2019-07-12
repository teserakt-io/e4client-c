
#include "MQTTClient.h"

/* Use the file storage command */
#define E4_STORE_FILE
#include "e4/e4.h"
#include "e4/strlcpy.h"
#include "e4/util.h"

/* local header includes */
#include "e4cli.h"

/*
 * recv_message - the purpose of this function is to demonstrate how the C
 * library should be used when receiving messages. You do not need to
 * write a separate function from your mqtt or other protocol receive handler
 * as in this code, this is simply for clarity
 *
 * The most important points are as follows:
 *
 * 1. The concept of topic can be anything that uniquely identifies a
 * communication. It does not need to be an MQTT topic as demonstrated here.
 * 2. There must be some way for E4 to identify its own messages. These will be
 *    received encrypted by your code and passed to the unprotect function. In
 *    these cases there is a deliberate result code, E4_RESULT_OK_CONTROL, that
 *    lets you know that E4 processing was OK, but that the message was an E4
 *    internal message and is not suitable for processing by your application.
 * 3. An E4_ERROR_TOPICKEY_MISSING message is raised whenever E4 does not have
 *    a key for a given topic. It is up to you to decide the policy of your
 *    application in this case: you can discard any message you receive that is
 *    not encrypted, or you may process it anyway.
 */
void recv_message(e4client *client, const char *topic_name, const char *payload, size_t payload_len)
{

    int r = 0;
    size_t len = 0;
    uint8_t buf[1025];

    /* zero the receive buffer */
    memset(buf, 0, sizeof buf);

    /* This is all you need to call in order to decrypt messages received
     * You can then decide based on the return code what sort of policy to
     * implement.
     * If an E4 control message is received, it is handled in this function
     * and the internal state of E4 is updated, represented by the e4storage
     * type.
     */
    r = e4c_unprotect_message(buf, sizeof(buf) - 1, &len, (uint8_t *)payload,
                              payload_len, topic_name, &client->store);

    switch (r)
    {
    case E4_RESULT_OK_CONTROL:
        /* your code can do nothing here. You should not rely on the
         * buffer containing anything usable for your application.
         */
        printf("Control message received.\n");
        /* Don't do this in your code; it exists for demo purposes only: */
        break;
    case E4_RESULT_OK:
        buf[len] = 0;
        printf(">E4 MSG Received On Topic=%s:\n", topic_name);
        dump_hex(buf, len);

        break;
    case E4_ERROR_TOPICKEY_MISSING:
        /* in this case, E4 found no topic key and has not touched the
         * resulting buffer. Two options are possible:
         *
         *  1) If this client is _supposed_ to be encrypted and has
         *     not got the topic key, the data will be scrambled.
         *  2) The topic may not be encrypted, in which case the
         *     data is available in its unencrypted form.
         *
         * E4 cannot by default distinguish between the two cases;
         * we leave what is to be done at this stage up to application
         * developers.
         */
        printf(">MSG Received on Topic %s, no key available.\n", topic_name);
    default:
        /* in this case E4 had an internal state error for some reason,
         * such as running out of internal storage space. */
        printf(">E4 MSG received on Topic %s, produced error code %d.\n", topic_name, r);
        break;
    };

    return;
}
