# Makefile
# 2018-05-01  Markku-Juhani O. Saarinen <mjos@iki.fi>

BIN		= e4clic
OBJS		= clic.o 			\
		e4client.o			\
		e4persistent.o			\
		crypto/aes_siv.o		\
		crypto/aes256enc_openssl.o	\
		crypto/sha3.o			\
		crypto/keccakf1600.o

DIST		= e4clic

CC		= gcc
CFLAGS		= -Wall -Ofast
LIBS		= -lcrypto -lpaho-mqtt3c -lpthread
LDFLAGS		= -L.
INCLUDES	= -Icrypto -Ipaho.mqtt.c/src

$(BIN): 	$(OBJS)
		$(CC) $(LDFLAGS) -o $(BIN) $(OBJS) $(LIBS)

.c.o:
		$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
		rm -rf $(DIST)-*.t?z $(OBJS) $(BIN) *~

dist:		clean
		cd ..; \
		tar cfvz $(DIST)/$(DIST)-`date "+%Y%m%d%H%M"`.tgz \
			$(DIST)/* $(DIST)/.gitignore
