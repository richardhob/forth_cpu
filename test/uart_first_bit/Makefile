
# C Bits

CC = g++
CFLAGS = -g -Og -I$(abspath ../unity)
LDFLAGS = -L$(abspath ..) -lunity

SRC  := uart_first_bit.c main.c
VSRC := uart_first_bit.v

ROOT := ../../src
VPATH := $(ROOT)/uart_rx $(ROOT)
VFLAGS := $(addprefix -I,$(VPATH))

vpath %.v $(VPATH)

.PHONY: all
all: build

.PHONY: run
run: obj_dir/Vuart_first_bit
	./obj_dir/Vuart_first_bit

.PHONY: build
build: obj_dir/Vuart_first_bit

../libunity.a:
	cd .. && make -f unity.mk

obj_dir/Vuart_first_bit: $(VSRC) $(SRC) | ../libunity.a
	verilator $^ --cc --build --exe --trace --timing -CFLAGS "$(CFLAGS)" -LDFLAGS "$(LDFLAGS)" $(VFLAGS)

.PHONY: clean
clean:
	rm -rf obj_dir
