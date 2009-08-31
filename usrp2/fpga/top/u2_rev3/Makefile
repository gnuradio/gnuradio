#
# Copyright 2008 Ettus Research LLC
# 
# This file is part of GNU Radio
# 
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
# 
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
# 

##################################################
# xtclsh Shell and tcl Script Path
##################################################
#XTCLSH := /opt/Xilinx/10.1/ISE/bin/lin/xtclsh
XTCLSH := xtclsh
ISE_HELPER := ../tcl/ise_helper.tcl

##################################################
# Project Setup
##################################################
BUILD_DIR := build/
export TOP_MODULE := u2_rev3
export PROJ_FILE := $(BUILD_DIR)$(TOP_MODULE).ise

##################################################
# Project Properties
##################################################
export PROJECT_PROPERTIES := \
family Spartan3 \
device xc3s2000 \
package fg456 \
speed -5 \
top_level_module_type "HDL" \
synthesis_tool "XST (VHDL/Verilog)" \
simulator "ISE Simulator (VHDL/Verilog)" \
"Preferred Language" "Verilog" \
"Enable Message Filtering" FALSE \
"Display Incremental Messages" FALSE 

##################################################
# Sources
##################################################
export SOURCE_ROOT := ../../../
export SOURCES := \
control_lib/CRC16_D16.v \
control_lib/atr_controller.v \
control_lib/bin2gray.v \
control_lib/newfifo/buffer_int.v \
control_lib/newfifo/buffer_pool.v \
control_lib/cascadefifo2.v \
control_lib/dcache.v \
control_lib/decoder_3_8.v \
control_lib/dpram32.v \
control_lib/fifo_2clock.v \
control_lib/fifo_2clock_casc.v \
control_lib/newfifo/newfifo_2clock.v \
control_lib/newfifo/cascadefifo_2clock.v \
control_lib/gray2bin.v \
control_lib/gray_send.v \
control_lib/icache.v \
control_lib/longfifo.v \
control_lib/mux4.v \
control_lib/mux8.v \
control_lib/nsgpio.v \
control_lib/ram_2port.v \
control_lib/ram_harv_cache.v \
control_lib/ram_loader.v \
control_lib/setting_reg.v \
control_lib/settings_bus.v \
control_lib/shortfifo.v \
control_lib/medfifo.v \
control_lib/srl.v \
control_lib/system_control.v \
control_lib/wb_1master.v \
control_lib/wb_readback_mux.v \
control_lib/simple_uart.v \
control_lib/simple_uart_tx.v \
control_lib/simple_uart_rx.v \
control_lib/oneshot_2clk.v \
control_lib/sd_spi.v \
control_lib/sd_spi_wb.v \
control_lib/wb_bridge_16_32.v \
control_lib/reset_sync.v \
simple_gemac/simple_gemac_wrapper.v \
simple_gemac/simple_gemac.v \
simple_gemac/simple_gemac_wb.v \
simple_gemac/simple_gemac_tx.v \
simple_gemac/simple_gemac_rx.v \
simple_gemac/crc.v \
simple_gemac/delay_line.v \
simple_gemac/flow_ctrl_tx.v \
simple_gemac/address_filter.v \
control_lib/newfifo/ll8_shortfifo.v \
control_lib/newfifo/ll8_to_fifo36.v \
simple_gemac/ll8_to_txmac.v \
simple_gemac/rxmac_to_ll8.v \
control_lib/newfifo/fifo_short.v \
control_lib/newfifo/fifo36_to_ll8.v \
coregen/fifo_xlnx_2Kx36_2clk.v \
coregen/fifo_xlnx_2Kx36_2clk.xco \
coregen/fifo_xlnx_512x36_2clk.v \
coregen/fifo_xlnx_512x36_2clk.xco \
eth/mac_rxfifo_int.v \
eth/mac_txfifo_int.v \
eth/rtl/verilog/Clk_ctrl.v \
eth/rtl/verilog/MAC_rx.v \
eth/rtl/verilog/MAC_rx/Broadcast_filter.v \
eth/rtl/verilog/MAC_rx/CRC_chk.v \
eth/rtl/verilog/MAC_rx/MAC_rx_FF.v \
eth/rtl/verilog/MAC_rx/MAC_rx_add_chk.v \
eth/rtl/verilog/MAC_rx/MAC_rx_ctrl.v \
eth/rtl/verilog/MAC_top.v \
eth/rtl/verilog/MAC_tx.v \
eth/rtl/verilog/MAC_tx/CRC_gen.v \
eth/rtl/verilog/MAC_tx/MAC_tx_FF.v \
eth/rtl/verilog/MAC_tx/MAC_tx_addr_add.v \
eth/rtl/verilog/MAC_tx/MAC_tx_ctrl.v \
eth/rtl/verilog/MAC_tx/Random_gen.v \
eth/rtl/verilog/Phy_int.v \
eth/rtl/verilog/RMON.v \
eth/rtl/verilog/RMON/RMON_addr_gen.v \
eth/rtl/verilog/RMON/RMON_ctrl.v \
eth/rtl/verilog/Reg_int.v \
eth/rtl/verilog/eth_miim.v \
eth/rtl/verilog/flow_ctrl_rx.v \
eth/rtl/verilog/flow_ctrl_tx.v \
eth/rtl/verilog/miim/eth_clockgen.v \
eth/rtl/verilog/miim/eth_outputcontrol.v \
eth/rtl/verilog/miim/eth_shiftreg.v \
extram/wb_zbt16_b.v \
opencores/8b10b/decode_8b10b.v \
opencores/8b10b/encode_8b10b.v \
opencores/aemb/rtl/verilog/aeMB_bpcu.v \
opencores/aemb/rtl/verilog/aeMB_core_BE.v \
opencores/aemb/rtl/verilog/aeMB_ctrl.v \
opencores/aemb/rtl/verilog/aeMB_edk32.v \
opencores/aemb/rtl/verilog/aeMB_ibuf.v \
opencores/aemb/rtl/verilog/aeMB_regf.v \
opencores/aemb/rtl/verilog/aeMB_xecu.v \
opencores/i2c/rtl/verilog/i2c_master_bit_ctrl.v \
opencores/i2c/rtl/verilog/i2c_master_byte_ctrl.v \
opencores/i2c/rtl/verilog/i2c_master_defines.v \
opencores/i2c/rtl/verilog/i2c_master_top.v \
opencores/i2c/rtl/verilog/timescale.v \
opencores/simple_pic/rtl/simple_pic.v \
opencores/spi/rtl/verilog/spi_clgen.v \
opencores/spi/rtl/verilog/spi_defines.v \
opencores/spi/rtl/verilog/spi_shift.v \
opencores/spi/rtl/verilog/spi_top.v \
opencores/spi/rtl/verilog/timescale.v \
sdr_lib/acc.v \
sdr_lib/add2.v \
sdr_lib/add2_and_round.v \
sdr_lib/add2_and_round_reg.v \
sdr_lib/add2_reg.v \
sdr_lib/cic_dec_shifter.v \
sdr_lib/cic_decim.v \
sdr_lib/cic_int_shifter.v \
sdr_lib/cic_interp.v \
sdr_lib/cic_strober.v \
sdr_lib/clip.v \
sdr_lib/clip_reg.v \
sdr_lib/cordic.v \
sdr_lib/cordic_z24.v \
sdr_lib/cordic_stage.v \
sdr_lib/dsp_core_rx.v \
sdr_lib/dsp_core_tx.v \
sdr_lib/hb_dec.v \
sdr_lib/hb_interp.v \
sdr_lib/round.v \
sdr_lib/round_reg.v \
sdr_lib/rx_control.v \
sdr_lib/rx_dcoffset.v \
sdr_lib/sign_extend.v \
sdr_lib/small_hb_dec.v \
sdr_lib/small_hb_int.v \
sdr_lib/tx_control.v \
serdes/serdes.v \
serdes/serdes_fc_rx.v \
serdes/serdes_fc_tx.v \
serdes/serdes_rx.v \
serdes/serdes_tx.v \
timing/time_receiver.v \
timing/time_sender.v \
timing/time_sync.v \
timing/timer.v \
top/u2_core/u2_core.v \
top/u2_rev3/u2_rev3.ucf \
top/u2_rev3/u2_rev3.v 

