// -*- verilog -*-
//
//  USRP - Universal Software Radio Peripheral
//
//  Copyright (C) 2006 Martin Dudok van Heel
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Boston, MA  02110-1301  USA
//
`include "config.vh"
`include "../../../firmware/include/fpga_regs_common.v"
`include "../../../firmware/include/fpga_regs_standard.v"
// Clock, enable, and reset controls for whole system
// Modified version to enable multi_usrp synchronisation

module master_control_multi
  ( input master_clk, input usbclk,
    input wire [6:0] serial_addr, input wire [31:0] serial_data, input wire serial_strobe,
    input wire rx_slave_sync,
    output tx_bus_reset, output rx_bus_reset,
    output wire tx_dsp_reset, output wire rx_dsp_reset,
    output wire enable_tx, output wire enable_rx,
    output wire sync_rx,
    output wire [7:0] interp_rate, output wire [7:0] decim_rate,
    output tx_sample_strobe, output strobe_interp,
    output rx_sample_strobe, output strobe_decim,
    input tx_empty,
    input wire [15:0] debug_0,input wire [15:0] debug_1,input wire [15:0] debug_2,input wire [15:0] debug_3,
    output wire [15:0] reg_0, output wire [15:0] reg_1, output wire [15:0] reg_2, output wire [15:0] reg_3
    );
   
   wire [15:0] reg_1_std;

   master_control master_control_standard
     ( .master_clk(master_clk),.usbclk(usbclk),
       .serial_addr(serial_addr),.serial_data(serial_data),.serial_strobe(serial_strobe),
       .tx_bus_reset(tx_bus_reset),.rx_bus_reset(rx_bus_reset),
       .tx_dsp_reset(tx_dsp_reset),.rx_dsp_reset(rx_dsp_reset),
       .enable_tx(enable_tx),.enable_rx(enable_rx),
       .interp_rate(interp_rate),.decim_rate(decim_rate),
       .tx_sample_strobe(tx_sample_strobe),.strobe_interp(strobe_interp),
       .rx_sample_strobe(rx_sample_strobe),.strobe_decim(strobe_decim),
       .tx_empty(tx_empty),
       .debug_0(debug_0),.debug_1(debug_1),
       .debug_2(debug_2),.debug_3(debug_3),
       .reg_0(reg_0),.reg_1(reg_1_std),.reg_2(reg_2),.reg_3(reg_3) );

   // FIXME need a separate reset for all control settings 
   // Master/slave Controls assignments
   wire [7:0] rx_master_slave_controls;
   setting_reg_masked #(`FR_RX_MASTER_SLAVE) sr_rx_mstr_slv_ctrl(.clock(master_clk),.reset(1'b0),.strobe(serial_strobe),.addr(serial_addr),.in(serial_data),.out(rx_master_slave_controls));
 
   assign     sync_rx = rx_master_slave_controls[`bitnoFR_RX_SYNC] | (rx_master_slave_controls[`bitnoFR_RX_SYNC_SLAVE] & rx_slave_sync);
  //sync if we are told by master_control or if we get a hardware slave sync
  //TODO There can be a one sample difference between master and slave sync. 
  //     Maybe use a register for sync_rx which uses the (neg or pos) edge of master_clock and/or rx_slave_sync to trigger
  //     Or even use a seperate sync_rx_out and sync_rx_internal (which lags behind)
  //TODO make output pin not hardwired
assign reg_1 ={(rx_master_slave_controls[`bitnoFR_RX_SYNC_MASTER])? sync_rx:reg_1_std[15],reg_1_std[14:0]};

   
endmodule // master_control
