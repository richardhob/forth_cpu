
# C Bits

CC = g++
CFLAGS = -g -Og -I$(abspath ../unity)
LDFLAGS = -L$(abspath ..) -lunity

SRC  := find_index.c main.c
VSRC := find_index.v

ROOT := ../../src
VPATH := $(ROOT)
VFLAGS := $(addprefix -I,$(VPATH))

vpath %.v $(VPATH)

.PHONY: all
all: build

.PHONY: run
run: obj_dir/Vfind_index
	./obj_dir/Vfind_index

.PHONY: build
build: obj_dir/Vfind_index

../libunity.a:
	cd .. && make -f unity.mk

obj_dir/Vfind_index: $(VSRC) $(SRC) | ../libunity.a
	verilator $^ --cc --build --exe --trace --timing -CFLAGS "$(CFLAGS)" -LDFLAGS "$(LDFLAGS)" $(VFLAGS)

.PHONY: clean
clean:
	rm -rf obj_dir
