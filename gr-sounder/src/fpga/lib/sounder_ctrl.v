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

`include "../../../../usrp/firmware/include/fpga_regs_common.v"
`include "../../../../usrp/firmware/include/fpga_regs_standard.v"

module sounder_ctrl(clk_i,rst_i,saddr_i,sdata_i,s_strobe_i,
		    reset_o,transmit_o,receive_o,loopback_o,
		    degree_o,ampl_o,mask_o,
		    tx_strobe_o,rx_strobe_o,sum_strobe_o,ref_strobe_o);

   input         clk_i;         // Master clock @ 64 MHz
   input         rst_i;         // Master synchronous reset
   input  [6:0]  saddr_i;	// Configuration bus address
   input  [31:0] sdata_i;	// Configuration bus data
   input 	 s_strobe_i;    // Configuration bus write
   output 	 reset_o;
   output 	 transmit_o;
   output 	 receive_o;
   output 	 loopback_o;
   output [4:0]  degree_o;
   output [13:0] ampl_o;
   output [15:0] mask_o;
   output 	 tx_strobe_o;
   output        rx_strobe_o;
   output        sum_strobe_o;
   output        ref_strobe_o;
   
   setting_reg #(`FR_USER_0) sr_mode
     ( .clock(clk_i),.reset(1'b0),.strobe(s_strobe_i),.addr(saddr_i),.in(sdata_i),
       .out({loopback_o,receive_o,transmit_o,reset_o}) );

   setting_reg #(`FR_USER_1) sr_lfsr_degree
     ( .clock(clk_i),.reset(1'b0),.strobe(s_strobe_i),.addr(saddr_i),.in(sdata_i),
       .out(degree_o) );
   
   setting_reg #(`FR_USER_2) sr_lfsr_ampl
     ( .clock(clk_i),.reset(1'b0),.strobe(s_strobe_i),.addr(saddr_i),.in(sdata_i),
       .out(ampl_o) );
   
   wire [16:0] len;
   lfsr_constants constants
     (.clk_i(clk_i),.rst_i(rst_i),.degree_i(degree_o),.mask_o(mask_o),
      .len_o(len) );

   reg [15:0] phase;
   assign tx_strobe_o = ~phase[0];
   assign ref_strobe_o = tx_strobe_o & !(phase>>1 == len>>1);
   assign sum_strobe_o = (phase == len);
   
   reg 	  rx_strobe_o;
   always @(posedge clk_i)
     if (rst_i)
       begin
	  phase <= #5 16'hFFFF;
	  rx_strobe_o <= #5 0;
       end
     else
       if (sum_strobe_o)
	 begin
	    phase <= #5 0;
	    rx_strobe_o <= #5 1'b1;
	 end
       else
	 begin
	    phase <= #5 phase + 16'b1;
	    rx_strobe_o <= #5 0;
	 end









   
endmodule // sounder_ctrl
