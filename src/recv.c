
#include "MQTTClient.h"

/* Use the file storage command */
#define E4_STORE_FILE
#include "e4/e4.h"
#include "e4/strlcpy.h"
#include "e4/util.h"

/* local header includes */
#include "e4cli.h"

/*
 * recv_message - the purpose of this function is to
 *
 *
 *
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
    r = e4c_unprotect_message(buf, sizeof(buf) - 1, &len, payload, payload_len,
                              topic_name, client);

    switch (r)
    {
    case E4_ERROR_OK_CONTROL:
        /* your code can do nothing here. You should not rely on the
         * buffer containing anything usable for your application.
         */
        printf("Control message received.\n");
        /* Don't do this in your code; it exists for demo purposes only: */
        break;
    case E4_ERROR_OK:
        buf[len] = 0;
        printf("<<< () %s:\n", topicName);
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
        printf("<<< (!%d) %s:\n", r, topicName);
    default:

        break;
    };

    return;
}
