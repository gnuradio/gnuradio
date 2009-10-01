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
    #(parameter	decode_w = 8,			// address decode width
      parameter	s0_addr = 8'h0,			// slave 0 address
      parameter	s0_mask = 8'h0,			// slave 0 don't cares
      parameter	s1_addr = 8'h0,			// slave 1 address
      parameter	s1_mask = 8'h0,			// slave 1 don't cares
      parameter	s2_addr = 8'h0,			// slave 2 address
      parameter	s2_mask = 8'h0,			// slave 2 don't cares
      parameter	s3_addr = 8'h0,			// slave 3 address
      parameter	s3_mask = 8'h0,			// slave 3 don't cares
      parameter	s4_addr = 8'h0,			// slave 4 address
      parameter	s4_mask = 8'h0,			// slave 4 don't cares
      parameter	s5_addr = 8'h0,			// slave 5 address
      parameter	s5_mask = 8'h0,			// slave 5 don't cares
      parameter	s6_addr = 8'h0,			// slave 6 address
      parameter	s6_mask = 8'h0,			// slave 6 don't cares
      parameter	s7_addr = 8'h0,			// slave 7 address
      parameter	s7_mask = 8'h0,			// slave 7 don't cares
      parameter	s8_addr = 8'h0,			// slave 8 address
      parameter	s8_mask = 8'h0,			// slave 8 don't cares
      parameter	s9_addr = 8'h0,			// slave 9 address
      parameter	s9_mask = 8'h0,			// slave 9 don't cares
      parameter	sa_addr = 8'h0,			// slave a address
      parameter	sa_mask = 8'h0,			// slave a don't cares
      parameter	sb_addr = 8'h0,			// slave b address
      parameter	sb_mask = 8'h0,			// slave b don't cares
      parameter	sc_addr = 8'h0,			// slave c address
      parameter	sc_mask = 8'h0,			// slave c don't cares
      parameter	sd_addr = 8'h0,			// slave d address
      parameter	sd_mask = 8'h0,			// slave d don't cares
      parameter	se_addr = 8'h0,			// slave e address
      parameter	se_mask = 8'h0,			// slave e don't cares
      parameter	sf_addr = 8'h0,			// slave f address
      parameter	sf_mask = 8'h0,			// slave f don't cares
            
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
       
       input [dw-1:0] 	sa_dat_i,
       output [dw-1:0] 	sa_dat_o,
       output [aw-1:0] 	sa_adr_o,
       output [sw-1:0] 	sa_sel_o,
       output 		sa_we_o,
       output 		sa_cyc_o,
       output 		sa_stb_o,
       input 		sa_ack_i,
       input 		sa_err_i,
       input 		sa_rty_i,
       
       input [dw-1:0] 	sb_dat_i,
       output [dw-1:0] 	sb_dat_o,
       output [aw-1:0] 	sb_adr_o,
       output [sw-1:0] 	sb_sel_o,
       output 		sb_we_o,
       output 		sb_cyc_o,
       output 		sb_stb_o,
       input 		sb_ack_i,
       input 		sb_err_i,
       input 		sb_rty_i,
       
       input [dw-1:0] 	sc_dat_i,
       output [dw-1:0] 	sc_dat_o,
       output [aw-1:0] 	sc_adr_o,
       output [sw-1:0] 	sc_sel_o,
       output 		sc_we_o,
       output 		sc_cyc_o,
       output 		sc_stb_o,
       input 		sc_ack_i,
       input 		sc_err_i,
       input 		sc_rty_i,
       
       input [dw-1:0] 	sd_dat_i,
       output [dw-1:0] 	sd_dat_o,
       output [aw-1:0] 	sd_adr_o,
       output [sw-1:0] 	sd_sel_o,
       output 		sd_we_o,
       output 		sd_cyc_o,
       output 		sd_stb_o,
       input 		sd_ack_i,
       input 		sd_err_i,
       input 		sd_rty_i,
       
       input [dw-1:0] 	se_dat_i,
       output [dw-1:0] 	se_dat_o,
       output [aw-1:0] 	se_adr_o,
       output [sw-1:0] 	se_sel_o,
       output 		se_we_o,
       output 		se_cyc_o,
       output 		se_stb_o,
       input 		se_ack_i,
       input 		se_err_i,
       input 		se_rty_i,
       
       input [dw-1:0] 	sf_dat_i,
       output [dw-1:0] 	sf_dat_o,
       output [aw-1:0] 	sf_adr_o,
       output [sw-1:0] 	sf_sel_o,
       output 		sf_we_o,
       output 		sf_cyc_o,
       output 		sf_stb_o,
       input 		sf_ack_i,
       input 		sf_err_i,
       input 		sf_rty_i
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
       1024 : i_dat_s <= sa_dat_i;
       2048 : i_dat_s <= sb_dat_i;
       4096 : i_dat_s <= sc_dat_i;
       8192 : i_dat_s <= sd_dat_i;
       16384 : i_dat_s <= se_dat_i;
       32768 : i_dat_s <= sf_dat_i;
       default : i_dat_s <= s0_dat_i;
     endcase // case(ssel_dec)
   
   assign 		{m0_ack_o, m0_err_o, m0_rty_o} 
     =  {s0_ack_i | s1_ack_i | s2_ack_i | s3_ack_i | s4_ack_i | s5_ack_i | s6_ack_i | s7_ack_i |
	 s8_ack_i | s9_ack_i | sa_ack_i | sb_ack_i | sc_ack_i | sd_ack_i | se_ack_i | sf_ack_i ,
	 s0_err_i | s1_err_i | s2_err_i | s3_err_i | s4_err_i | s5_err_i | s6_err_i | s7_err_i |
	 s8_err_i | s9_err_i | sa_err_i | sb_err_i | sc_err_i | sd_err_i | se_err_i | sf_err_i ,
	 s0_rty_i | s1_rty_i | s2_rty_i | s3_rty_i | s4_rty_i | s5_rty_i | s6_rty_i | s7_rty_i |
	 s8_rty_i | s9_rty_i | sa_rty_i | sb_rty_i | sc_rty_i | sd_rty_i | se_rty_i | sf_rty_i };

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
   
   assign 		sa_adr_o = m0_adr_i;
   assign 		sa_sel_o = m0_sel_i;
   assign 		sa_dat_o = m0_dat_i;
   assign 		sa_we_o = m0_we_i;
   assign 		sa_cyc_o = m0_cyc_i;
   assign 		sa_stb_o = m0_cyc_i & m0_stb_i & ssel_dec[10];   
   
   assign 		sb_adr_o = m0_adr_i;
   assign 		sb_sel_o = m0_sel_i;
   assign 		sb_dat_o = m0_dat_i;
   assign 		sb_we_o = m0_we_i;
   assign 		sb_cyc_o = m0_cyc_i;
   assign 		sb_stb_o = m0_cyc_i & m0_stb_i & ssel_dec[11];   
   
   assign 		sc_adr_o = m0_adr_i;
   assign 		sc_sel_o = m0_sel_i;
   assign 		sc_dat_o = m0_dat_i;
   assign 		sc_we_o = m0_we_i;
   assign 		sc_cyc_o = m0_cyc_i;
   assign 		sc_stb_o = m0_cyc_i & m0_stb_i & ssel_dec[12];   
   
   assign 		sd_adr_o = m0_adr_i;
   assign 		sd_sel_o = m0_sel_i;
   assign 		sd_dat_o = m0_dat_i;
   assign 		sd_we_o = m0_we_i;
   assign 		sd_cyc_o = m0_cyc_i;
   assign 		sd_stb_o = m0_cyc_i & m0_stb_i & ssel_dec[13];   
   
   assign 		se_adr_o = m0_adr_i;
   assign 		se_sel_o = m0_sel_i;
   assign 		se_dat_o = m0_dat_i;
   assign 		se_we_o = m0_we_i;
   assign 		se_cyc_o = m0_cyc_i;
   assign 		se_stb_o = m0_cyc_i & m0_stb_i & ssel_dec[14];   
   
   assign 		sf_adr_o = m0_adr_i;
   assign 		sf_sel_o = m0_sel_i;
   assign 		sf_dat_o = m0_dat_i;
   assign 		sf_we_o = m0_we_i;
   assign 		sf_cyc_o = m0_cyc_i;
   assign 		sf_stb_o = m0_cyc_i & m0_stb_i & ssel_dec[15];   
   
   // Address decode logic
   // WARNING -- must make sure these are mutually exclusive!


   assign 		ssel_dec[0] = ~|((m0_adr_i[aw-1:aw-decode_w] ^ s0_addr) & s0_mask);
   assign 		ssel_dec[1] = ~|((m0_adr_i[aw-1:aw-decode_w] ^ s1_addr) & s1_mask);
   assign 		ssel_dec[2] = ~|((m0_adr_i[aw-1:aw-decode_w] ^ s2_addr) & s2_mask);
   assign 		ssel_dec[3] = ~|((m0_adr_i[aw-1:aw-decode_w] ^ s3_addr) & s3_mask);
   assign 		ssel_dec[4] = ~|((m0_adr_i[aw-1:aw-decode_w] ^ s4_addr) & s4_mask);
   assign 		ssel_dec[5] = ~|((m0_adr_i[aw-1:aw-decode_w] ^ s5_addr) & s5_mask);
   assign 		ssel_dec[6] = ~|((m0_adr_i[aw-1:aw-decode_w] ^ s6_addr) & s6_mask);
   assign 		ssel_dec[7] = ~|((m0_adr_i[aw-1:aw-decode_w] ^ s7_addr) & s7_mask);
   assign 		ssel_dec[8] = ~|((m0_adr_i[aw-1:aw-decode_w] ^ s8_addr) & s8_mask);
   assign 		ssel_dec[9] = ~|((m0_adr_i[aw-1:aw-decode_w] ^ s9_addr) & s9_mask);
   assign 		ssel_dec[10] = ~|((m0_adr_i[aw-1:aw-decode_w] ^ sa_addr) & sa_mask);
   assign 		ssel_dec[11] = ~|((m0_adr_i[aw-1:aw-decode_w] ^ sb_addr) & sb_mask);
   assign 		ssel_dec[12] = ~|((m0_adr_i[aw-1:aw-decode_w] ^ sc_addr) & sc_mask);
   assign 		ssel_dec[13] = ~|((m0_adr_i[aw-1:aw-decode_w] ^ sd_addr) & sd_mask);
   assign 		ssel_dec[14] = ~|((m0_adr_i[aw-1:aw-decode_w] ^ se_addr) & se_mask);
   assign 		ssel_dec[15] = ~|((m0_adr_i[aw-1:aw-decode_w] ^ sf_addr) & sf_mask);

