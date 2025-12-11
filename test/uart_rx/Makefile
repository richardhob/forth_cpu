
# C Bits

CC = g++
CFLAGS = -g -Og -I$(abspath ../unity)
LDFLAGS = -L$(abspath ..) -lunity

SRC  := uart_rx.c main.c
VSRC := uart_rx.v

ROOT := ../../src
VPATH := $(ROOT)
VFLAGS := $(addprefix -I,$(VPATH))

vpath %.v $(VPATH)

.PHONY: all
all: build

.PHONY: run
run: obj_dir/Vuart_rx
	./obj_dir/Vuart_rx

.PHONY: build
build: obj_dir/Vuart_rx

../libunity.a:
	cd .. && make -f unity.mk

obj_dir/Vuart_rx: $(VSRC) $(SRC) | ../libunity.a
	verilator $^ --cc --build --exe --trace --timing -CFLAGS "$(CFLAGS)" -LDFLAGS "$(LDFLAGS)" $(VFLAGS)

.PHONY: clean
clean:
	rm -rf obj_dir
