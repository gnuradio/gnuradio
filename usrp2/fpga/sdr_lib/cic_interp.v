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


module cic_interp
  #(parameter bw = 16, parameter N = 4, parameter log2_of_max_rate = 7)
    (input clock,
     input reset,
     input enable,
     input [7:0] rate,
     input strobe_in,
     input strobe_out,
     input [bw-1:0] signal_in,
     output reg [bw-1:0] signal_out);
   
   integer 	     i;
   localparam 	     maxbitgain = (N-1)*log2_of_max_rate;

   wire [bw+maxbitgain-1:0] signal_in_ext;
   reg [bw+maxbitgain-1:0]  integrator [0:N-1];
   reg [bw+maxbitgain-1:0]  differentiator [0:N-1];
   reg [bw+maxbitgain-1:0]  pipeline [0:N-1];
   
   sign_extend #(bw,bw+maxbitgain) 
     ext_input (.in(signal_in),.out(signal_in_ext));
   
   //FIXME Note that this section has pipe and diff reversed
   // It still works, but is confusing
   always @(posedge clock)
     if(reset | ~enable)
       for(i=0;i<N;i=i+1)
	 integrator[i] <= 0;
     else if (enable & strobe_out)
       begin
	  if(strobe_in)
	    integrator[0] <= integrator[0] + pipeline[N-1];
	  for(i=1;i<N;i=i+1)
	    integrator[i] <= integrator[i] + integrator[i-1];
       end
   
   always @(posedge clock)
     if(reset | ~enable)
       begin
	  for(i=0;i<N;i=i+1)
	    begin
	       differentiator[i] <= 0;
	       pipeline[i] <= 0;
 	    end
       end
     else if (enable && strobe_in)
       begin
	  differentiator[0] <= signal_in_ext;
	  pipeline[0] <= signal_in_ext - differentiator[0];
	  for(i=1;i<N;i=i+1)
	    begin
	       differentiator[i] <= pipeline[i-1];
	       pipeline[i] <= pipeline[i-1] - differentiator[i];
	    end
       end

   wire [bw-1:0] signal_out_unreg;
   cic_int_shifter #(bw)
     cic_int_shifter(rate,integrator[N-1],signal_out_unreg);
   
   always @(posedge clock)
     signal_out <= signal_out_unreg;
   
endmodule // cic_interp

