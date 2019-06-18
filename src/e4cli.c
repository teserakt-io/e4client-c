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
#include "e4/strlcpy.h"

/* local header includes */
#include "e4cli.h"
#include "mqtt.h"

const char cli_commands[] = "e4cli (c) 2018-2019 Teserakt AG, All Rights Reserved."
" "
"E4CLI takes the following commands"
" "
"The following commands can be used:"
" "
"    !setid                      - set the device ID"
"    !setkey <hex>               - set the device key (raw, use hex)"
"    !setpwd <pwd>               - set the device password (derives key)"
"    !genkey                     - generate key"
"    !settopickey <hex>          - sets a topic key"
"    !s, !subscribe <topic>      - subscribe to topic"
"    !u, !unsubscribe <topic>    - unsubscribe from topic"
"    !c, !changetopic <topic>    - set output filter to a given topic"
"    !e, !e4msg <topic> <msg>    - send E4 protected message msg on topic"
"    !m, !clearmsg <topic> <msg> - send clear messgae msg on topic"
"    !l, !list"
"    !z, !zero"
"    !q, !quit"
"";

const char version[] = "E4CLI E4 Command Line Client C Version X.Y.Z (...)"
"Copyright (c) 2018-2019 Teserakt AG, Switzerland. https://www.teserakt.io/"
"";


// TODO: unglobal this.
e4client client;

void printhelp() {
    printf("%s\n", cli_commands);
}

void client_setid(e4client* client, const char* arg) {

    uint8_t id[E4_ID_LEN];
    size_t arglen = strlen(arg);
    int result = 0;

    memset(id, 0, sizeof id);

    if ( arglen == 0 || arglen != E4_ID_LEN*2 ) {
        printf("Expected client ID of length %d, not received.\n", E4_ID_LEN*2);
        return;
    }

    r = e4c_hex_decode(id, sizeof(id), arg, arglen);     
    if ( r == 0 ) {
        printf("Unable to decode ID: invalid data.\n");
        return;
    }

    client->store.e4c_set_id(client->store, id);
    return;
}

void client_setalias(e4client* client, const char* arg) {
    
    char id[E4_ID_LEN] = {0};

    
    e4c_derive_clientid(id, sizeof id, arg, arglen);
}

void client_setkey(e4client* client, const char* arg) {

}

void client_setpwd(const char* arg) {

}

void client_genkey(const char* arg) {
    
}

void client_settopickey(const char* arg) {
    
}

void client_subscribe(const char* arg) {
    
    size_t arglen = strlen(arg);
    if ( arglen == 0 )
    {
        printf("client_subscribe: Invalid topic");
        return;
    }

    MQTTClient_subscribe(handle, arg, 0);
}

void client_unsubscribe(const char* arg) {
    size_t arglen = strlen(arg);
    if ( arglen == 0 )
    {
        printf("client_subscribe: Invalid topic");
        return;
    }

    MQTTClient_unsubscribe(handle, arg);
}

void client_changetopic(const char* arg) {
    
}

void client_e4msg(const char* arg) {
    
}

void client_clearmsg(const char* arg) {
    
}

void client_list(const char* arg) {
    
}

void client_zero(const char* arg) {
    
}

