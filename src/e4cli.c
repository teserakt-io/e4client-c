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

        if (strcmp(commmand, "")

    }
}

void init() {

}

int main(int argc, char** argv) {

}
