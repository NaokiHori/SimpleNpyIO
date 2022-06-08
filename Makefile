CC       := cc
CFLAGS   := -O3 -std=c99 -Wall -Wextra -DLOGGING_SIMPLE_NPYIO
INCLUDES := -Iinclude
SRCSDIR  := src

all:
	$(CC) $(CFLAGS) $(INCLUDES) $(SRCSDIR)/simple_npyio.c $(SRCSDIR)/main.c -o a.out

clean:
	$(RM) *.out

.PHONY : all clean
