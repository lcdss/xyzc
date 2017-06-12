TARGET ?= xyzc
SRC_DIRS ?= ./src

SRCS := $(shell find $(SRC_DIRS) -name *.c)
OBJS := $(addsuffix .o, $(basename $(SRCS)))
DEPS := $(OBJS:.o=.d)
CFLAGS = -std=c11 -Wall -Wextra -Werror \
         -Wshadow -Wundef -Wcast-qual -Wcast-align -Wfloat-equal \
				 -Wpointer-arith -Wstrict-prototypes -Wswitch-default -Wconversion \
				 -Wunreachable-code -Wstrict-overflow=5 -Wwrite-strings -Waggregate-return \
				 -ftrapv -fverbose-asm -finstrument-functions -g
         # -Wswitch-enum

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I, $(INC_DIRS))

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $(CFLAGS) $(OBJS) -o $@ $(LOADLIBES) $(LDLIBS)

.PHONY: clean
clean:
	$(RM) $(TARGET) $(OBJS) $(DEPS)

-include $(DEPS)
