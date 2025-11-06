
# C Bits

CC = g++
CFLAGS = -g -Og -I$(abspath ../unity)
LDFLAGS = -L$(abspath ..) -lunity

SRC  := fifo.c main.c
VSRC := fifo.v

vpath %.v ../../src/fifo

.PHONY: all
all: build

.PHONY: run
run: obj_dir/Vfifo
	./obj_dir/Vfifo

.PHONY: exe
build: obj_dir/Vfifo

../libunity.a:
	cd .. && make -f unity.mk

obj_dir/Vfifo: $(VSRC) $(SRC) | ../libunity.a
	verilator $^ --binary --exe --trace -CFLAGS "$(CFLAGS)" -LDFLAGS "$(LDFLAGS)"

.PHONY: clean
clean:
	rm -rf obj_dir
