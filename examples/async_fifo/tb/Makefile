###############################################################################
# Copyright (C) 2025 dozecat. All rights reserved.
# SPDX-License-Identifier: MIT
#
# @file        Makefile
# @brief       Build and run async_fifo testbench using Verilator + corosim
# @see         https://github.com/dozecat/corosim
#
# Usage:
#   make          — build + simulate
#   make sim     — build + simulate
#   make build   — compile only
#   make waves   — view waveform (GTKWave)
#   make clean   — remove generated files
#
# Modification History:
# Ver   Who  Date        Changes
# ----  ---- ----------  -----------------------------------------------------
# 1.0        2026/07/25  Initial release
###############################################################################

MODULE    = async_fifo
VERILATOR = verilator

RTL_FILES = $(abspath ../rtl/async_fifo.v)

TEST_CPP  = ./async_fifo_tb.cpp

COROSIM_SRC  = $(abspath ../../../src)
COROSIM_HPP  = $(shell find $(COROSIM_SRC) -name "*.hpp")
COROSIM_CPP  = $(shell find $(COROSIM_SRC) -name "*.cpp")
COROSIM_INC  = -I$(COROSIM_SRC)

# Default target: build and simulate
.DEFAULT_GOAL := sim

.PHONY: sim
sim: waveform.vcd

.PHONY: build
build: obj_dir/V$(MODULE)

.PHONY: waves
waves: waveform.vcd
	@echo
	@echo "### WAVES ###"
	gtkwave waveform.vcd

waveform.vcd: obj_dir/V$(MODULE)
	@echo
	@echo "### SIMULATING ###"
	@./obj_dir/V$(MODULE)

obj_dir/V$(MODULE): $(RTL_FILES) $(TEST_CPP) $(COROSIM_HPP) $(COROSIM_CPP)
	@echo
	@echo "### VERILATING ###"
	$(VERILATOR) -cc --exe --build -j 4 \
		--top-module $(MODULE) \
		-Wno-DEPRECATED \
		-GDEPTH=16 \
		$(RTL_FILES) $(TEST_CPP) $(COROSIM_CPP) \
		-CFLAGS "$(COROSIM_INC) -std=c++20" \
		--trace

.PHONY: clean
clean:
	rm -rf obj_dir waveform.vcd
