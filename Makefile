# Executables

PREFIX ?= arm-none-eabi
CC := $(PREFIX)-gcc
AR := $(PREFIX)-ar

# Sources

SRC = $(wildcard *.c)
OBJS = $(SRC:.c=.o)
INCLUDE += -I.

# Flags

ARCH_FLAGS = -mthumb -mcpu=cortex-m3 -msoft-float -mfix-cortex-m3-ldrd

CFLAGS += -Os -std=c99 -g
CFLAGS += $(ARCH_FLAGS)
CFLAGS += -Wextra -Wshadow -Wimplicit-function-declaration
CFLAGS += -Wredundant-decls -Wmissing-prototypes -Wstrict-prototypes
CFLAGS += -fno-common -ffunction-sections -fdata-sections
CFLAGS += -MD
CFLAGS += -Wall -Wundef
CFLAGS += $(INCLUDE)

# Targets

%.o: %.c
	$(CC) $(CFLAGS) -o $(*).o -c $(*).c

%.a: %.o
	$(AR) $(ARFLAGS) "$@" $(OBJS)

all: $(OBJS)

clean:
	$(RM) *.o *.d $(OBJS)

.PHONY: clean
