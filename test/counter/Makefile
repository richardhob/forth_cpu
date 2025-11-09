
# C Bits

CC = g++
CFLAGS = -g -Og -I$(abspath ../unity)
LDFLAGS = -L$(abspath ..) -lunity

SRC  := counter.c main.c
VSRC := counter.v

vpath %.v ../../src/counter

.PHONY: all
all: build

.PHONY: run
run: obj_dir/Vcounter
	./obj_dir/Vcounter

.PHONY: build
build: obj_dir/Vcounter

../libunity.a:
	cd .. && make -f unity.mk

obj_dir/Vcounter: $(VSRC) $(SRC) | ../libunity.a
	verilator $^ --cc --build --exe --trace --timing -CFLAGS "$(CFLAGS)" -LDFLAGS "$(LDFLAGS)"

.PHONY: clean
clean:
	rm -rf obj_dir