##################################################
# Process Properties
##################################################
export SYNTHESIZE_PROPERTIES := \
"Number of Clock Buffers" 6 \
"Pack I/O Registers into IOBs" Yes \
"Optimization Effort" High \
"Optimize Instantiated Primitives" TRUE \
"Register Balancing" Yes \
"Use Clock Enable" Auto \
"Use Synchronous Reset" Auto \
"Use Synchronous Set" Auto

export TRANSLATE_PROPERTIES := \
"Macro Search Path" "$(shell pwd)/../../coregen/"

export MAP_PROPERTIES := \
"Allow Logic Optimization Across Hierarchy" TRUE \
"Map to Input Functions" 4 \
"Optimization Strategy (Cover Mode)" Speed \
"Pack I/O Registers/Latches into IOBs" "For Inputs and Outputs" \
"Perform Timing-Driven Packing and Placement" TRUE \
"Map Effort Level" High \
"Extra Effort" Normal \
"Combinatorial Logic Optimization" TRUE \
"Register Duplication" TRUE

export PLACE_ROUTE_PROPERTIES := \
"Place & Route Effort Level (Overall)" High 

export STATIC_TIMING_PROPERTIES := \
"Number of Paths in Error/Verbose Report" 10 \
"Report Type" "Error Report"

export GEN_PROG_FILE_PROPERTIES := \
"Configuration Rate" 6 \
"Create Binary Configuration File" TRUE \
"Done (Output Events)" 5 \
"Enable Bitstream Compression" TRUE \
"Enable Outputs (Output Events)" 6 

export SIM_MODEL_PROPERTIES := ""

##################################################
# Make Options
##################################################
all:
	@echo make proj, check, synth, bin, or clean

proj:
	PROCESS_RUN="" $(XTCLSH) $(ISE_HELPER)	

check:
	PROCESS_RUN="Check Syntax" $(XTCLSH) $(ISE_HELPER)	

synth:
	PROCESS_RUN="Synthesize - XST" $(XTCLSH) $(ISE_HELPER)	

bin:
	PROCESS_RUN="Generate Programming File" $(XTCLSH) $(ISE_HELPER)		

clean:
	rm -rf $(BUILD_DIR)


