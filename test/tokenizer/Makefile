
# C Bits

CC = g++
CFLAGS = -g -Og -I$(abspath ../unity)
LDFLAGS = -L$(abspath ..) -lunity

SRC  := tokenizer.c main.c
VSRC := tokenizer.v

ROOT := ../../src
VPATH := $(ROOT) $(ROOT)/compiler
VFLAGS := $(addprefix -I,$(VPATH))

vpath %.v $(VPATH)

.PHONY: all
all: build

.PHONY: run
run: obj_dir/Vtokenizer
	./obj_dir/Vtokenizer

.PHONY: build
build: obj_dir/Vtokenizer

../libunity.a:
	cd .. && make -f unity.mk

obj_dir/Vtokenizer: $(VSRC) $(SRC) | ../libunity.a
	verilator $^ --cc --build --exe --trace --timing -CFLAGS "$(CFLAGS)" -LDFLAGS "$(LDFLAGS)" $(VFLAGS)

.PHONY: clean
clean:
	rm -rf obj_dir
