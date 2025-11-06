
# C Bits

CC ?= gcc
CFLAGS += -g -Og -Iunity

vpath %.c unity fifo .

TEST_SRC := fifo.c 
UNITY_SRC := unity.c unity_fixture.c unity_memory.c

SRC  := test_main.c $(TEST_SRC) $(UNITY_SRC)
OBJ  := $(SRC:.c=.o)

VSRC := fifo.v

.PHONY: all
all: tests
	./tests

include vlibs.mk

tests: $(OBJ) | $(VOBJ_DIR)
	$(CC) $^ -o $@ $(LDFLAGS) $(CFLAGS)

.PHONY: clean
clean: vlibs_clean
	rm $(OBJ)
	rm tests
