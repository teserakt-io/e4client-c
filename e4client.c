//	e4client.c
//	2018-05-01	Markku-Juhani O. Saarinen <markku@teserakt.io>
//	(c) 2018 Copyright Teserakt AG

#include <string.h>
#ifndef __AVR__
#include <stdio.h>
#include <time.h>
#endif
#include "e4client.h"
#include "sha3.h"
#include "aes_siv.h"

#ifdef __AVR__

// Arduino has very limited SRAM; each takes 96 bytes
#define E4C_TOPICS_MAX 5

// Wall-clock time, incremented exernally, calibrated from control message
uint64_t secs1970 = 0;

#else

// Embedded Linux or other >8 bit platform will have 10kB for this table
#define E4C_TOPICS_MAX 100

#endif

// Currently allow 10 min slack in either direction in timestamps
#define E4C_TIME_FUTURE   (10 * 60)
#define E4C_TIME_TOO_OLD  (10 * 60)

// This is the topic-key structure

typedef struct {
	uint8_t topic[E4C_TOPIC_LEN];
	uint8_t key[E4C_KEY_LEN];
} topic_key_t;

// number of topic keys
static int topic_keys_no = 0;

static topic_key_t topic_keys[E4C_TOPICS_MAX] =
{
	// KEY 0 is always the "id key"
	{ { 0 }, { 0 } }
};

// Protect message

int e4c_protect_message(uint8_t *cptr, size_t cmax, size_t *clen,
	const uint8_t *mptr, size_t mlen, const char *topic)
{
	int i;
	size_t clen2;
	const uint8_t *key = NULL;
	uint8_t topx[E4C_TOPIC_LEN];
	uint64_t time_now = 0;

	// hash the topic
	sha3(topic, strlen(topic), topx, E4C_TOPIC_LEN);

	for (i = 0; i < topic_keys_no; i++) {	// search for it
		if (memcmp(topic_keys[i].topic, topx, E4C_TOPIC_LEN) == 0) {
			key = topic_keys[i].key;
			break;							// found it
		}
	}
	if (i >= topic_keys_no)
		return E4ERR_TopicKeyMissing;

	if (mlen + 24 > cmax)					// actually: not enough space
		return E4ERR_TooShortCiphertext;
	*clen = mlen + 24;

#ifdef __AVR__
	time_now = secs1970;					// externally incremented
#else
	time_now = time(NULL);					// timestamp
#endif

	for (i = 0; i < 8; i++) {
		cptr[i] = time_now & 0xFF;
		time_now >>= 8;
	}

	// encrypt
	clen2 = 0;
	aes256_encrypt_siv(cptr + 8, &clen2, cptr, 8, mptr, mlen, key);

	return 0;
}

// Remove topic (hash)

int e4c_remove_topic(const uint8_t *topic_hash)
{
	int i, j;

	for (i = 0; i < topic_keys_no; i++) {
		if (memcmp(topic_keys[i].topic, topic_hash, E4C_TOPIC_LEN) == 0) {
			// remove this item and move list up
			for (j = i + 1; j < topic_keys_no; j++) {
				memcpy(&topic_keys[j - 1], &topic_keys[j],
					sizeof(topic_key_t));
			}
			topic_keys_no--;
			return 0;
		}
	}

	return E4ERR_TopicKeyMissing;
}

// Clear all topics except ID key

int e4c_reset_topics()
{
	if (topic_keys_no < 1)
		return E4ERR_TopicKeyMissing;
	topic_keys_no = 1;

	memset(&topic_keys[topic_keys_no], 0x00,
		sizeof(topic_key_t) * (E4C_TOPICS_MAX - topic_keys_no));

	return 0;
}

// set id key for this instance index 0

int e4c_set_id_key(const uint8_t *cmd_topic_hash, const uint8_t *key)
{
	if (cmd_topic_hash != NULL)
		memcpy(topic_keys[0].topic, cmd_topic_hash, E4C_TOPIC_LEN);
	if (key != NULL)	
		memcpy(topic_keys[0].key, key, E4C_KEY_LEN);

	if (cmd_topic_hash != NULL && key != NULL) {
		if (topic_keys_no < 1)
			topic_keys_no = 1;
	}

	return 0;
}

// set key for given topic (hash)

