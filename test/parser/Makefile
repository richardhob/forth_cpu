
# C Bits

CC = g++
CFLAGS = -g -Og -I$(abspath ../unity)
LDFLAGS = -L$(abspath ..) -lunity

SRC  := parser.c main.c
VSRC := parser.v

ROOT := ../../src
VPATH := $(ROOT) $(ROOT)/compiler
VFLAGS := $(addprefix -I,$(VPATH))

vpath %.v $(VPATH)

.PHONY: all
all: build

.PHONY: run
run: obj_dir/Vparser
	./obj_dir/Vparser

.PHONY: build
build: obj_dir/Vparser

../libunity.a:
	cd .. && make -f unity.mk

obj_dir/Vparser: $(VSRC) $(SRC) | ../libunity.a
	verilator $^ --cc --build --exe --trace --timing -CFLAGS "$(CFLAGS)" -LDFLAGS "$(LDFLAGS)" $(VFLAGS)

.PHONY: clean
clean:
	rm -rf obj_dir
