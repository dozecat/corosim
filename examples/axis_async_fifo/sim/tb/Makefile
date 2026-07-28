MODULE    = axis_async_fifo_tb
TOP_SV    = ./$(MODULE).sv
TEST_CPP  = ./$(MODULE).cpp

VERILATOR = verilator

RTL_DIR   = $(abspath ../../rtl)
RTL_FILES = $(shell find $(RTL_DIR) -name "*.sv" -o -name "*.v")

COROSIM_SRC  = $(abspath ../../../../src)
COROSIM_HPP  = $(shell find $(COROSIM_SRC) -name "*.hpp")
COROSIM_CPP  = $(shell find $(COROSIM_SRC) -name "*.cpp")
COROSIM_INC  = -I$(COROSIM_SRC)

BFM_DIR      = $(abspath ../bfm)
BFM_INC      = -I$(BFM_DIR)

FRAME_FIFO     ?= 0
ALWAYS_RECEIVE ?= 0

OBJ_SUFFIX := _f$(FRAME_FIFO)a$(ALWAYS_RECEIVE)
OBJ_DIR   = obj_dir$(OBJ_SUFFIX)
STAMP     = .stamp.verilate$(OBJ_SUFFIX)
EXE       = $(OBJ_DIR)/V$(MODULE)
WAVEFORM  = waveform$(OBJ_SUFFIX).vcd

PARAMS = -GFRAME_FIFO=$(FRAME_FIFO) -GALWAYS_RECEIVE=$(ALWAYS_RECEIVE)

.DEFAULT_GOAL := sim

.PHONY: sim
sim: $(WAVEFORM)

.PHONY: build
build: $(EXE)

.PHONY: waves
waves: $(WAVEFORM)
	gtkwave $(WAVEFORM)

$(WAVEFORM): $(EXE)
	@echo "### SIMULATING ### (FRAME=$(FRAME_FIFO) ALWAYS=$(ALWAYS_RECEIVE))"
	@./$(EXE) $(WAVEFORM)

$(EXE): $(STAMP)
	$(MAKE) -j -C $(OBJ_DIR) -f V$(MODULE).mk

$(STAMP): $(TOP_SV) $(TEST_CPP) $(COROSIM_HPP) $(COROSIM_CPP) FORCE
	@echo "### VERILATING ### (FRAME=$(FRAME_FIFO) ALWAYS=$(ALWAYS_RECEIVE))"
	mkdir -p $(OBJ_DIR)
	$(VERILATOR) --trace --trace-depth 99 --no-trace-params \
	  --x-assign unique --x-initial unique \
	  --top-module $(MODULE) \
	  -cc $(TOP_SV) --exe $(TEST_CPP) $(COROSIM_CPP) \
	  -Mdir $(OBJ_DIR) \
	  -CFLAGS "$(COROSIM_INC) $(BFM_INC) -std=c++20" \
	  $(RTL_FILES) \
	  $(PARAMS) \
	  -public \
	  -Wno-PINCONNECTEMPTY -Wno-NULLPORT -Wno-PROCASSINIT \
	  -Wno-GENUNNAMED -Wno-IMPLICIT -Wno-PINNOTFOUND \
	  -Wno-PINMISSING -Wno-WIDTHEXPAND -Wno-WIDTHTRUNC \
	  -Wno-UNUSEDPARAM -Wno-SYNCASYNCNET -Wno-UNUSED \
	  -Wno-SELRANGE -Wno-DECLFILENAME -Wno-UNSIGNED \
	  -Wno-ASCRANGE -Wno-CMPCONST -Wno-MULTIDRIVEN \
	  -Wno-MULTITOP -Wno-UNOPTFLAT \
	  -Wno-EOFNEWLINE
	@touch $@

FORCE:

.PHONY: frame
frame:
	+$(MAKE) FRAME_FIFO=1

.PHONY: always_recv
always_recv:
	+$(MAKE) ALWAYS_RECEIVE=1

.PHONY: frame-always_recv
frame-always_recv:
	+$(MAKE) FRAME_FIFO=1 ALWAYS_RECEIVE=1

.PHONY: test-all
test-all:
	@echo "=== basic ===" &&     $(MAKE) && \
	echo "=== frame ===" &&      $(MAKE) frame && \
	echo "=== always_recv ===" && $(MAKE) always_recv && \
	echo "=== frame+always_recv ===" && $(MAKE) frame-always_recv

.PHONY: clean
clean:
	rm -rf .stamp.verilate* ./obj_dir* *.vcd
