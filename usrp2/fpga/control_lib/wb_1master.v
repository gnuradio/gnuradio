/////////////////////////////////////////////////////////////////////
////                                                             ////
////  WISHBONE Connection Bus Top Level		                 ////
////                                                             ////
////                                                             ////
////  Original Author: Johny Chi		                 ////
////                   chisuhua@yahoo.com.cn                     ////
////  Modified By Matt Ettus, matt@ettus.com                     ////
////                                                             ////
////                                                             ////
/////////////////////////////////////////////////////////////////////
////                                                              ////
//// Copyright (C) 2000, 2007 Authors and OPENCORES.ORG           ////
////                                                              ////
//// This source file may be used and distributed without         ////
//// restriction provided that this copyright statement is not    ////
//// removed from the file and that any derivative work contains  ////
//// the original copyright notice and the associated disclaimer. ////
////                                                              ////
//// This source file is free software; you can redistribute it   ////
//// and/or modify it under the terms of the GNU Lesser General   ////
//// Public License as published by the Free Software Foundation; ////
//// either version 2.1 of the License, or (at your option) any   ////
//// later version.                                               ////
////                                                              ////
//// This source is distributed in the hope that it will be       ////
//// useful, but WITHOUT ANY WARRANTY; without even the implied   ////
//// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR      ////
//// PURPOSE.  See the GNU Lesser General Public License for more ////
//// details.                                                     ////
////                                                              ////
//// You should have received a copy of the GNU Lesser General    ////
//// Public License along with this source; if not, download it   ////
//// from http://www.opencores.org/lgpl.shtml                     ////
////                                                              ////
//////////////////////////////////////////////////////////////////////
//
//   Up to 8 slaves share a Wishbone Bus connection to 1 master

  module wb_1master
    #(parameter	s0_addr_w = 4,			// slave 0 address decode width
      parameter	s0_addr = 4'h0,			// slave 0 address
      parameter	s1_addr_w = 4 ,			// slave 1 address decode width
      parameter	s1_addr = 4'h1,			// slave 1 address 
      parameter	s215_addr_w = 8 ,		// slave 2 to slave 7 address decode width
      parameter	s2_addr = 8'h92,		// slave 2 address
      parameter	s3_addr = 8'h93,		// slave 3 address
      parameter	s4_addr = 8'h94,		// slave 4 address
      parameter	s5_addr = 8'h95,		// slave 5 address
      parameter	s6_addr = 8'h96,		// slave 6 address
      parameter	s7_addr = 8'h97,		// slave 7 address
      parameter	s8_addr = 8'h98,		// slave 7 address
      parameter	s9_addr = 8'h99,		// slave 7 address
      parameter	s10_addr = 8'h9a,		// slave 7 address
      parameter	s11_addr = 8'h9b,		// slave 7 address
      parameter	s12_addr = 8'h9c,		// slave 7 address
      parameter	s13_addr = 8'h9d,		// slave 7 address
      parameter	s14_addr = 8'h9e,		// slave 7 address
      parameter	s15_addr = 8'h9f,		// slave 7 address
      
      parameter	dw = 32,		// Data bus Width
      parameter	aw = 32,		// Address bus Width
      parameter	sw = 4)                 // Number of Select Lines
      
      (input clk_i, 
       input rst_i,
       
       // Master  Interface
       input [dw-1:0]   m0_dat_i,
       output [dw-1:0] 	m0_dat_o,
       input [aw-1:0] 	m0_adr_i,
       input [sw-1:0] 	m0_sel_i,
       input 		m0_we_i,
       input 		m0_cyc_i,
       input 		m0_stb_i,
       output 		m0_ack_o,
       output 		m0_err_o,
       output 		m0_rty_o,
       
       // Slave Interfaces
       input [dw-1:0] 	s0_dat_i,
       output [dw-1:0] 	s0_dat_o,
       output [aw-1:0] 	s0_adr_o,
       output [sw-1:0] 	s0_sel_o,
       output 		s0_we_o,
       output 		s0_cyc_o,
       output 		s0_stb_o,
       input 		s0_ack_i,
       input 		s0_err_i,
       input 		s0_rty_i,
       
       input [dw-1:0] 	s1_dat_i,
       output [dw-1:0] 	s1_dat_o,
       output [aw-1:0] 	s1_adr_o,
       output [sw-1:0] 	s1_sel_o,
       output 		s1_we_o,
       output 		s1_cyc_o,
       output 		s1_stb_o,
       input 		s1_ack_i,
       input 		s1_err_i,
       input 		s1_rty_i,
       
       input [dw-1:0] 	s2_dat_i,
       output [dw-1:0] 	s2_dat_o,
       output [aw-1:0] 	s2_adr_o,
       output [sw-1:0] 	s2_sel_o,
       output 		s2_we_o,
       output 		s2_cyc_o,
       output 		s2_stb_o,
       input 		s2_ack_i,
       input 		s2_err_i,
       input 		s2_rty_i,
       
       input [dw-1:0] 	s3_dat_i,
       output [dw-1:0] 	s3_dat_o,
       output [aw-1:0] 	s3_adr_o,
       output [sw-1:0] 	s3_sel_o,
       output 		s3_we_o,
       output 		s3_cyc_o,
       output 		s3_stb_o,
       input 		s3_ack_i,
       input 		s3_err_i,
       input 		s3_rty_i,
       
       input [dw-1:0] 	s4_dat_i,
       output [dw-1:0] 	s4_dat_o,
       output [aw-1:0] 	s4_adr_o,
       output [sw-1:0] 	s4_sel_o,
       output 		s4_we_o,
       output 		s4_cyc_o,
       output 		s4_stb_o,
       input 		s4_ack_i,
       input 		s4_err_i,
       input 		s4_rty_i,
       
       input [dw-1:0] 	s5_dat_i,
       output [dw-1:0] 	s5_dat_o,
       output [aw-1:0] 	s5_adr_o,
       output [sw-1:0] 	s5_sel_o,
       output 		s5_we_o,
       output 		s5_cyc_o,
       output 		s5_stb_o,
       input 		s5_ack_i,
       input 		s5_err_i,
       input 		s5_rty_i,
       
       input [dw-1:0] 	s6_dat_i,
       output [dw-1:0] 	s6_dat_o,
       output [aw-1:0] 	s6_adr_o,
       output [sw-1:0] 	s6_sel_o,
       output 		s6_we_o,
       output 		s6_cyc_o,
       output 		s6_stb_o,
       input 		s6_ack_i,
       input 		s6_err_i,
       input 		s6_rty_i,
       
       input [dw-1:0] 	s7_dat_i,
       output [dw-1:0] 	s7_dat_o,
       output [aw-1:0] 	s7_adr_o,
       output [sw-1:0] 	s7_sel_o,
       output 		s7_we_o,
       output 		s7_cyc_o,
       output 		s7_stb_o,
       input 		s7_ack_i,
       input 		s7_err_i,
       input 		s7_rty_i,

       input [dw-1:0] 	s8_dat_i,
       output [dw-1:0] 	s8_dat_o,
       output [aw-1:0] 	s8_adr_o,
       output [sw-1:0] 	s8_sel_o,
       output 		s8_we_o,
       output 		s8_cyc_o,
       output 		s8_stb_o,
       input 		s8_ack_i,
       input 		s8_err_i,
       input 		s8_rty_i,
       
       input [dw-1:0] 	s9_dat_i,
       output [dw-1:0] 	s9_dat_o,
       output [aw-1:0] 	s9_adr_o,
       output [sw-1:0] 	s9_sel_o,
       output 		s9_we_o,
       output 		s9_cyc_o,
       output 		s9_stb_o,
       input 		s9_ack_i,
       input 		s9_err_i,
       input 		s9_rty_i,
       
       input [dw-1:0] 	s10_dat_i,
       output [dw-1:0] 	s10_dat_o,
       output [aw-1:0] 	s10_adr_o,
       output [sw-1:0] 	s10_sel_o,
       output 		s10_we_o,
       output 		s10_cyc_o,
       output 		s10_stb_o,
       input 		s10_ack_i,
       input 		s10_err_i,
       input 		s10_rty_i,
       
       input [dw-1:0] 	s11_dat_i,
       output [dw-1:0] 	s11_dat_o,
       output [aw-1:0] 	s11_adr_o,
       output [sw-1:0] 	s11_sel_o,
       output 		s11_we_o,
       output 		s11_cyc_o,
       output 		s11_stb_o,
       input 		s11_ack_i,
       input 		s11_err_i,
       input 		s11_rty_i,
       
       input [dw-1:0] 	s12_dat_i,
       output [dw-1:0] 	s12_dat_o,
       output [aw-1:0] 	s12_adr_o,
       output [sw-1:0] 	s12_sel_o,
       output 		s12_we_o,
       output 		s12_cyc_o,
       output 		s12_stb_o,
       input 		s12_ack_i,
       input 		s12_err_i,
       input 		s12_rty_i,
       
       input [dw-1:0] 	s13_dat_i,
       output [dw-1:0] 	s13_dat_o,
       output [aw-1:0] 	s13_adr_o,
       output [sw-1:0] 	s13_sel_o,
       output 		s13_we_o,
       output 		s13_cyc_o,
       output 		s13_stb_o,
       input 		s13_ack_i,
       input 		s13_err_i,
       input 		s13_rty_i,
       
       input [dw-1:0] 	s14_dat_i,
       output [dw-1:0] 	s14_dat_o,
       output [aw-1:0] 	s14_adr_o,
       output [sw-1:0] 	s14_sel_o,
       output 		s14_we_o,
       output 		s14_cyc_o,
       output 		s14_stb_o,
       input 		s14_ack_i,
       input 		s14_err_i,
       input 		s14_rty_i,
       
       input [dw-1:0] 	s15_dat_i,
       output [dw-1:0] 	s15_dat_o,
       output [aw-1:0] 	s15_adr_o,
       output [sw-1:0] 	s15_sel_o,
       output 		s15_we_o,
       output 		s15_cyc_o,
       output 		s15_stb_o,
       input 		s15_ack_i,
       input 		s15_err_i,
       input 		s15_rty_i
       );
   
   // ////////////////////////////////////////////////////////////////
   //
   // Local wires
   //
   
   wire [15:0] 		ssel_dec;
   reg [dw-1:0] 	i_dat_s;	// internal share bus , slave data to master
   
   // Master output Interface
   assign 		m0_dat_o = i_dat_s;
   
   always @*
     case(ssel_dec)
       1   : i_dat_s <= s0_dat_i;
       2   : i_dat_s <= s1_dat_i;
       4   : i_dat_s <= s2_dat_i;
       8   : i_dat_s <= s3_dat_i;
       16  : i_dat_s <= s4_dat_i;
       32  : i_dat_s <= s5_dat_i;
       64  : i_dat_s <= s6_dat_i;
       128 : i_dat_s <= s7_dat_i;
       256 : i_dat_s <= s8_dat_i;
       512 : i_dat_s <= s9_dat_i;
       1024 : i_dat_s <= s10_dat_i;
       2048 : i_dat_s <= s11_dat_i;
       4096 : i_dat_s <= s12_dat_i;
       8192 : i_dat_s <= s13_dat_i;
       16384 : i_dat_s <= s14_dat_i;
       32768 : i_dat_s <= s15_dat_i;
       default : i_dat_s <= s0_dat_i;
     endcase // case(ssel_dec)
   
   assign 		{m0_ack_o, m0_err_o, m0_rty_o} 
     =  {s0_ack_i | s1_ack_i | s2_ack_i | s3_ack_i | s4_ack_i | s5_ack_i | s6_ack_i | s7_ack_i |
	 s8_ack_i | s9_ack_i | s10_ack_i | s11_ack_i | s12_ack_i | s13_ack_i | s14_ack_i | s15_ack_i ,
	 s0_err_i | s1_err_i | s2_err_i | s3_err_i | s4_err_i | s5_err_i | s6_err_i | s7_err_i |
	 s8_err_i | s9_err_i | s10_err_i | s11_err_i | s12_err_i | s13_err_i | s14_err_i | s15_err_i ,
	 s0_rty_i | s1_rty_i | s2_rty_i | s3_rty_i | s4_rty_i | s5_rty_i | s6_rty_i | s7_rty_i |
	 s8_rty_i | s9_rty_i | s10_rty_i | s11_rty_i | s12_rty_i | s13_rty_i | s14_rty_i | s15_rty_i };

   // Slave output interfaces
   assign 		s0_adr_o = m0_adr_i;
   assign 		s0_sel_o = m0_sel_i;
   assign 		s0_dat_o = m0_dat_i;
   assign 		s0_we_o = m0_we_i;
   assign 		s0_cyc_o = m0_cyc_i;
   assign 		s0_stb_o = m0_cyc_i & m0_stb_i & ssel_dec[0];   
   
   assign 		s1_adr_o = m0_adr_i;
   assign 		s1_sel_o = m0_sel_i;
   assign 		s1_dat_o = m0_dat_i;
   assign 		s1_we_o = m0_we_i;
   assign 		s1_cyc_o = m0_cyc_i;
   assign 		s1_stb_o = m0_cyc_i & m0_stb_i & ssel_dec[1];   
   
   assign 		s2_adr_o = m0_adr_i;
   assign 		s2_sel_o = m0_sel_i;
   assign 		s2_dat_o = m0_dat_i;
   assign 		s2_we_o = m0_we_i;
   assign 		s2_cyc_o = m0_cyc_i;
   assign 		s2_stb_o = m0_cyc_i & m0_stb_i & ssel_dec[2];   
   
   assign 		s3_adr_o = m0_adr_i;
   assign 		s3_sel_o = m0_sel_i;
   assign 		s3_dat_o = m0_dat_i;
   assign 		s3_we_o = m0_we_i;
   assign 		s3_cyc_o = m0_cyc_i;
   assign 		s3_stb_o = m0_cyc_i & m0_stb_i & ssel_dec[3];   
   
   assign 		s4_adr_o = m0_adr_i;
   assign 		s4_sel_o = m0_sel_i;
   assign 		s4_dat_o = m0_dat_i;
   assign 		s4_we_o = m0_we_i;
   assign 		s4_cyc_o = m0_cyc_i;
   assign 		s4_stb_o = m0_cyc_i & m0_stb_i & ssel_dec[4];   
   
   assign 		s5_adr_o = m0_adr_i;
   assign 		s5_sel_o = m0_sel_i;
   assign 		s5_dat_o = m0_dat_i;
   assign 		s5_we_o = m0_we_i;
   assign 		s5_cyc_o = m0_cyc_i;
   assign 		s5_stb_o = m0_cyc_i & m0_stb_i & ssel_dec[5];   
   
   assign 		s6_adr_o = m0_adr_i;
   assign 		s6_sel_o = m0_sel_i;
   assign 		s6_dat_o = m0_dat_i;
   assign 		s6_we_o = m0_we_i;
   assign 		s6_cyc_o = m0_cyc_i;
   assign 		s6_stb_o = m0_cyc_i & m0_stb_i & ssel_dec[6];   
   
   assign 		s7_adr_o = m0_adr_i;
   assign 		s7_sel_o = m0_sel_i;
   assign 		s7_dat_o = m0_dat_i;
   assign 		s7_we_o = m0_we_i;
   assign 		s7_cyc_o = m0_cyc_i;
   assign 		s7_stb_o = m0_cyc_i & m0_stb_i & ssel_dec[7];   
   
   assign 		s8_adr_o = m0_adr_i;
   assign 		s8_sel_o = m0_sel_i;
   assign 		s8_dat_o = m0_dat_i;
   assign 		s8_we_o = m0_we_i;
   assign 		s8_cyc_o = m0_cyc_i;
   assign 		s8_stb_o = m0_cyc_i & m0_stb_i & ssel_dec[8];   
   
   assign 		s9_adr_o = m0_adr_i;
   assign 		s9_sel_o = m0_sel_i;
   assign 		s9_dat_o = m0_dat_i;
   assign 		s9_we_o = m0_we_i;
   assign 		s9_cyc_o = m0_cyc_i;
   assign 		s9_stb_o = m0_cyc_i & m0_stb_i & ssel_dec[9];   
   
   assign 		s10_adr_o = m0_adr_i;
   assign 		s10_sel_o = m0_sel_i;
   assign 		s10_dat_o = m0_dat_i;
   assign 		s10_we_o = m0_we_i;
   assign 		s10_cyc_o = m0_cyc_i;
   assign 		s10_stb_o = m0_cyc_i & m0_stb_i & ssel_dec[10];   
   
   assign 		s11_adr_o = m0_adr_i;
   assign 		s11_sel_o = m0_sel_i;
   assign 		s11_dat_o = m0_dat_i;
   assign 		s11_we_o = m0_we_i;
   assign 		s11_cyc_o = m0_cyc_i;
   assign 		s11_stb_o = m0_cyc_i & m0_stb_i & ssel_dec[11];   
   
   assign 		s12_adr_o = m0_adr_i;
   assign 		s12_sel_o = m0_sel_i;
   assign 		s12_dat_o = m0_dat_i;
   assign 		s12_we_o = m0_we_i;
   assign 		s12_cyc_o = m0_cyc_i;
   assign 		s12_stb_o = m0_cyc_i & m0_stb_i & ssel_dec[12];   
   
   assign 		s13_adr_o = m0_adr_i;
   assign 		s13_sel_o = m0_sel_i;
   assign 		s13_dat_o = m0_dat_i;
   assign 		s13_we_o = m0_we_i;
   assign 		s13_cyc_o = m0_cyc_i;
   assign 		s13_stb_o = m0_cyc_i & m0_stb_i & ssel_dec[13];   
   
   assign 		s14_adr_o = m0_adr_i;
   assign 		s14_sel_o = m0_sel_i;
   assign 		s14_dat_o = m0_dat_i;
   assign 		s14_we_o = m0_we_i;
   assign 		s14_cyc_o = m0_cyc_i;
   assign 		s14_stb_o = m0_cyc_i & m0_stb_i & ssel_dec[14];   
   
   assign 		s15_adr_o = m0_adr_i;
   assign 		s15_sel_o = m0_sel_i;
   assign 		s15_dat_o = m0_dat_i;
   assign 		s15_we_o = m0_we_i;
   assign 		s15_cyc_o = m0_cyc_i;
   assign 		s15_stb_o = m0_cyc_i & m0_stb_i & ssel_dec[15];   
   
   // Address decode logic
   // WARNING -- must make sure these are mutually exclusive!
   assign 		ssel_dec[0] = (m0_adr_i[aw -1 : aw - s0_addr_w ] == s0_addr);
   assign 		ssel_dec[1] = (m0_adr_i[aw -1 : aw - s1_addr_w ] == s1_addr);
   assign 		ssel_dec[2] = (m0_adr_i[aw -1 : aw - s215_addr_w ] == s2_addr);
   assign 		ssel_dec[3] = (m0_adr_i[aw -1 : aw - s215_addr_w ] == s3_addr);
   assign 		ssel_dec[4] = (m0_adr_i[aw -1 : aw - s215_addr_w ] == s4_addr);
   assign 		ssel_dec[5] = (m0_adr_i[aw -1 : aw - s215_addr_w ] == s5_addr);
   assign 		ssel_dec[6] = (m0_adr_i[aw -1 : aw - s215_addr_w ] == s6_addr);
   assign 		ssel_dec[7] = (m0_adr_i[aw -1 : aw - s215_addr_w ] == s7_addr);
   assign 		ssel_dec[8] = (m0_adr_i[aw -1 : aw - s215_addr_w ] == s8_addr);
   assign 		ssel_dec[9] = (m0_adr_i[aw -1 : aw - s215_addr_w ] == s9_addr);
   assign 		ssel_dec[10] = (m0_adr_i[aw -1 : aw - s215_addr_w ] == s10_addr);
   assign 		ssel_dec[11] = (m0_adr_i[aw -1 : aw - s215_addr_w ] == s11_addr);
   assign 		ssel_dec[12] = (m0_adr_i[aw -1 : aw - s215_addr_w ] == s12_addr);
   assign 		ssel_dec[13] = (m0_adr_i[aw -1 : aw - s215_addr_w ] == s13_addr);
   assign 		ssel_dec[14] = (m0_adr_i[aw -1 : aw - s215_addr_w ] == s14_addr);
   assign 		ssel_dec[15] = (m0_adr_i[aw -1 : aw - s215_addr_w ] == s15_addr);
   
endmodule // wb_1master
