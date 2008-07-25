// -*- verilog -*-
//
//  USRP - Universal Software Radio Peripheral
//
//  Copyright (C) 2003 Matt Ettus
//  Copyright (C) 2008 Corgan Enterprises LLC
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

// Integrate and dump decimation filter
// 
// Functionally equivalent to single-stage CIC decimator, simpler code
// Results in single sample impulse response at decimated rate

module integrator
  ( clock,reset,enable,rate,strobe_in,strobe_out,signal_in,signal_out);
   parameter bw = 16;
   parameter maxbitgain = 8;
   
   input       clock;
   input       reset;
   input       enable;
   input [7:0] rate;
   input       strobe_in;
   input       strobe_out;	

   input  [bw-1:0] signal_in;
   wire   [bw-1:0] signal_out_unreg;
   output [bw-1:0] signal_out;
   reg    [bw-1:0] signal_out;

   wire [bw+maxbitgain-1:0] signal_in_ext;
   reg  [bw+maxbitgain-1:0] accum;
   reg  [bw+maxbitgain-1:0] dump;

   sign_extend #(bw,bw+maxbitgain) 
      ext_input (.in(signal_in),.out(signal_in_ext));
   
   // Integrate samples, dump on strobe out
   always @(posedge clock)
     if (reset | ~enable)
       begin
	 accum <= 0;
	 dump <= 0;
       end
     else if (enable && strobe_in)
        if (~strobe_out)
	   accum <= accum + signal_in_ext;
        else
	   begin
	     dump <= accum;
	     accum <= signal_in_ext;
           end
   
   // Normalize for integration bit gain
   integ_shifter #(bw)
	shifter(rate,dump,signal_out_unreg);

   always @(posedge clock)
     signal_out <= #1 signal_out_unreg;
   
endmodule // integrator
