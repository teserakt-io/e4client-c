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

/* local header includes */
#include "e4cli.h"
#include "mqtt.h"

void repl(e4client *client)
{

    char line[256] = { 0 };
    int i = 0;

    while (fgets(line, sizeof(line), stdin) != NULL)
    {

        char *command = NULL;
        char *arg = NULL;

        int linelen = strlen(line);

        // user pressed enter/entered nothing except a newline.
        if (linelen == 1)
        {
            continue;
        }

        // remove trailing new line from fgets()
        if (linelen >= 1 && line[linelen - 1] == '\n')
        {
            line[--linelen] = 0;
        }

        if (line[0] != '!')
        {
            printf("Unrecognized command, try !help\n");
            continue;
        }

        for (i = 1; i < linelen; i++)
        {
            if (i == linelen - 1)
            {
                command = &line[1];
                break;
            }
            if (line[i] == ' ')
            {
                line[i] = 0;
                command = &line[1];
                // don't assign if i = linelen-1; potential buffer overflow.
                if (i + 1 < linelen)
                {
                    arg = &line[i + 1];
                }
                break;
            }
        }

        if (command == NULL)
        {
            // error, let's do something about it?
            continue;
        }

        if (strcmp(command, "setid") == 0)
        {
            client_setid(client, arg);
            if (strcmp(command, "setalias") == 0)
            {
                client_setid(client, arg);
            }
            else if (strcmp(command, "setkey") == 0)
            {
                client_setkey(client, arg);
            }
            else if (strcmp(command, "setpwd") == 0)
            {
                client_setpwd(client, arg);
            }
            else if (strcmp(command, "genkey") == 0)
            {
                client_genkey(client, arg);
            }
            else if (strcmp(command, "settopickey") == 0)
            {
                client_settopickey(client, arg);
            }
            else if (strcmp(command, "subscribe") == 0 || strcmp(command, "s") == 0)
            {
                client_subscribe(client, arg);
            }
            else if (strcmp(command, "unsubscribe") == 0 || strcmp(command, "u") == 0)
            {
                client_unsubscribe(client, arg);
            }
            else if (strcmp(command, "changetopic") == 0 || strcmp(command, "u") == 0)
            {
                client_changetopic(client, arg);
            }
            else if (strcmp(command, "e4msg") == 0 || strcmp(command, "e") == 0)
            {
                client_e4msg(client, arg);
            }
            else if (strcmp(command, "clearmsg") == 0 || strcmp(command, "m") == 0)
            {
                client_clearmsg(client, arg);
            }
            else if (strcmp(command, "list") == 0 || strcmp(command, "l") == 0)
            {
                client_list(client, arg);
            }
            else if (strcmp(command, "zero") == 0 || strcmp(command, "z") == 0)
            {
                client_zero(client, arg);
            }
            else if (strcmp(command, "help") == 0 || strcmp(command, "h") == 0)
            {
            }
            else if (strcmp(command, "quit") == 0 || strcmp(command, "q") == 0)
            {
                break;
            }
            else
            {
                printf("Unrecognized command %s\n", command);
            }
        }
    }
}
