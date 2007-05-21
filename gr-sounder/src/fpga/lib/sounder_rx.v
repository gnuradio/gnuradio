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

module sounder_rx(clk_i,rst_i,ena_i,rx_strobe_i,tx_strobe_i,mask_i,degree_i,len_i,
		  rx_in_i_i,rx_in_q_i,rx_i_o,rx_q_o,rx_strobe_o,
		  loop_i);
   
   input         clk_i;		// Master clock
   input         rst_i;         // Subsystem reset
   input         ena_i;		// Subsystem enable
   input         rx_strobe_i;   // Strobe every received sample
   input         tx_strobe_i;	// Strobe every transmitted sample
    
   input  [15:0] mask_i;	// PN code LFSR mask
   input  [4:0]  degree_i;	// PN code LFSR sequency degree
   input  [15:0] len_i;		// PN code LFSR sequence length
   input  [15:0] rx_in_i_i;	// I channel on receive
   input  [15:0] rx_in_q_i;	// Q channel on receive

   output [15:0] rx_i_o;	// I channel of impulse response
   output [15:0] rx_q_o;	// Q channel of impulse response
   output        rx_strobe_o;   // Impulse response value ready

   input         loop_i;        // Implement loopback
   
   wire strobe_in  = loop_i ? tx_strobe_i : rx_strobe_i;
   wire [16:0] len = loop_i ? (len_i - 1) : ((len_i << 1) - 2);
		 
   strobe #(17) phase_strobe(.clk_i(clk_i),.rst_i(rst_i),.ena_i(ena_i),
		       .rate_i(len),.strobe_i(strobe_in),.strobe_o(rx_strobe_o),
		       .count_o());

   wire pn_ref;
   wire ref_strobe = tx_strobe_i & ~rx_strobe_o; // Retard reference phase once per period
   lfsr ref_code
     ( .clk_i(clk_i),.rst_i(rst_i),.ena_i(ena_i),.strobe_i(ref_strobe),.mask_i(mask_i),.pn_o(pn_ref) );

   wire [5:0] offset = (5'd16-degree_i);
   
   reg  [31:0] sum_i, sum_q;
   reg  [31:0] total_i, total_q;
   wire [31:0] scaled_i = total_i << offset;
   wire [31:0] scaled_q = total_q << offset;
   wire [31:0] i_ext, q_ext;

   sign_extend #(16,32) i_extender(rx_in_i_i, i_ext);
   sign_extend #(16,32) q_extender(rx_in_q_i, q_ext);

   wire [31:0] prod_i = pn_ref ? i_ext : -i_ext;
   wire [31:0] prod_q = pn_ref ? q_ext : -q_ext;
   
   always @(posedge clk_i)
     if (rst_i | ~ena_i)
       begin
	  sum_i <= 0;
	  sum_q <= 0;
	  total_i <= 0;
	  total_q <= 0;
       end
     else if (rx_strobe_o)
       begin
	  total_i <= sum_i + prod_i;
	  total_q <= sum_q + prod_q;
	  sum_i <= 0;
	  sum_q <= 0;
       end
     else if (strobe_in)
       begin
	  sum_i = sum_i + prod_i;
	  sum_q = sum_q + prod_q;
       end
   
   assign rx_i_o = scaled_i[31:16];
   assign rx_q_o = scaled_q[31:16];
   
endmodule // sounder_rx

