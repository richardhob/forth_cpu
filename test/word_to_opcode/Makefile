
# C Bits

CC = g++
CFLAGS = -g -Og -I$(abspath ../unity)
LDFLAGS = -L$(abspath ..) -lunity

SRC  := word_to_opcode.c main.c
VSRC := word_to_opcode.v

ROOT := ../../src
VPATH := $(ROOT) $(ROOT)/compiler
VFLAGS := $(addprefix -I,$(VPATH))

vpath %.v $(VPATH)

.PHONY: all
all: build

.PHONY: run
run: obj_dir/Vword_to_opcode
	./obj_dir/Vword_to_opcode

.PHONY: build
build: obj_dir/Vword_to_opcode

../libunity.a:
	cd .. && make -f unity.mk

obj_dir/Vword_to_opcode: $(VSRC) $(SRC) | ../libunity.a
	verilator $^ --cc --build --exe --trace --timing -CFLAGS "$(CFLAGS)" -LDFLAGS "$(LDFLAGS)" $(VFLAGS)

.PHONY: clean
clean:
	rm -rf obj_dir
