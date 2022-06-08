PRJCT_ROOT := ../..
CC         := cc
CFLAGS     := -O3 -std=c99 -Wall -Wextra#-DLOGGING_SIMPLE_NPYIO
INCLUDES   := -I$(PRJCT_ROOT)/include
SRCSDIR    := src
LIBSRC     := $(PRJCT_ROOT)/src/simple_npyio.c

all: test01 test02 test03 test04

test01:
	$(CC) $(CFLAGS) $(INCLUDES) $(LIBSRC) src/test01.c -o test01.out

test02:
	$(CC) $(CFLAGS) $(INCLUDES) $(LIBSRC) src/test02.c -o test02.out

test03:
	$(CC) $(CFLAGS) $(INCLUDES) $(LIBSRC) src/test03.c -o test03.out

test04:
	$(CC) $(CFLAGS) $(INCLUDES) $(LIBSRC) src/test04.c -o test04.out

clean:
	$(RM) test*.out npyfiles/*.npy

.PHONY : all clean
