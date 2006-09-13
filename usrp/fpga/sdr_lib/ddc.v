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



// DDC block

module ddc(input clock,
			input reset,
			input enable,
			input [3:0] rate1,
			input [3:0] rate2,
			output strobe,
			input [31:0] freq,
			input [15:0] i_in,
			input [15:0] q_in,
			output [15:0] i_out,
			output [15:0] q_out
			);
   parameter bw = 16;
   parameter zw = 16;

	wire [15:0] i_cordic_out, q_cordic_out;
	wire [31:0] phase;

	wire strobe1, strobe2;
	reg [3:0] strobe_ctr1,strobe_ctr2;

	always @(posedge clock)
		if(reset | ~enable)
			strobe_ctr2 <= #1 4'd0;
		else if(strobe2)
			strobe_ctr2 <= #1 4'd0;
		else	
			strobe_ctr2 <= #1 strobe_ctr2 + 4'd1;
				
	always @(posedge clock)
		if(reset | ~enable)
			strobe_ctr1 <= #1 4'd0;
		else if(strobe1)
			strobe_ctr1 <= #1 4'd0;
		else if(strobe2)
			strobe_ctr1 <= #1 strobe_ctr1 + 4'd1;
				

	assign strobe2 = enable & ( strobe_ctr2 == rate2 );
	assign strobe1 = strobe2 & ( strobe_ctr1 == rate1 );

	assign strobe = strobe1;

	function [2:0] log_ceil;
	input [3:0] val;
	
		log_ceil = val[3] ? 3'd4 : val[2] ? 3'd3 : val[1] ? 3'd2 : 3'd1; 
	endfunction	
	
	wire [2:0] shift1 = log_ceil(rate1);
	wire [2:0] shift2 = log_ceil(rate2);
	
	cordic #(.bitwidth(bw),.zwidth(zw),.stages(16))
		cordic(.clock(clock), .reset(reset), .enable(enable),
			.xi(i_in), .yi(q_in), .zi(phase[31:32-zw]), 
			.xo(i_cordic_out), .yo(q_cordic_out), .zo()  );
		
	cic_decim_2stage #(.bw(bw),.N(4)) 
		decim_i(.clock(clock),.reset(reset),.enable(enable),
			.strobe1(1'b1),.strobe2(strobe2),.strobe3(strobe1),.shift1(shift2),.shift2(shift1),
			.signal_in(i_cordic_out),.signal_out(i_out));
			
	cic_decim_2stage #(.bw(bw),.N(4)) 
		decim_q(.clock(clock),.reset(reset),.enable(enable),
			.strobe1(1'b1),.strobe2(strobe2),.strobe3(strobe1),.shift1(shift2),.shift2(shift1),
			.signal_in(q_cordic_out),.signal_out(q_out));
	
	phase_acc #(.resolution(32))
		nco (.clk(clock),.reset(reset),.enable(enable),
			.freq(freq),.phase(phase));
		
endmodule
