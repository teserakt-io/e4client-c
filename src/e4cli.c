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
#include "e4/strlcpy.h"
#include "e4/util.h"

/* local header includes */
#include "e4cli.h"
#include "mqtt.h"

const char cli_commands[] =
"e4cli (c) 2018-2019 Teserakt AG, All Rights Reserved.\n"
"\n"
"E4CLI takes the following commands\n"
"\n"
"The following commands can be used:\n"
"\n"
"    !setid                      - set the device ID\n"
"    !setkey <hex>               - set the device key (raw, use hex)\n"
"    !setpwd <pwd>               - set the device password (derives key)\n"
"    !genkey                     - generate key\n"
"    !settopickey <hex>          - sets a topic key\n"
"    !s, !subscribe <topic>      - subscribe to topic\n"
"    !u, !unsubscribe <topic>    - unsubscribe from topic\n"
"    !c, !changetopic <topic>    - set output filter to a given topic\n"
"    !e, !e4msg <topic> <msg>    - send E4 protected message msg on topic\n"
"    !m, !clearmsg <topic> <msg> - send clear messgae msg on topic\n"
"    !l, !list\n"
"    !z, !zero\n"
"    !q, !quit\n"
"";

const char version[] =
"E4CLI E4 Command Line Client C Version X.Y.Z (...)"
"Copyright (c) 2018-2019 Teserakt AG, Switzerland. https://www.teserakt.io/"
"";
typedef enum _clientidtype_tag
{
    UNKNOWN,
    ID,
    ALIAS
} clientidtype;

// TODO: unglobal this.
e4client client;

void printhelp() { printf("%s\n", cli_commands); }

int argparse(char *filestore,
             const size_t fslen,
             char *broker,
             const size_t brokerlen,
             char *clientx,
             const size_t clidlen,
             clientidtype *idtype,
             int *help,
             int argc,
             char **argv)
{


    int validargs = 0;
    int clientset = 0;
    int i = 1;
    while (i < argc)
    {
        int step = 1;

        char *arg = argv[i];
        size_t arglen = strlen(arg);

        if (arglen >= 2 && strncmp(arg, "-", 1) == 0)
        {

            // TODO: there is almost certainly a better way of doing this
            // however we must be careful not to have any dependency on
            // libraries we cannot easily run on devices.
            char *argname = NULL;
            char argletter = arg[1];
            if (argletter == '-')
            {
                argname = &arg[2];
            }

            if (argletter == 'f' || strncmp(argname, "filestore", 9) == 0)
            {
                if (i + 1 < argc)
                {
                    size_t pathlen = 0;
                    size_t bytesparsed = 0;
                    char *path = NULL;
                    step += 1;
                    path = argv[i + 1];
                    pathlen = strlen(path);

                    bytesparsed = strlcpy(filestore, path, fslen);
                    if (bytesparsed >= pathlen)
                    {
                        printf("filestore: invalid parameter");
                        validargs = 1;
                        break;
                    }
                }
                else
                {
                    printf("filestore: no filename specified");
                    validargs = 1;
                    break;
                }
            }
            else if (argletter == 'i' || strncmp(argname, "clientid", 8) == 0)
            {
                if (i + 1 < argc)
                {
                    size_t clientlen = 0;
                    size_t bytesparsed = 0;
                    char *client = NULL;
                    step += 1;
                    client = argv[i + 1];
                    clientlen = strlen(client);

                    bytesparsed = strlcpy(clientx, client, clidlen);
                    if (bytesparsed >= clientlen)
                    {
                        printf("clientid: invalid parameter");
                        validargs = 1;
                        break;
                    }
                    if (clientset == 1)
                    {
                        printf("clientid: clientalias already passed");
                        validargs = 1;
                        break;
                    }
                    clientset = 1;
                }
                else
                {
                    printf("clientid: no filename specified");
                    validargs = 1;
                    break;
                }
            }
            else if (argletter == 'a' || strncmp(argname, "clientalias", 8) == 0)
            {
                if (i + 1 < argc)
                {
                    size_t clientlen = 0;
                    size_t bytesparsed = 0;
                    char *client = NULL;
                    step += 1;
                    client = argv[i + 1];
                    clientlen = strlen(client);

                    bytesparsed = strlcpy(clientx, client, clidlen);
                    if (bytesparsed >= clientlen)
                    {
                        printf("clientid: invalid parameter");
                        validargs = 1;
                        break;
                    }
                    if (clientset == 1)
                    {
                        printf("clientid: clientalias already passed");
                        validargs = 1;
                        break;
                    }
                }
                else
                {
                    printf("clientid: no filename specified");
                    validargs = 1;
                    break;
                }
            }
            else if (argletter == 'b' || strncmp(argname, "broker", 8) == 0)
            {
                if (i + 1 < argc)
                {
                    size_t brokerplen = 0;
                    size_t bytesparsed = 0;
                    char *brokerp = NULL;
                    step += 1;
                    brokerp = argv[i + 1];
                    brokerplen = strlen(brokerp);

                    bytesparsed = strlcpy(broker, brokerp, brokerplen);
                    if (bytesparsed >= brokerplen)
                    {
                        printf("broker: invalid parameter");
                        validargs = 1;
                        break;
                    }
                }
                else
                {
                    printf("broker: no filename specified");
                    validargs = 1;
                    break;
                }
            }
            else if (argletter == 'h' || strncmp(argname, "help", 4) == 0)
            {
                *help = 1;
            }
            else
            {
                printf("argparse: unknown argument %s\n", arg);
                validargs = 1;
                break;
            }
        }
        else
        {
            printf("Unknown argument: %s\n", arg);
            validargs = 1;
            break;
        }


        i += step;
    }

    return validargs;
}