void repl() {

    char line[256] = {0};
    int i = 0;

    while (fgets(line, sizeof(line), stdin) != NULL) {

        char* command = NULL;
        char* arg = NULL;

        int linelen = strlen(line);

        // user pressed enter/entered nothing except a newline.
        if (linelen == 1 ) {
            continue;
        }

        // remove trailing new line from fgets()
        if (linelen >= 1 && line[linelen-1] == '\n') {
            line[--linelen] = 0;
        }

        if (line[0] != '!')
        {
            printf("Unrecognized command, try !help\n");
            continue;
        }

        for (i=1; i < linelen; i++ ){
            if (i == linelen-1)  {
                command = &line[1];
                break;
            }
            if (line[i]==' ') {
                line[i] = 0;
                command = &line[1];
                // don't assign if i = linelen-1; potential buffer overflow.
                if ( i+1 < linelen ) {
                    arg = &line[i+1];
                }
                break;
            }
        }

        if ( command == NULL ) {
            // error, let's do something about it?
            continue;
        }

        if (strcmp(command, "setid") == 0) { 
            client_setid(arg);
        if (strcmp(command, "setalias") == 0) { 
            client_setid(arg);
        } else if (strcmp(command, "setkey") == 0) {
            client_setkey(arg);
        } else if (strcmp(command, "setpwd") == 0) {
            client_setpwd(arg);
        } else if (strcmp(command, "genkey") == 0) {
            client_genkey(arg);
        } else if (strcmp(command, "settopickey") == 0) {
            client_settopickey(arg);
        } else if (strcmp(command, "subscribe") == 0 || 
                   strcmp(command, "s") == 0) {
            client_subscribe(arg);
        } else if (strcmp(command, "unsubscribe") == 0 || 
                   strcmp(command, "u") == 0) {
            client_unsubscribe(arg);
        } else if (strcmp(command, "changetopic") == 0 || 
                   strcmp(command, "u") == 0) {
            client_changetopic(arg);
        } else if (strcmp(command, "e4msg") == 0 || 
                   strcmp(command, "e") == 0) {
            client_e4msg(arg);
        } else if (strcmp(command, "clearmsg") == 0 || 
                   strcmp(command, "m") == 0) {
            client_clearmsg(arg);
        } else if (strcmp(command, "list") == 0 || 
                   strcmp(command, "l") == 0) {
            client_list(arg);
        } else if (strcmp(command, "zero") == 0 || 
                   strcmp(command, "z") == 0) {
            client_zero(arg);
        } else if (strcmp(command, "help") == 0 || 
                   strcmp(command, "h") == 0) {
        } else if (strcmp(command, "quit") == 0 || 
                   strcmp(command, "q") == 0) {
            break;
        } else
        {
            printf("Unrecognized command %s\n", command);
        }
        

    }
}

enum idtype { UNKNOWN, ID, ALIAS };

int argparse(char* filestore, const size_t fslen, 
              char* broker, const size_t brokerlen,
              char* clientx,  const size_t clidlen, 
              enum idtype* idtype,
              int* help, int argc, char** argv) {


    int validargs = 0;
    int clientset = 0;
    int i = 1, j = 0;
    while ( i < argc ) {
        int step=1;

        char *arg = argv[i];
        size_t arglen = strlen(arg);
        
        if ( arglen >= 2 && strncmp(arg, "-", 1) == 0 ) {
            
            // TODO: there is almost certainly a better way of doing this
            // however we must be careful not to have any dependency on 
            // libraries we cannot easily run on devices.
            char* argname = NULL;
            char argletter = arg[1];
            if ( argletter == '-' ) {
                argname = &arg[2];
            }

            if ( argletter == 'f' || strncmp(argname, "filestore", 9) == 0 ) {
                if ( i+1 < argc ) {
                    size_t pathlen = 0;
                    size_t bytesparsed = 0;
                    char* path = NULL;
                    step += 1;
                    path = argv[i+1];
                    pathlen = strlen(path);

                    bytesparsed = strlcpy(filestore, path, fslen);
                    if ( bytesparsed >= pathlen ) {
                        printf("filestore: invalid parameter");
                        validargs = 1;
                        break;
                    }
                }
                else {
                    printf("filestore: no filename specified");
                    validargs = 1;
                    break;
                }
            } 
            else if ( argletter == 'i' || strncmp(argname, "clientid", 8) == 0 ) 
            {
                if ( i+1 < argc ) {
                    size_t clientlen = 0;
                    size_t bytesparsed = 0;
                    char* client = NULL;
                    step += 1;
                    client = argv[i+1];
                    clientlen = strlen(client);

                    bytesparsed = strlcpy(clientidx, client, clidlen);
                    if ( bytesparsed >= clientlen ) {
                        printf("clientid: invalid parameter");
                        validargs = 1;
                        break;
                    }
                    if ( clientset == 1 ) {
                        printf("clientid: clientalias already passed");
                        validargs = 1;
                        break;
                    }
                    clientset = 1;
                }
                else {
                    printf("clientid: no filename specified");
                    validargs = 1;
                    break;
                }
            } 
            else if ( argletter == 'a' || strncmp(argname, "clientalias", 8) == 0 ) 
            {
                if ( i+1 < argc ) {
                    size_t clientlen = 0;
                    size_t bytesparsed = 0;
                    char* client = NULL;
                    step += 1;
                    client = argv[i+1];
                    clientlen = strlen(client);

                    bytesparsed = strlcpy(clientx, clientid, clidlen);
                    if ( bytesparsed >= clientlen ) {
                        printf("clientid: invalid parameter");
                        validargs = 1;
                        break;
                    }
                    if ( clientset == 1 ) {
                        printf("clientid: clientalias already passed");
                        validargs = 1;
                        break;
                    }
                }
                else {
                    printf("clientid: no filename specified");
                    validargs = 1;
                    break;
                }
            } 
            else if ( argletter == 'b' || strncmp(argname, "broker", 8) == 0 ) 
            {
                if ( i+1 < argc ) {
                    size_t pathlen = 0;
                    size_t bytesparsed = 0;
                    char* brokerp = NULL;
                    step += 1;
                    brokerp = argv[i+1];
                    brokerplen = strlen(brokerp);

                    bytesparsed = strlcpy(broker, brokerp, brokerplen);
                    if ( bytesparsed >= brokerplen ) {
                        printf("broker: invalid parameter");
                        validargs = 1;
                        break;
                    }
                }
                else {
                    printf("broker: no filename specified");
                    validargs = 1;
                    break;
                }
            }
            else if ( argletter == 'h' || strncmp(argname, "help", 4) == 0) {
                *help = 1;
            }
            else {
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



int main(int argc, char** argv) {

    MQTTClient mqttClient;
    char filestore[256];
    char client[256];
    char broker[256];
    char e4cmdtopic[256];
    int helpflag = 0;
    enum idtype idtype = UNKNOWN;

    memset(filestore, 0, sizeof(filestore));
    memset(client, 0, sizeof(client));
    memset(broker, 0, sizeof(broker));

    if ( argparse(filestore, sizeof(filestore), 
                  clientid, sizeof(clientid),
                  &idtype,
                  &helpflag, argc, argv) != 0 ) {
        printf("Invalid command line arguments, exiting.\n");
        return 1;
    }

    if ( helpflag ) {
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
    if ( strlen(filepath) == 0 ) {
        
        switch (idtype) {
        case idtype::ID:
            {
            if ( strlen(client) != 2*E4_ID_LEN) {
                printf("Invalid Client ID Length.\n");
                return 1;
            }
            // TODO:decode hex to bytes.
            }
        case idtype::ALIAS:
            {
            
            }
        default:
            // This codepath should be unreachable.
        }
    } 
    else
    {
        // we _do_ have a filepath. let's try to sync data from it.
        
        if (e4c_load(&e4client.store, filepath) != 0 {
            printf("Unable to load data from %s.\n", filepath);
            return 1;
        }
    }

    /* from here, we are NOT running in help mode and do have a clientid 
     * and broker, albeit possibly not valid */
    

    mqtt_init(client.Client, broker);    
    
    return 0;
}
