
# C Bits

CC = g++
CFLAGS = -g -Og -I$(abspath ../unity)
LDFLAGS = -L$(abspath ..) -lunity

SRC  := uart_rx_to_tx.c main.c
VSRC := uart_rx_to_tx.v uart_rx.v uart_tx.v

ROOT := ../../src
VPATH := $(ROOT) $(ROOT)/examples
VFLAGS := $(addprefix -I,$(VPATH))

vpath %.v $(VPATH)

.PHONY: all
all: build

.PHONY: run
run: obj_dir/Vuart_rx_to_tx
	./obj_dir/Vuart_rx_to_tx

.PHONY: build
build: obj_dir/Vuart_rx_to_tx

../libunity.a:
	cd .. && make -f unity.mk

obj_dir/Vuart_rx_to_tx: $(VSRC) $(SRC) | ../libunity.a
	verilator $^ --cc --build --exe --trace --timing -CFLAGS "$(CFLAGS)" -LDFLAGS "$(LDFLAGS)" $(VFLAGS)

.PHONY: clean
clean:
	rm -rf obj_dir
