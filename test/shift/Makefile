
# C Bits

CC = g++
CFLAGS = -g -Og -I$(abspath ../unity)
LDFLAGS = -L$(abspath ..) -lunity

SRC  := shift.c main.c
VSRC := shift.v

vpath %.v ../../src

.PHONY: all
all: build

.PHONY: run
run: obj_dir/Vshift
	./obj_dir/Vshift

.PHONY: build
build: obj_dir/Vshift

../libunity.a:
	cd .. && make -f unity.mk

obj_dir/Vshift: $(VSRC) $(SRC) | ../libunity.a
	verilator $^ --cc --build --exe --trace --timing -CFLAGS "$(CFLAGS)" -LDFLAGS "$(LDFLAGS)"

.PHONY: clean
clean:
	rm -rf obj_dir
