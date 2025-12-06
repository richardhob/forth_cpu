
# C Bits

CC = g++
CFLAGS = -g -Og -I$(abspath ../unity)
LDFLAGS = -L$(abspath ..) -lunity

SRC  := uart_data_bits.c main.c
VSRC := uart_data_bits.v

ROOT := ../../src
VPATH := $(ROOT)/uart_rx $(ROOT)
VFLAGS := $(addprefix -I,$(VPATH))

vpath %.v $(VPATH)

.PHONY: all
all: build

.PHONY: run
run: obj_dir/Vuart_data_bits
	./obj_dir/Vuart_data_bits

.PHONY: build
build: obj_dir/Vuart_data_bits

../libunity.a:
	cd .. && make -f unity.mk

obj_dir/Vuart_data_bits: $(VSRC) $(SRC) | ../libunity.a
	verilator $^ --cc --build --exe --trace --timing -CFLAGS "$(CFLAGS)" -LDFLAGS "$(LDFLAGS)" $(VFLAGS)

.PHONY: clean
clean:
	rm -rf obj_dir
