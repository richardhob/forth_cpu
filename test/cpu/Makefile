
# C Bits

CC = g++
CFLAGS = -g -Og -I$(abspath ../unity)
LDFLAGS = -L$(abspath ..) -lunity

SRC  := cpu.c main.c operations.c
VSRC := cpu.v

ROOT := ../../src
VPATH := $(ROOT) $(ROOT)/compiler
VFLAGS := $(addprefix -I,$(VPATH))

vpath %.v $(VPATH)

.PHONY: all
all: build

.PHONY: run
run: obj_dir/Vcpu
	./obj_dir/Vcpu

.PHONY: build
build: obj_dir/Vcpu

../libunity.a:
	cd .. && make -f unity.mk

obj_dir/Vcpu: $(VSRC) $(SRC) | ../libunity.a
	verilator $^ --cc --build --exe --trace --timing -CFLAGS "$(CFLAGS)" -LDFLAGS "$(LDFLAGS)" $(VFLAGS)

.PHONY: clean
clean:
	rm -rf obj_dir
