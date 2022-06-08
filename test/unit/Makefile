PRJCT_ROOT := ../..
CC 		     := cc
CFLAGS     := -std=c99 -Wall -Wextra
# directly include src of the project
# to access to the static functions
INCLUDES   := -I$(PRJCT_ROOT)/include \
							-I$(PRJCT_ROOT)/src
LIBS			 := -L/usr/local/lib \
							-lcunit
SRCS1      := src/auxiliary.c
SRCS2      := src/reader.c
SRCS3      := src/writer.c
TARGET1    := test_auxiliary.out
TARGET2    := test_reader.out
TARGET3    := test_writer.out

all: clean auxiliary reader writer

auxiliary:
	$(CC) $(CFLAGS) $(INCLUDES) $(SRCS1) -o $(TARGET1) $(LIBS)

reader:
	$(CC) $(CFLAGS) $(INCLUDES) $(SRCS2) -o $(TARGET2) $(LIBS)

writer:
	$(CC) $(CFLAGS) $(INCLUDES) $(SRCS3) -o $(TARGET3) $(LIBS)

clean:
	$(RM) *.out *.xml

