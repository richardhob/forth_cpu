
CC := gcc
CFLAGS += -g -Og -Iunity

vpath %.c unity fifo .

TEST_SRC := fifo.c 
UNITY_SRC := unity.c unity_fixture.c unity_memory.c

SRC := test_main.c $(TEST_SRC) $(UNITY_SRC)
OBJ := $(SRC:.c=.o)

.PHONY: all
all: tests
	./tests

tests: $(OBJ)
	$(CC) $^ -o $@

.PHONY: clean
clean:
	rm $(OBJ)
	rm tests
