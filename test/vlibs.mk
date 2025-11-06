
# Expect VSRC to be defined externally ... should include the list of V files to
# verilate. Which live in a directory of the same name
#
# Example:  VSRC=fifo.v
# Lives in: src/fifo/fifo.v
#
VS := $(VSRC:.v=)

VSRC_DIRS := $(addprefix ../,$(VS))
VOBJ_DIR  := $(addprefix obj_dir_,$(VS))

vpath %.v $(VSRC_DIRS)

LDFLAGS := $(addprefix -L,$(VOBJ_DIR)) \
           $(addprefix -lV,$(VS))

.PHONY: vlibs
vlibs: $(VOBJ_DIR)

obj_dir_%: %.v
	verilator $< --cc --Mdir $@
	cd $@; make -f V$(<F:.v=).mk

.PHONY: clean
vlibs_clean:
	rm -rf $(VOBJ_DIR)
