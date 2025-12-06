
# C Bits

CC = g++
CFLAGS = -g -Og -I$(abspath ../unity)
LDFLAGS = -L$(abspath ..) -lunity

SRC  := uart_stop_bits.c main.c
VSRC := uart_stop_bits.v

ROOT := ../../src
VPATH := $(ROOT)/uart_rx $(ROOT)
VFLAGS := $(addprefix -I,$(VPATH))

vpath %.v $(VPATH)

.PHONY: all
all: build

.PHONY: run
run: obj_dir/Vuart_stop_bits
	./obj_dir/Vuart_stop_bits

.PHONY: build
build: obj_dir/Vuart_stop_bits

../libunity.a:
	cd .. && make -f unity.mk

obj_dir/Vuart_stop_bits: $(VSRC) $(SRC) | ../libunity.a
	verilator $^ --cc --build --exe --trace --timing -CFLAGS "$(CFLAGS)" -LDFLAGS "$(LDFLAGS)" $(VFLAGS)

.PHONY: clean
clean:
	rm -rf obj_dir
