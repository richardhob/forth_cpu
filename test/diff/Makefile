
# C Bits

CC = g++
CFLAGS = -g -Og -I$(abspath ../unity)
LDFLAGS = -L$(abspath ..) -lunity

SRC  := diff.c main.c
VSRC := diff.v

vpath %.v ../../src

.PHONY: all
all: build

.PHONY: run
run: obj_dir/Vdiff
	./obj_dir/Vdiff

.PHONY: build
build: obj_dir/Vdiff

../libunity.a:
	cd .. && make -f unity.mk

obj_dir/Vdiff: $(VSRC) $(SRC) | ../libunity.a
	verilator $^ --cc --build --exe --trace --timing -CFLAGS "$(CFLAGS)" -LDFLAGS "$(LDFLAGS)"

.PHONY: clean
clean:
	rm -rf obj_dir
