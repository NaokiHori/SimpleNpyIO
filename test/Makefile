ROOT   := ..
CC     := cc
CFLAG  := -std=c99 -Wall -Wextra -Werror -DSNPYIO_ENABLE_LOGGING
INC    := -I$(ROOT)/include
SRC    := $(ROOT)/src/snpyio.c main.c
TARGET := a.out

help:
	@echo "all   : create \"$(TARGET)\""
	@echo "clean : remove \"$(TARGET)\""
	@echo "help  : show this message"

all:
	$(CC) $(CFLAG) $(INC)  $(SRC) -o $(TARGET)

clean:
	$(RM) $(TARGET)

.PHONY : help all clean

