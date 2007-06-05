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

module sounder_rx(clk_i,rst_i,ena_i,sum_strobe_i,ref_strobe_i,
		  mask_i,degree_i,rx_in_i_i,rx_in_q_i,rx_i_o,rx_q_o);
   
   input         clk_i;		// Master clock
   input         rst_i;         // Subsystem reset
   input         ena_i;		// Subsystem enable
   input         sum_strobe_i;  // Strobe on last sample per period
   input         ref_strobe_i;  // PN code reference retarded one sample per period
    
   input  [15:0] mask_i;	// PN code LFSR mask
   input  [4:0]  degree_i;	// PN code LFSR sequency degree
   
   input  [15:0] rx_in_i_i;	// I channel on receive
   input  [15:0] rx_in_q_i;	// Q channel on receive

   output [15:0] rx_i_o;	// I channel of impulse response
   output [15:0] rx_q_o;	// Q channel of impulse response

   reg  [31:0] sum_i, sum_q;
   reg  [31:0] total_i, total_q;
   wire [31:0] i_ext, q_ext;

   sign_extend #(16,32) i_extender(rx_in_i_i, i_ext);
   sign_extend #(16,32) q_extender(rx_in_q_i, q_ext);

   wire pn_ref;
   lfsr ref_code
     ( .clk_i(clk_i),.rst_i(rst_i),.ena_i(ena_i),.strobe_i(ref_strobe_i),.mask_i(mask_i),.pn_o(pn_ref) );

   wire [31:0] prod_i = pn_ref ? i_ext : -i_ext;
   wire [31:0] prod_q = pn_ref ? q_ext : -q_ext;

   always @(posedge clk_i)
     if (rst_i | ~ena_i)
       begin
	  sum_i <= #5 0;
	  sum_q <= #5 0;
	  total_i <= #5 0;
	  total_q <= #5 0;
       end
     else
       if (sum_strobe_i)
	 begin
	    total_i <= #5 sum_i;
	    total_q <= #5 sum_q;
	    sum_i <= #5 prod_i;
	    sum_q <= #5 prod_q;
	 end
       else
	 begin
	    sum_i <= #5 sum_i + prod_i;
	    sum_q <= #5 sum_q + prod_q;
	 end

   wire [5:0]  offset = (5'd16-degree_i);
   wire [31:0] scaled_i = total_i << offset;
   wire [31:0] scaled_q = total_q << offset;
   assign rx_i_o = scaled_i[31:16];
   assign rx_q_o = scaled_q[31:16];
   
endmodule // sounder_rx

