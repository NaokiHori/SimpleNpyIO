CC       := cc
CFLAGS   := -O3 -std=c99 -Wall -Wextra#-DLOGGING_SIMPLE_NPYIO
INCLUDES := -Iinclude
SRCSDIR  := src

all:
	$(CC) $(CFLAGS) $(INCLUDES) $(SRCSDIR)/simple_npyio.c $(SRCSDIR)/main.c -o a.out

test01:
	$(CC) $(CFLAGS) $(INCLUDES) $(SRCSDIR)/simple_npyio.c $(SRCSDIR)/test01.c -o test01.out

test02:
	$(CC) $(CFLAGS) $(INCLUDES) $(SRCSDIR)/simple_npyio.c $(SRCSDIR)/test02.c -o test02.out

test03:
	$(CC) $(CFLAGS) $(INCLUDES) $(SRCSDIR)/simple_npyio.c $(SRCSDIR)/test03.c -o test03.out

test04:
	$(CC) $(CFLAGS) $(INCLUDES) $(SRCSDIR)/simple_npyio.c $(SRCSDIR)/test04.c -o test04.out

clean:
	$(RM) *.out

.PHONY : all clean
