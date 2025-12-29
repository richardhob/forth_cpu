
# C Bits

CC = g++
CFLAGS = -g -Og -I$(abspath ../unity)
LDFLAGS = -L$(abspath ..) -lunity

SRC  := word_to_int.c main.c
VSRC := word_to_int.v

ROOT := ../../src
VPATH := $(ROOT) $(ROOT)/compiler
VFLAGS := $(addprefix -I,$(VPATH))

vpath %.v $(VPATH)

.PHONY: all
all: build

.PHONY: run
run: obj_dir/Vword_to_int
	./obj_dir/Vword_to_int

.PHONY: build
build: obj_dir/Vword_to_int

../libunity.a:
	cd .. && make -f unity.mk

obj_dir/Vword_to_int: $(VSRC) $(SRC) | ../libunity.a
	verilator $^ --cc --build --exe --trace --timing -CFLAGS "$(CFLAGS)" -LDFLAGS "$(LDFLAGS)" $(VFLAGS)

.PHONY: clean
clean:
	rm -rf obj_dir
