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

module strobe(clk_i,rst_i,ena_i,rate_i,strobe_i,strobe_o,count_o);
   parameter width = 16;

   input              clk_i;
   input 	      rst_i;
   input 	      ena_i;
   input  [width-1:0] rate_i;	// Desired period minus one
   input 	      strobe_i;
   output 	      strobe_o;
   output [width-1:0] count_o;

   
   reg [width-1:0] counter;

   always @(posedge clk_i)
     if(rst_i | ~ena_i)
       counter <= 32'hFFFFFFFF;	// First period is short by one
     else if(strobe_i)
       if(counter == rate_i)
	 counter <= 0;
       else 
	 counter <= counter + 1;

   assign strobe_o = (counter == rate_i) & strobe_i;
   assign count_o = counter;
   
endmodule // strobe
