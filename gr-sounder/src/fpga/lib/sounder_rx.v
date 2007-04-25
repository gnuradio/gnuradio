// -*- verilog -*-
//
//  USRP - Universal Software Radio Peripheral
//
//  Copyright (C) 2007 Corgan Enterprises LLC
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

module sounder_rx(clk_i,rst_i,ena_i,rx_strobe_i,tx_strobe_i,mask_i,len_i,
		  rx_in_i_i,rx_in_q_i,rx_i_o,rx_q_o,rx_strobe_o);
   
   input         clk_i;		// Master clock
   input         rst_i;         // Subsystem reset
   input         ena_i;		// Subsystem enable
   input         rx_strobe_i;   // Strobe every received sample
   input         tx_strobe_i;	// Strobe every transmitted sample
    
   input  [15:0] mask_i;	// PN code LFSR mask
   input  [15:0] len_i;		// PN code LFSR sequence length
   input  [15:0] rx_in_i_i;	// I channel on receive
   input  [15:0] rx_in_q_i;	// Q channel on receive

   output [15:0] rx_i_o;	// I channel of impulse response
   output [15:0] rx_q_o;	// Q channel of impulse response
   output        rx_strobe_o;   // Impulse response value ready

   // LFSR phase counter
   reg [15:0] count;
   wire cycle = (count == (len_i - 1));

   always @(posedge clk_i)
     if (rst_i | ~ena_i)
       count <= 16'b0;
     else
       if (cycle)
	 count <= 16'b0;
       else
	 count <= count + 16'b1;
           
   // Retard LFSR phase once per cycle
   wire lfsr_strobe = (tx_strobe_i & ~cycle);

   // Recreate local reference of transmitted PN code
   wire pn;
   lfsr reference
     ( .clk_i(clk_i),.rst_i(rst_i),.ena_i(ena_i),.strobe_i(lfsr_strobe),.mask_i(mask_i),.pn_o(pn) );

   wire [31:0] rx_i_ext, rx_q_ext;
   sign_extend #(16,32) i_extend(rx_in_i_i, rx_i_ext);
   sign_extend #(16,32) q_extend(rx_in_q_i, rx_q_ext);

   reg [31:0] accum;
   always @(posedge clk_i)
     if (rst_i | ~ena_i)
       accum <= 32'b0;
     else
       if (rx_strobe_i)
	 if (cycle)
	   accum <= 32'b0;
	 else
	   if (pn)
	     accum <= accum + rx_i_ext;
	   else
	     accum <= accum - rx_i_ext;
   
   assign rx_i_o = accum[31:16];
   assign rx_q_o = accum[15:0];
   assign rx_strobe_o = rx_strobe_i & cycle;

endmodule // sounder_rx

