CC       := cc
CFLAGS   := -O3 -std=c99 -Wall -Wextra
DEPEND   := -MMD
INCLUDES := -Iinclude
SRCSDIR  := src
OBJSDIR  := obj
SRCS     := $(foreach dir, $(shell find $(SRCSDIR) -type d), $(wildcard $(dir)/*.c))
OBJS     := $(addprefix $(OBJSDIR)/, $(subst $(SRCSDIR)/,,$(SRCS:.c=.o)))
DEPS     := $(addprefix $(OBJSDIR)/, $(subst $(SRCSDIR)/,,$(SRCS:.c=.d)))
TARGET   := a.out

all: $(TARGET)

$(TARGET): $(OBJS)
		$(CC) $(CFLAGS) $(DEPEND) -o $@ $^

$(OBJSDIR)/%.o: $(SRCSDIR)/%.c
		@if [ ! -e `dirname $@` ]; then \
			mkdir -p `dirname $@`; \
		fi
		$(CC) $(CFLAGS) $(DEPEND) $(INCLUDES) -c $< -o $@

clean:
		$(RM) -r $(OBJSDIR) $(TARGET)

-include $(DEPS)

.PHONY : all clean
