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
// A packet is 512 Bytes.  Each fifo line is 2 bytes
// Fifo has 1024 or 2048 lines

`include "../../firmware/include/fpga_regs_common.v"
`include "../../firmware/include/fpga_regs_standard.v"

module rx_buffer
  ( input usbclk,
    input bus_reset,  // Not used in RX
    input reset,  // DSP side reset (used here), do not reset registers
    input reset_regs, //Only reset registers
    output [15:0] usbdata,
    input RD,
    output wire have_pkt_rdy,
    output reg rx_overrun,
    input wire [3:0] channels,
    input wire [15:0] ch_0,
    input wire [15:0] ch_1,
    input wire [15:0] ch_2,
    input wire [15:0] ch_3,
    input wire [15:0] ch_4,
    input wire [15:0] ch_5,
    input wire [15:0] ch_6,
    input wire [15:0] ch_7,
    input rxclk,
    input rxstrobe,
    input clear_status,
    input [6:0] serial_addr, input [31:0] serial_data, input serial_strobe,
    output [15:0] debugbus
    );

   wire [15:0] fifodata, fifodata_8;
   reg [15:0]  fifodata_16;
   
   wire [11:0] rxfifolevel;
   wire rx_empty, rx_full;

   wire bypass_hb, want_q;
   wire [4:0] bitwidth;
   wire [3:0] bitshift;
   
   setting_reg #(`FR_RX_FORMAT) sr_rxformat(.clock(rxclk),.reset(reset_regs),
					    .strobe(serial_strobe),.addr(serial_addr),.in(serial_data),
					    .out({bypass_hb,want_q,bitwidth,bitshift}));

   // Receive FIFO (ADC --> USB)

   // 257 Bug Fix
   reg [8:0] read_count;
   always @(negedge usbclk)
     if(bus_reset)
       read_count <= #1 9'd0;
     else if(RD & ~read_count[8])
       read_count <= #1 read_count + 9'd1;
     else
       read_count <= #1 RD ? read_count : 9'b0;
   
   // Detect overrun
   always @(posedge rxclk)
     if(reset)
       rx_overrun <= 1'b0;
     else if(rxstrobe & (store_next != 0))
       rx_overrun <= 1'b1;
     else if(clear_status)
       rx_overrun <= 1'b0;

   reg [3:0] store_next;
   always @(posedge rxclk)
     if(reset)
       store_next <= #1 4'd0;
     else if(rxstrobe & (store_next == 0))
       store_next <= #1 4'd1;
     else if(~rx_full & (store_next == channels))
       store_next <= #1 4'd0;
     else if(~rx_full & (bitwidth == 5'd8) & (store_next == (channels>>1)))
       store_next <= #1 4'd0;
     else if(~rx_full & (store_next != 0))
       store_next <= #1 store_next + 4'd1;

   assign    fifodata = (bitwidth == 5'd8) ? fifodata_8 : fifodata_16;

   assign    fifodata_8 = {round_8(top),round_8(bottom)};
   reg [15:0] top,bottom;

   function [7:0] round_8;
      input [15:0] in_val;

      round_8 = in_val[15:8] + (in_val[15] & |in_val[7:0]);
   endfunction // round_8
      
   always @*
     case(store_next)
       4'd1 : begin
	  bottom = ch_0;
	  top = ch_1;
       end
       4'd2 : begin
	  bottom = ch_2;
	  top = ch_3;
       end
       4'd3 : begin
	  bottom = ch_4;
	  top = ch_5;
       end
       4'd4 : begin
	  bottom = ch_6;
	  top = ch_7;
       end
       default : begin
	  top = 16'hFFFF;
	  bottom = 16'hFFFF;
       end
     endcase // case(store_next)
   
   always @*
     case(store_next)
       4'd1 : fifodata_16 = ch_0;
       4'd2 : fifodata_16 = ch_1;
       4'd3 : fifodata_16 = ch_2;
       4'd4 : fifodata_16 = ch_3;
       4'd5 : fifodata_16 = ch_4;
       4'd6 : fifodata_16 = ch_5;
       4'd7 : fifodata_16 = ch_6;
       4'd8 : fifodata_16 = ch_7;
       default : fifodata_16 = 16'hFFFF;
     endcase // case(store_next)
   
   fifo_4k rxfifo 
     ( .data ( fifodata ),
       .wrreq (~rx_full & (store_next != 0)),
       .wrclk ( rxclk ),

       .q ( usbdata ),
       .rdreq ( RD & ~read_count[8] ), 
       .rdclk ( ~usbclk ),
       
       .aclr ( reset ),  // This one is asynchronous, so we can use either reset
       
       .rdempty ( rx_empty ),
       .rdusedw ( rxfifolevel ),
       .wrfull ( rx_full ),
       .wrusedw (  )
       );
   
   assign have_pkt_rdy = (rxfifolevel >= 256);

   // Debugging Aids
   assign debugbus[0] = RD;
   assign debugbus[1] = rx_overrun;
   assign debugbus[2] = read_count[8];
   assign debugbus[3] = rx_full;
   assign debugbus[4] = rxstrobe;
   assign debugbus[5] = usbclk;
   assign debugbus[6] = have_pkt_rdy;
   assign debugbus[10:7] = store_next;
   //assign debugbus[15:11] = rxfifolevel[4:0];
   assign debugbus[15:11] = bitwidth;
   
endmodule // rx_buffer