/*
   assign 		ssel_dec[0] = (m0_adr_i[aw -1 : aw - decode_w ] == s0_addr);
   assign 		ssel_dec[1] = (m0_adr_i[aw -1 : aw - decode_w ] == s1_addr);
   assign 		ssel_dec[2] = (m0_adr_i[aw -1 : aw - decode_w ] == s2_addr);
   assign 		ssel_dec[3] = (m0_adr_i[aw -1 : aw - decode_w ] == s3_addr);
   assign 		ssel_dec[4] = (m0_adr_i[aw -1 : aw - decode_w ] == s4_addr);
   assign 		ssel_dec[5] = (m0_adr_i[aw -1 : aw - decode_w ] == s5_addr);
   assign 		ssel_dec[6] = (m0_adr_i[aw -1 : aw - decode_w ] == s6_addr);
   assign 		ssel_dec[7] = (m0_adr_i[aw -1 : aw - decode_w ] == s7_addr);
   assign 		ssel_dec[8] = (m0_adr_i[aw -1 : aw - decode_w ] == s8_addr);
   assign 		ssel_dec[9] = (m0_adr_i[aw -1 : aw - decode_w ] == s9_addr);
   assign 		ssel_dec[10] = (m0_adr_i[aw -1 : aw - decode_w ] == sa_addr);
   assign 		ssel_dec[11] = (m0_adr_i[aw -1 : aw - decode_w ] == sb_addr);
   assign 		ssel_dec[12] = (m0_adr_i[aw -1 : aw - decode_w ] == sc_addr);
   assign 		ssel_dec[13] = (m0_adr_i[aw -1 : aw - decode_w ] == sd_addr);
   assign 		ssel_dec[14] = (m0_adr_i[aw -1 : aw - decode_w ] == se_addr);
   assign 		ssel_dec[15] = (m0_adr_i[aw -1 : aw - decode_w ] == sf_addr);
 */ 
endmodule // wb_1master
