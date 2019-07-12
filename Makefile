# Makefile
# 2018-05-01  Markku-Juhani O. Saarinen <mjos@iki.fi>
# 2018-12-01  Antony Vennard <antony@teserakt.io>

CC	    = gcc
AR          = ar
ARFLAGS     = rcs
CFLAGS	    = -Wall -Werror -Ofast
LDFLAGS	    = -L. -lc -lpthread 
INCLUDES    = -Iinclude -I$(E4LIBDIR)/include -Isrc/crypto -Ipaho.mqtt.c/src


# BUILD environment
GITCOMMIT=$(shell git rev-list -1 HEAD)
NOW=$(shell date "+%Y%m%d%H%M")

# OBJ paths match their src folder equivalents
INCDIR  = include
OBJDIR  = build
SRCDIR  = src
LIBDIR  = lib
BINDIR  = bin
BIN     = $(BINDIR)/e4cli
LIB	= $(LIBDIR)/$(LIBNAME).a
DISTDIR	= dist

LIBS    = libpaho-mqtt3c.a $(E4LIBDIR)/lib/libe4.a 

OBJS    = $(OBJDIR)/commands.o      \
          $(OBJDIR)/repl.o          \
          $(OBJDIR)/recv.o          \
          $(OBJDIR)/mqtt.o          \
          $(OBJDIR)/e4cli.o

default: setup $(BIN)

setup:
	mkdir -p $(OBJDIR); \
	mkdir -p $(LIBDIR); \
	mkdir -p $(DISTDIR); \

$(BIN): $(OBJS)
	mkdir -p bin;
	$(CC) $(LDFLAGS) -o $(BIN) $(OBJS) $(LIBS)

build/%.o: src/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(DISTDIR) $(OBJDIR) $(LIBDIR) $(OBJS) $(LIB) *~ *.e4p
	find . -name "*.o" -exec rm -f {} \;

dist: $(BIN)
	@echo 'Making $(DISTDIR)/$(LIBNAME)-$(NOW)-$(GITCOMMIT).tar.bz2'
	tar cfvj $(DISTDIR)/$(LIBNAME)-$(NOW)-$(GITCOMMIT).tar.bz2 $(LIBDIR)/* $(INCDIR)/*

format:
	clang-format -i src/*.c src/*.h
