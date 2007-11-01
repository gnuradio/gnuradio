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

module tx_buffer
  ( // USB Side
    input usbclk,
    input bus_reset,  // Used here for the 257-Hack to fix the FX2 bug
    input [15:0] usbdata,
    input wire WR,
    output reg have_space,
    output reg tx_underrun,
    input clear_status,

    // DSP Side
    input txclk,
    input reset,  // standard DSP-side reset
    input wire [3:0] channels,
    output reg [15:0] tx_i_0,
    output reg [15:0] tx_q_0,
    output reg [15:0] tx_i_1,
    output reg [15:0] tx_q_1,
    input txstrobe,
    output wire tx_empty,
    output [31:0] debugbus
    );
   
   wire [11:0] 	  txfifolevel;
   wire [15:0] 	  fifodata;
   wire 	  rdreq;
   reg [3:0] 	  phase;
   wire 	  sop_f, iq_f;
   reg 		  sop;
   
   // USB Side of FIFO
   reg [15:0] 	  usbdata_reg;
   reg 		  wr_reg;
   reg [8:0] 	  write_count;
   
   always @(posedge usbclk)
     have_space <= (txfifolevel < (4092-256));  // be extra conservative
   
   always @(posedge usbclk)
     begin
	wr_reg <= WR;
	usbdata_reg <= usbdata;
     end
   
   always @(posedge usbclk)
     if(bus_reset)
       write_count <= 0;
     else if(wr_reg)
       write_count <= write_count + 1;
     else
       write_count <= 0;
   
   always @(posedge usbclk)
     sop <= WR & ~wr_reg; // Edge detect
   
   // FIFO
   fifo_4k_18 txfifo 
     ( // USB Write Side
       .data ( {sop,write_count[0],usbdata_reg} ),
       .wrreq ( wr_reg & ~write_count[8] ),
       .wrclk ( usbclk ),
       .wrfull ( ),
       .wrempty ( ),
       .wrusedw ( txfifolevel ),
       // DSP Read Side
       .q ( {sop_f, iq_f, fifodata} ),			
       .rdreq ( rdreq ),
       .rdclk ( txclk ),
       .rdfull ( ),
       .rdempty ( tx_empty ),
       .rdusedw (  ),
       // Async, shared
       .aclr ( reset ) );
   
   // DAC Side of FIFO
   always @(posedge txclk)
     if(reset)
       begin
	  {tx_i_0,tx_q_0,tx_i_1,tx_q_1} <= 64'h0;
	  phase <= 4'd0;
       end
     else if(phase == channels)
       begin
	  if(txstrobe)
	    phase <= 4'd0;
       end
     else
       if(~tx_empty)
	 begin
	    case(phase)
	      4'd0 : tx_i_0 <= fifodata;
	      4'd1 : tx_q_0 <= fifodata;
	      4'd2 : tx_i_1 <= fifodata;
	      4'd3 : tx_q_1 <= fifodata;
	    endcase // case(phase)
	    phase <= phase + 4'd1;
	 end
      
   assign    rdreq = ((phase != channels) & ~tx_empty);
   
   // Detect Underruns, cross clock domains
   reg clear_status_dsp, tx_underrun_dsp;
   always @(posedge txclk)
     clear_status_dsp <= clear_status;

   always @(posedge usbclk)
     tx_underrun <= tx_underrun_dsp;
	    
   always @(posedge txclk)
     if(reset)
       tx_underrun_dsp <= 1'b0;
     else if(txstrobe & (phase != channels))
       tx_underrun_dsp <= 1'b1;
     else if(clear_status_dsp)
       tx_underrun_dsp <= 1'b0;

   // TX debug bus
   // 
   // 15:0  txclk  domain => TXA [15:0]
   // 31:16 usbclk domain => RXA [15:0]
   
   assign debugbus[0]     = reset;
   assign debugbus[1]     = txstrobe;
   assign debugbus[2]     = rdreq;
   assign debugbus[6:3]   = phase;
   assign debugbus[7]     = tx_empty;
   assign debugbus[8]     = tx_underrun_dsp;
   assign debugbus[9]     = iq_f;
   assign debugbus[10]    = sop_f;
   assign debugbus[14:11] = 0;
   assign debugbus[15]    = txclk;
	  
   assign debugbus[16]    = bus_reset;
   assign debugbus[17]    = WR;
   assign debugbus[18]    = wr_reg;
   assign debugbus[19]    = have_space;
   assign debugbus[20]    = write_count[8];
   assign debugbus[21]    = write_count[0];
   assign debugbus[22]    = sop;
   assign debugbus[23]    = tx_underrun;
   assign debugbus[30:24] = 0;
   assign debugbus[31]    = usbclk;
          
endmodule // tx_buffer