int main(int argc, char **argv)
{
    clientidtype idtype;
    e4client e4client;
    char filestore[256];
    char clientid[256];
    char broker[256];
    //char e4cmdtopic[256];
    int helpflag = 0;

    memset(filestore, 0, sizeof(filestore));
    memset(clientid, 0, sizeof(clientid));
    memset(broker, 0, sizeof(broker));

    /*int argparse(char *filestore,
             const size_t fslen,
             char *broker,
             const size_t brokerlen,
             char *clientx,
             const size_t clidlen,
             enum idtype *idtype,
             int *help,
             int argc,
             char **argv)
    */
    if (argparse(filestore, 
                 sizeof(filestore), 
                 broker,
                 sizeof(broker),
                 clientid, 
                 sizeof(clientid),
                 &idtype, 
                 &helpflag, 
                 argc, 
                 argv) != 0)
    {
        printf("Invalid command line arguments. Call with --help for more.\n");
        return 1;
    }

    if (helpflag)
    {
        printhelp();
        return 0;
    }

    /* now we have parsed the arguments, we need to decide what to do. Our
     * options are:
     * 1. A filepath was passed. If this is the case we can load the
     * clientid and key material directly from storage.
     * 2. A filepath was not passed. Everything must be loaded from the
     * command line
     *
     * In a real device, storage would be pre-populated (it does not have
     * to comply with the e4storage definition given in the c library -
     * any storage that implements this interface is suitable).
     */

    e4c_init(&e4client.store);

    // filepath not passed. This is fine, let's decode what we can.
    if (strlen(filestore) == 0)
    {

        switch (idtype)
        {
        case ID:
        {
            // decode hex into -> e4client->clientid.
            if (strlen(clientid) != 2 * E4_ID_LEN)
            {
                printf("Invalid Client ID Length.\n");
                return 1;
            }
            // TODO:decode hex to bytes.
            e4c_hex_decode(e4client.clientid, sizeof(e4client.clientid),
                           clientid, strlen(clientid));
        }
        case ALIAS:
        {
            char hexid[E4_ID_LEN * 2];
            memset(hexid, 0, E4_ID_LEN * 2);

            e4c_derive_clientid(hexid, E4_ID_LEN*2, clientid, strlen(clientid));
        }
        case UNKNOWN:
        default:
            printf("Internal error!\n");
            goto exit;
        }

        strlcpy(e4client.clientid, clientid, sizeof(e4client.clientid));
    }
    else
    {
        // we _do_ have a filepath. let's try to sync data from it.

        if (e4c_load(&e4client.store, filestore) != 0)
        {
            printf("Unable to load data from %s.\n", filestore);
            return 1;
        }
    }

    /* from here, we are NOT running in help mode and do have a clientid
     * and broker, albeit possibly not valid */

    /* let's set up the broer */
    if (mqtt_init(&e4client, broker) != 0)
    {
        printf("Failed to connect to broker. Exiting.\n");
        goto exit;
    };

    /* and now we can run the repl: */
    repl(&e4client);

    /* shut down the mqtt client */
    mqtt_deinit(&e4client);
exit:
    return 0;
}


void dump_hex(const void *data, size_t len)
{
    size_t i, j;
    uint8_t ch;

    for (i = 0; i < len; i += 16)
    {

        printf("%03X ", (int)i);

        for (j = 0; j < 16; j++)
        {
            if (i + j >= len)
            {
                putchar(' ');
            }
            else
            {
                ch = ((const uint8_t *)data)[i + j];
                if (ch >= 32 && ch < 127)
                    putchar(ch);
                else
                    putchar('.');
            }
        }
        printf("  ");
        for (j = i; j < i + 16 && j < len; j++)
        {
            ch = ((const uint8_t *)data)[j];
            printf(" %02X", ch);
        }
        printf("\n");
    }
}
