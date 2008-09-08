/* -*- verilog -*-
 * 
 *  USRP - Universal Software Radio Peripheral
 * 
 *  Copyright (C) 2005 Matt Ettus
 * 
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 * 
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 * 
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Boston, MA  02110-1301  USA
 */

/*
 * This implements a 31-tap halfband filter that decimates by two.
 * The coefficients are symmetric, and with the exception of the middle tap,
 * every other coefficient is zero.  The middle section of taps looks like this:
 *
 *  ..., -1468, 0, 2950, 0, -6158, 0, 20585, 32768, 20585, 0, -6158, 0, 2950, 0, -1468, ...
 *                                             |
 *                           middle tap -------+
 *
 * See coeff_rom.v for the full set.  The taps are scaled relative to 32768,
 * thus the middle tap equals 1.0.  Not counting the middle tap, there are 8
 * non-zero taps on each side, and they are symmetric.  A naive implementation
 * requires a mulitply for each non-zero tap.  Because of symmetry, we can
 * replace 2 multiplies with 1 add and 1 multiply.  Thus, to compute each output
 * sample, we need to perform 8 multiplications.  Since the middle tap is 1.0,
 * we just add the corresponding delay line value.
 *
 * About timing: We implement this with a single multiplier, so it takes
 * 8 cycles to compute a single output.  However, since we're decimating by two 
 * we can accept a new input value every 4 cycles.  strobe_in is asserted when
 * there's a new input sample available.  Depending on the overall decimation
 * rate, strobe_in may be asserted less frequently than once every 4 clocks.
 * On the output side, we assert strobe_out when output contains a new sample.
 *
 * Implementation: Every time strobe_in is asserted we store the new data into
 * the delay line.  We split the delay line into two components, one for the
 * even samples, and one for the odd samples.  ram16_odd is the delay line for
 * the odd samples.  This ram is written on each odd assertion of strobe_in, and
 * is read on each clock when we're computing the dot product.  ram16_even is
 * similar, although because it holds the even samples we must be able to read
 * two samples from different addresses at the same time, while writing the incoming
 * even samples. Thus it's "triple-ported".
 */

module halfband_decim
  (input clock, input reset, input enable, input strobe_in, output wire strobe_out,
   input wire [15:0] data_in, output reg [15:0] data_out,output wire [15:0] debugctrl);

   reg [3:0] rd_addr1;
   reg [3:0] rd_addr2;
   reg [3:0] phase;
   reg [3:0] base_addr;

   wire      signed [15:0] mac_out,middle_data, sum, coeff;
   wire      signed [30:0] product;
   wire      signed [33:0] sum_even;
   wire      clear;
   reg 	     store_odd;
   
   always @(posedge clock)
     if(reset)
       store_odd <= #1 1'b0;
     else
       if(strobe_in)
	 store_odd <= #1 ~store_odd;

   wire      start = strobe_in & store_odd;
   always @(posedge clock)
     if(reset)
       base_addr <= #1 4'd0;
     else if(start)
       base_addr <= #1 base_addr + 4'd1;

   always @(posedge clock)
     if(reset)
       phase <= #1 4'd8;
     else if (start)
       phase <= #1 4'd0;
     else if(phase != 4'd8)
       phase <= #1 phase + 4'd1;

   reg 	     start_d1,start_d2,start_d3,start_d4,start_d5,start_d6,start_d7,start_d8,start_d9,start_dA,start_dB,start_dC,start_dD;
   always @(posedge clock)
     begin
	start_d1 <= #1 start;
	start_d2 <= #1 start_d1;
	start_d3 <= #1 start_d2;
	start_d4 <= #1 start_d3;
	start_d5 <= #1 start_d4;
	start_d6 <= #1 start_d5;
	start_d7 <= #1 start_d6;
	start_d8 <= #1 start_d7;
	start_d9 <= #1 start_d8;
	start_dA <= #1 start_d9;
	start_dB <= #1 start_dA;
	start_dC <= #1 start_dB;
	start_dD <= #1 start_dC;
     end // always @ (posedge clock)
   
   reg 	  mult_en, mult_en_pre;
   always @(posedge clock)
     begin
	mult_en_pre <= #1 phase!=8;
	mult_en <= #1 mult_en_pre;
     end
   
   assign clear = start_d4; // was dC
   wire   latch_result = start_d4; // was dC
   assign strobe_out = start_d5; // was dD
   wire   acc_en;
   
   always @*
     case(phase[2:0])
       3'd0 : begin rd_addr1 = base_addr + 4'd0; rd_addr2 = base_addr + 4'd15; end
       3'd1 : begin rd_addr1 = base_addr + 4'd1; rd_addr2 = base_addr + 4'd14; end
       3'd2 : begin rd_addr1 = base_addr + 4'd2; rd_addr2 = base_addr + 4'd13; end
       3'd3 : begin rd_addr1 = base_addr + 4'd3; rd_addr2 = base_addr + 4'd12; end
       3'd4 : begin rd_addr1 = base_addr + 4'd4; rd_addr2 = base_addr + 4'd11; end
       3'd5 : begin rd_addr1 = base_addr + 4'd5; rd_addr2 = base_addr + 4'd10; end
       3'd6 : begin rd_addr1 = base_addr + 4'd6; rd_addr2 = base_addr + 4'd9; end
       3'd7 : begin rd_addr1 = base_addr + 4'd7; rd_addr2 = base_addr + 4'd8; end
       default: begin rd_addr1 = base_addr + 4'd0; rd_addr2 = base_addr + 4'd15; end
     endcase // case(phase)
   
   coeff_rom coeff_rom (.clock(clock),.addr(phase[2:0]-3'd1),.data(coeff));
   
   ram16_2sum ram16_even (.clock(clock),.write(strobe_in & ~store_odd),
			  .wr_addr(base_addr),.wr_data(data_in),
			  .rd_addr1(rd_addr1),.rd_addr2(rd_addr2),
			  .sum(sum));

   ram16 ram16_odd (.clock(clock),.write(strobe_in & store_odd),  // Holds middle items
		    .wr_addr(base_addr),.wr_data(data_in),
		    //.rd_addr(base_addr+4'd7),.rd_data(middle_data));
		    .rd_addr(base_addr+4'd6),.rd_data(middle_data));

   mult mult(.clock(clock),.x(coeff),.y(sum),.product(product),.enable_in(mult_en),.enable_out(acc_en));

   acc acc(.clock(clock),.reset(reset),.enable_in(acc_en),.enable_out(),
	   .clear(clear),.addend(product),.sum(sum_even));

   wire signed [33:0] dout = sum_even + {{4{middle_data[15]}},middle_data,14'b0}; // We already divided product by 2!!!!

   always @(posedge clock)
     if(reset)
       data_out <= #1 16'd0;
     else if(latch_result)
       data_out <= #1 dout[30:15] + (dout[33]& |dout[14:0]);

   assign  debugctrl = { clock,reset,acc_en,mult_en,clear,latch_result,store_odd,strobe_in,strobe_out,phase};
   
endmodule // halfband_decim
