CC     := cc
CFLAG  := -std=c99 -Wall -Wextra -Werror
INC    := -Iinclude
SRCDIR := src
OBJDIR := obj
SRC    := $(shell find $(SRCDIR) -type f -name *.c)
OBJ    := $(patsubst %.c,$(OBJDIR)/%.o,$(SRC))
DEP    := $(patsubst %.c,$(OBJDIR)/%.d,$(SRC))
TARGET := a.out

help:
	@echo "all   : create \"$(TARGET)\""
	@echo "clean : remove \"$(TARGET)\" and object files under \"$(OBJDIR)\""
	@echo "help  : show this message"

all: $(TARGET)

clean:
	$(RM) -r $(OBJDIR) $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAG) $^ -o $@

$(OBJDIR)/%.o: %.c
	@if [ ! -e $(dir $@) ]; then \
		mkdir -p $(dir $@); \
	fi
	$(CC) $(CFLAG) -MMD $(INC) -c $< -o $@

-include $(DEP)

.PHONY : help all clean

