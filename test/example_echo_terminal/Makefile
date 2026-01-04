
# C Bits

CC = g++
CFLAGS = -g -Og -I$(abspath ../unity)
LDFLAGS = -L$(abspath ..) -lunity

SRC  := echo_terminal.c main.c uart.c
VSRC := echo_terminal.v 

ROOT := ../../src
VPATH := $(ROOT) $(ROOT)/examples
VFLAGS := $(addprefix -I,$(VPATH))

vpath %.v $(VPATH)

.PHONY: all
all: build

.PHONY: run
run: obj_dir/Vecho_terminal
	./obj_dir/Vecho_terminal

.PHONY: build
build: obj_dir/Vecho_terminal

../libunity.a:
	cd .. && make -f unity.mk

obj_dir/Vecho_terminal: $(VSRC) $(SRC) | ../libunity.a
	verilator $^ --cc --build --exe --trace --timing -CFLAGS "$(CFLAGS)" -LDFLAGS "$(LDFLAGS)" $(VFLAGS)

.PHONY: clean
clean:
	rm -rf obj_dir
