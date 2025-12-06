
# C Bits

CC = g++
CFLAGS = -g -Og -I$(abspath ../unity)
LDFLAGS = -L$(abspath ..) -lunity

SRC  := clock_divider.c main.c
VSRC := clock_divider.v

vpath %.v ../../src

.PHONY: all
all: build

.PHONY: run
run: obj_dir/Vclock_divider
	./obj_dir/Vclock_divider

.PHONY: build
build: obj_dir/Vclock_divider

../libunity.a:
	cd .. && make -f unity.mk

obj_dir/Vclock_divider: $(VSRC) $(SRC) | ../libunity.a
	verilator $^ --cc --build --exe --trace --timing -CFLAGS "$(CFLAGS)" -LDFLAGS "$(LDFLAGS)"

.PHONY: clean
clean:
	rm -rf obj_dir
