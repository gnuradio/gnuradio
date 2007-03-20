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


// NOTE   This only works for N=4, max decim rate of 128
// NOTE   signal "rate" is ONE LESS THAN the actual rate

module cic_dec_shifter(rate,signal_in,signal_out);
   parameter bw = 16;
   parameter maxbitgain = 28;
   
   input [7:0] rate;
   input       wire [bw+maxbitgain-1:0] signal_in;
   output      reg [bw-1:0] signal_out;

   function [4:0] bitgain;
      input [7:0] rate;
      case(rate)
	// Exact Cases -- N*log2(rate)
	8'd4 : bitgain = 8;
	8'd8 : bitgain = 12;
	8'd16 : bitgain = 16;
	8'd32 : bitgain = 20;
	8'd64 : bitgain = 24;
	8'd128 : bitgain = 28;
	
	// Nearest without overflow -- ceil(N*log2(rate))
	8'd5 : bitgain = 10;
	8'd6 : bitgain = 11;
	8'd7 : bitgain = 12;
	8'd9 : bitgain = 13;
	8'd10,8'd11 : bitgain = 14;
	8'd12,8'd13 : bitgain = 15;
	8'd14,8'd15 : bitgain = 16;
	8'd17,8'd18,8'd19 : bitgain = 17;
	8'd20,8'd21,8'd22 : bitgain = 18;
	8'd23,8'd24,8'd25,8'd26 : bitgain = 19;
	8'd27,8'd28,8'd29,8'd30,8'd31 : bitgain = 20;
	8'd33,8'd34,8'd35,8'd36,8'd37,8'd38 : bitgain = 21;
	8'd39,8'd40,8'd41,8'd42,8'd43,8'd44,8'd45 : bitgain = 22;
	8'd46,8'd47,8'd48,8'd49,8'd50,8'd51,8'd52,8'd53 : bitgain = 23;
	8'd54,8'd55,8'd56,8'd57,8'd58,8'd59,8'd60,8'd61,8'd62,8'd63 : bitgain = 24;
	8'd65,8'd66,8'd67,8'd68,8'd69,8'd70,8'd71,8'd72,8'd73,8'd74,8'd75,8'd76 : bitgain = 25;
	8'd77,8'd78,8'd79,8'd80,8'd81,8'd82,8'd83,8'd84,8'd85,8'd86,8'd87,8'd88,8'd89,8'd90 : bitgain = 26;
	8'd91,8'd92,8'd93,8'd94,8'd95,8'd96,8'd97,8'd98,8'd99,8'd100,8'd101,8'd102,8'd103,8'd104,8'd105,8'd106,8'd107 : bitgain = 27;
	default : bitgain = 28;
      endcase // case(rate)
   endfunction // bitgain
   
   wire [4:0] 	  shift = bitgain(rate+1);
   
   // We should be able to do this, but can't ....
   // assign 	  signal_out = signal_in[shift+bw-1:shift];
   
   always @*
     case(shift)
       5'd8  : signal_out = signal_in[8+bw-1:8];
       5'd10 : signal_out = signal_in[10+bw-1:10];
       5'd11 : signal_out = signal_in[11+bw-1:11];
       5'd12 : signal_out = signal_in[12+bw-1:12];
       5'd13 : signal_out = signal_in[13+bw-1:13];
       5'd14 : signal_out = signal_in[14+bw-1:14];
       5'd15 : signal_out = signal_in[15+bw-1:15];
       5'd16 : signal_out = signal_in[16+bw-1:16];
       5'd17 : signal_out = signal_in[17+bw-1:17];
       5'd18 : signal_out = signal_in[18+bw-1:18];
       5'd19 : signal_out = signal_in[19+bw-1:19];
       5'd20 : signal_out = signal_in[20+bw-1:20];
       5'd21 : signal_out = signal_in[21+bw-1:21];
       5'd22 : signal_out = signal_in[22+bw-1:22];
       5'd23 : signal_out = signal_in[23+bw-1:23];
       5'd24 : signal_out = signal_in[24+bw-1:24];
       5'd25 : signal_out = signal_in[25+bw-1:25];
       5'd26 : signal_out = signal_in[26+bw-1:26];
       5'd27 : signal_out = signal_in[27+bw-1:27];
       5'd28 : signal_out = signal_in[28+bw-1:28];
       
       default : signal_out = signal_in[28+bw-1:28];
     endcase // case(shift)

endmodule // cic_dec_shifter

