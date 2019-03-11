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

/* local header includes */
#include "e4cli.h"
#include "mqtt.h"

const char cli_commands[] = "
The following commands can be used:

    !setid
    !setkey
    !genkey
    !settopickey
    !s, !subscribe
    !u, !unsubscribe
    !c, !changetopic
    !e, !e4msg
    !m, !clearmsg
    !l, !list state
    !z, !zero state
    !q, !quit
";

void client_setid(const char* arg) {
    
}

void client_setkey(const char* arg) {

}

void client_genkey(const char* arg) {
    
}

void client_settopickey(const char* arg) {
    
}

void client_subscribe(const char* arg) {
    
    size_t arglen = strlen(arg);
    if ( arglen == 0 )
    {
        printf("client_subscribe: Invalid topic")
        return;
    }

    MQTTClient_subscribe(...)
}

void client_unsubscribe(const char* arg) {
    size_t arglen = strlen(arg);
    if ( arglen == 0 )
    {
        printf("client_subscribe: Invalid topic")
        return;
    }

    MQTTClient_unsubscribe(...)
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
        if (linelen >= 1 && line[l-1] == '\n') {
            line[--linelen] = 0;
        }

        if (line[0] != '!')
        {
            // TODO: what do we do here?
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
                    arg = &line[i+];
                }
                break
            }
            
        }

        if ( command == NULL ) {
            // error, let's do something about it?
            continue
        }

        if (strcmp(commmand, "setid") == 0) { 
            client_setid(arg);
        } else if (strcmp(command, "setkey") == 0) {
            client_setkey(arg);
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
        } else if (strcmp(command, "quit") == 0 || 
                   strcmp(command, "q") == 0) {
            break
        } else
        {
            printf("Unrecognized command %s\n", command);
        }
        

    }
}

const char version[] = "E4CLI E4 Command Line Client in C (c) Teserakt AG 2018, 2019.\n
https://www.teserakt.io/\n"


void argparse(int argc, char** argv) {

}

int main(int argc, char** argv) {

    MQTTClient mqttClient;
}
