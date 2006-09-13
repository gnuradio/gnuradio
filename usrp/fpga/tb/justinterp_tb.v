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


module cic_decim_tb;

cic_decim #(.bitwidth(16),.stages(4))
   decim(clock,reset,enable,strobe_in,strobe_out,signal_in,signal_out);
	
   reg clock;
   reg reset;
   reg enable;
   wire strobe;
   reg [15:0] signal_in;
   wire [15:0] signal_out;

   assign      strobe_in = 1'b1;
   reg 	       strobe_out;

   always @(posedge clock)
     while(1)
       begin
	  @(posedge clock);
	  @(posedge clock);
	  @(posedge clock);
	  @(posedge clock);
	  strobe_out <= 1'b1;
	  @(posedge clock);
	  @(posedge clock);
	  @(posedge clock);
	  @(posedge clock);
	  strobe_out <= 1'b0;
       end
   
   initial clock = 0;
   always #50 clock = ~clock;
   
   initial reset = 1;
   initial #1000 reset = 0;

   initial enable = 0;
   initial #2000 enable = 1;
   
   initial signal_in = 16'h1;
   initial #500000 signal_in = 16'h7fff;
   initial #1000000 signal_in = 16'h8000;
   initial #1500000 signal_in = 16'hffff;

   
   initial $dumpfile("decim.vcd");
   initial $dumpvars(0,cic_decim_tb);

   initial #10000000 $finish;
   
endmodule // cic_decim_tb
