// e4client.h
// 2018-05-01  Markku-Juhani O. Saarinen <mjos@iki.fi>

#ifndef _E4CLIENT_H_
#define _E4CLIENT_H_

#include <stdint.h>
#include <stddef.h>

#define E4ERR_Ok					0
#define E4ERR_InvalidTag			101
#define E4ERR_TimestampInFuture		102
#define E4ERR_TooOldTimestamp		103
#define E4ERR_TooShortCiphertext	104
#define E4ERR_TopicKeyMissing		105
#define E4ERR_ReplayedMessage		106

#define E4ERR_InvalidCommand		107

// field sizes

#define E4C_TOPIC_LEN 32
#define E4C_KEY_LEN 64

// == Local Client API ==

// Protect message mptr[mlen] and place it to cptr[cmax] (length in *clen)
int e4c_protect_message(uint8_t *cptr, size_t cmax, size_t *clen,
	const uint8_t *mptr, size_t mlen, const char *topic);

// Unprotect message cptr[clen] and place it to mptr[mmax] (length in *mlen)
int e4c_unprotect_message(uint8_t *mptr, size_t mmax, size_t *mlen,
	const uint8_t *cptr, size_t clen, const char *topic);

// == C2 Commands -- Internal ==

int e4c_remove_topic(const uint8_t *topic_hash);

int e4c_reset_topics();

int e4c_set_id_key(const uint8_t *cmd_topic_hash, const uint8_t *key);

int e4c_set_topic_key(const uint8_t *topic_hash, const uint8_t *key);

// == Debug ==

void e4c_debug_dumpkeys();

#endif