int e4c_set_topic_key(const uint8_t *topic_hash, const uint8_t *key)
{
	int i;

	for (i = 0; i < topic_keys_no; i++) {
		if (memcmp(topic_keys[i].topic, topic_hash, E4C_TOPIC_LEN) == 0)
			break;
	}
	if (i >= E4C_TOPICS_MAX)				// out of space
		return E4ERR_TopicKeyMissing;

	memcpy(topic_keys[i].topic, topic_hash, E4C_TOPIC_LEN);
	memcpy(topic_keys[i].key, key, E4C_KEY_LEN);

	if (i == topic_keys_no) {				// new topic
		topic_keys_no++;
	}

	return 0;
}

// Unprotect message

int e4c_unprotect_message(uint8_t *mptr, size_t mmax, size_t *mlen,
	const uint8_t *cptr, size_t clen, const char *topic)
{
	int i, j;
	uint8_t topx[E4C_TOPIC_LEN];
	uint64_t tstamp;
#ifndef __AVR__
	uint64_t secs1970;

	secs1970 = (uint64_t) time(NULL);		// this system has a RTC
#endif

	// hash the topic
	sha3(topic, strlen(topic), topx, E4C_TOPIC_LEN);

	if (clen < 24 || mmax < clen - 24)
		return E4ERR_TooShortCiphertext;

	for (i = 0; i < topic_keys_no; i++) {	// find the key
		if (memcmp(topic_keys[i].topic, topx, E4C_TOPIC_LEN) == 0) {
			break;
		}
	}
	if (i >= topic_keys_no)
		return E4ERR_TopicKeyMissing;

	// check timestamp
	tstamp = 0;
	for (j = 7; j >= 0; j--) {
		tstamp <<= 8;
		tstamp += (uint64_t) cptr[j];
	}

	// decrypt
	if (aes256_decrypt_siv(mptr, mlen, cptr, 8, cptr + 8, clen - 8, 
		topic_keys[i].key) != 0)
		return E4ERR_InvalidTag;

	if (*mlen + 1 > mmax)					// zero-pad it in place..
			return E4ERR_TooShortCiphertext;
	mptr[*mlen] = 0;


	// Since AVR has no real time clock, time is initially unknown. 
	if (secs1970 < 946684800) {
		if (i == 0)							// command channel ?
			secs1970 = tstamp;				// calibrate message
	} else {

		if (tstamp >= secs1970) {
			if (tstamp - secs1970 > E4C_TIME_FUTURE)
				return E4ERR_TimestampInFuture;
		} else {
			if (secs1970 - tstamp > E4C_TIME_TOO_OLD)
				return E4ERR_TooOldTimestamp; 
		}
	}

	if (i != 0)								// if not command, success
		return 0;

	// execute commands

	if (*mlen == 0)
		return E4ERR_InvalidCommand;

	switch (mptr[0]) {
		case 0x00:							// RemoveTopic(topic);
			return e4c_remove_topic((const uint8_t *) mptr + 1);

		case 0x01:							// ResetTopics();
			if (*mlen != 1)
				return E4ERR_InvalidCommand;
			return e4c_reset_topics();

		case 0x02:							// SetIdKey(key)
			if (*mlen != (1 + E4C_KEY_LEN))
				return E4ERR_InvalidCommand;
			return e4c_set_id_key(NULL, mptr + 1);

		case 0x03:							// SetTopicKey(topic, key)
			if (*mlen != (1 + E4C_KEY_LEN + E4C_TOPIC_LEN))
				return E4ERR_InvalidCommand;
			return e4c_set_topic_key((const uint8_t *) 
				mptr + E4C_KEY_LEN + 1, mptr + 1);
	}

	return E4ERR_InvalidCommand;
}

// debug

#ifndef __AVR__

void e4c_debug_dumpkeys()
{
	int i, j;

	for (i = 0; i < topic_keys_no; i++) {
		printf("!!! %2d topic hash = ", i);
		for (j = 0; j < E4C_TOPIC_LEN; j++)
			printf("%02X", topic_keys[i].topic[j]);
		printf("\n!!! %2d  topic key = ", i);
		for (j = 0; j < E4C_KEY_LEN; j++)
			printf("%02X", topic_keys[i].key[j]);
		printf("\n");
	}
}

#endif
