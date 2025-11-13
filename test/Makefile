
# Build Procedure
#
# - Create Unity library
# - Create test EXEs
# - Run test EXEs

PROJECTS:=fifo clock_divider counter

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
