
#ifndef CLIENT_H
#define CLIENT_H

void client_setid(e4client *client, const char *arg);
void client_genkey(e4client *client, const char *arg);
void client_setalias(e4client *client, const char *arg);
void client_setkey(e4client *client, const char *arg);
void client_settopickey(e4client *client, char *arg);
void client_setpwd(e4client *client, const char *arg);
void client_subscribe(e4client *client, const char *arg);
void client_unsubscribe(e4client *client, const char *arg);
void client_changetopic(e4client *client, const char *arg);
void client_e4msg(e4client *client, const char *arg);
void client_clearmsg(e4client *client, char *arg);
void client_list(e4client *client, const char *arg);
void client_zero(e4client *client, const char *arg);

#endif
