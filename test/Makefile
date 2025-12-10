
# Build Procedure
#
# - Create Unity library
# - Create test EXEs
# - Run test EXEs

_PROJECTS:=clock_divider\
          counter\
          fifo\
          shift\
          uart_data_bits\
          uart_first_bit\
          uart_stop_bits\
          uart_tx
PROJECTS=$(abspath $(_PROJECTS))

.PHONY: all
all: lib build run

.PHONY: lib
lib: 
	make -f unity.mk

.PHONY: build
build:
	for p in $(PROJECTS) ; do \
		cd $$p && make ; \
	done

.PHONY: run
run:
	for p in $(PROJECTS) ; do \
		cd $$p && make run ; \
	done

.PHONY: clean
clean: 
	make -f unity.mk unity_clean
	for p in $(PROJECTS) ; do \
		cd $$p && make clean ; \
	done
