// -*- verilog -*-
//
//  USRP - Universal Software Radio Peripheral
//
//  Copyright (C) 2003 Matt Ettus
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

// Interface to Cypress FX2 bus
// A packet is 512 Bytes, the fifo has 4096 lines of 18 bits each

`include "../../firmware/include/fpga_regs_common.v"
`include "../../firmware/include/fpga_regs_standard.v"

module rx_buffer
  ( // Read/USB side
    input usbclk,
    input bus_reset,
    output [15:0] usbdata,
    input RD,
    output reg have_pkt_rdy,
    output reg rx_overrun,
    input clear_status,
    // Write/DSP side
    input rxclk,
    input reset,  // DSP side reset (used here), do not reset registers
    input rxstrobe,
    input wire [3:0] channels,
    input wire [15:0] ch_0,
    input wire [15:0] ch_1,
    input wire [15:0] ch_2,
    input wire [15:0] ch_3,
    input wire [15:0] ch_4,
    input wire [15:0] ch_5,
    input wire [15:0] ch_6,
    input wire [15:0] ch_7,
    // Settings, on rxclk also
    input [6:0] serial_addr, input [31:0] serial_data, input serial_strobe,
    input reset_regs, //Only reset registers
    output [31:0] debugbus
    );
   
   wire [15:0] 	  fifodata, fifodata_8;
   reg [15:0] 	  fifodata_16;
   
   wire [11:0] 	  rxfifolevel;
   wire 	  rx_full;
   
   wire 	  bypass_hb, want_q;
   wire [4:0] 	  bitwidth;
   wire [3:0] 	  bitshift;
   
   setting_reg #(`FR_RX_FORMAT) sr_rxformat(.clock(rxclk),.reset(reset_regs),
					    .strobe(serial_strobe),.addr(serial_addr),.in(serial_data),
					    .out({bypass_hb,want_q,bitwidth,bitshift}));

   // USB Read Side of FIFO
   always @(negedge usbclk)
     have_pkt_rdy <= (rxfifolevel >= 256);

   // 257 Bug Fix
   reg [8:0] 	  read_count;
   always @(negedge usbclk)
     if(bus_reset)
       read_count <= 0;
     else if(RD)
       read_count <= read_count + 1;
     else
       read_count <= 0;
   
   // FIFO
   wire 	  ch0_in, ch0_out, iq_out;
   assign 	  ch0_in = (phase == 1);

   fifo_4k_18 rxfifo 
     ( // DSP Write Side
       .data ( {ch0_in, phase[0], fifodata} ),
       .wrreq (~rx_full & (phase != 0)),
       .wrclk ( rxclk ),
       .wrfull ( rx_full ),
       .wrempty ( ),
       .wrusedw ( ),
       // USB Read Side
       .q ( {ch0_out,iq_out,usbdata} ),
       .rdreq ( RD & ~read_count[8] ), 
       .rdclk ( ~usbclk ),
       .rdfull ( ),
       .rdempty ( ),
       .rdusedw ( rxfifolevel ),
       // Async, shared
       .aclr ( reset ) );

   // DSP Write Side of FIFO
   reg [15:0] ch_0_reg;
   reg [15:0] ch_1_reg;
   reg [15:0] ch_2_reg;
   reg [15:0] ch_3_reg;
   reg [15:0] ch_4_reg;
   reg [15:0] ch_5_reg;
   reg [15:0] ch_6_reg;
   reg [15:0] ch_7_reg;

   always @(posedge rxclk)
     if (rxstrobe)
       begin
         ch_0_reg <= ch_0;
         ch_1_reg <= ch_1;
         ch_2_reg <= ch_2;
         ch_3_reg <= ch_3;
         ch_4_reg <= ch_4;
         ch_5_reg <= ch_5;
         ch_6_reg <= ch_6;
         ch_7_reg <= ch_7;
       end

   reg [3:0] phase;
   always @(posedge rxclk)
     if(reset)
       phase <= 4'd0;
     else if(phase == 0)
       begin
	  if(rxstrobe)
	    phase <= 4'd1;
       end
     else if(~rx_full)
       if(phase == ((bitwidth == 5'd8) ? (channels>>1) : channels))
	 phase <= 4'd0;
       else
	 phase <= phase + 4'd1;
   
   assign    fifodata = (bitwidth == 5'd8) ? fifodata_8 : fifodata_16;
   
   assign    fifodata_8 = {round_8(top),round_8(bottom)};
   reg [15:0] top,bottom;
   
   function [7:0] round_8;
      input [15:0] in_val;
      
      round_8 = in_val[15:8] + (in_val[15] & |in_val[7:0]);
   endfunction // round_8
      
   always @*
     case(phase)
       4'd1 : begin
	  bottom = ch_0_reg;
	  top = ch_1_reg;
       end
       4'd2 : begin
	  bottom = ch_2_reg;
	  top = ch_3_reg;
       end
       4'd3 : begin
	  bottom = ch_4_reg;
	  top = ch_5_reg;
       end
       4'd4 : begin
	  bottom = ch_6_reg;
	  top = ch_7_reg;
       end
       default : begin
	  top = 16'hFFFF;
	  bottom = 16'hFFFF;
       end
     endcase // case(phase)
   
   always @*
     case(phase)
       4'd1 : fifodata_16 = ch_0_reg;
       4'd2 : fifodata_16 = ch_1_reg;
       4'd3 : fifodata_16 = ch_2_reg;
       4'd4 : fifodata_16 = ch_3_reg;
       4'd5 : fifodata_16 = ch_4_reg;
       4'd6 : fifodata_16 = ch_5_reg;
       4'd7 : fifodata_16 = ch_6_reg;
       4'd8 : fifodata_16 = ch_7_reg;
       default : fifodata_16 = 16'hFFFF;
     endcase // case(phase)
   
   // Detect overrun
   reg clear_status_dsp, rx_overrun_dsp;
   always @(posedge rxclk)
     clear_status_dsp <= clear_status;

   always @(negedge usbclk)
     rx_overrun <= rx_overrun_dsp;

   always @(posedge rxclk)
     if(reset)
       rx_overrun_dsp <= 1'b0;
     else if(rxstrobe & (phase != 0))
       rx_overrun_dsp <= 1'b1;
     else if(clear_status_dsp)
       rx_overrun_dsp <= 1'b0;

   // Debug bus
   //
   // 15:0  rxclk  domain => TXA 15:0
   // 31:16 usbclk domain => RXA 15:0
   
   assign debugbus[0]     = reset;
   assign debugbus[1]     = reset_regs;
   assign debugbus[2]     = rxstrobe;
   assign debugbus[6:3]   = channels;
   assign debugbus[7]     = rx_full;
   assign debugbus[11:8]  = phase;
   assign debugbus[12]    = ch0_in;
   assign debugbus[13]    = clear_status_dsp;
   assign debugbus[14]    = rx_overrun_dsp;
   assign debugbus[15]    = rxclk;

   assign debugbus[16]    = bus_reset;   
   assign debugbus[17]    = RD;
   assign debugbus[18]    = have_pkt_rdy;
   assign debugbus[19]    = rx_overrun;
   assign debugbus[20]    = read_count[0];
   assign debugbus[21]    = read_count[8];
   assign debugbus[22]    = ch0_out;
   assign debugbus[23]    = iq_out;
   assign debugbus[24]    = clear_status;
   assign debugbus[30:25] = 0;   
   assign debugbus[31]    = usbclk;
   
endmodule // rx_buffer

