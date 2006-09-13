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


module clk_divider(input reset, input wire in_clk,output reg out_clk, input [7:0] ratio);
   reg [7:0] counter;
   
   // FIXME maybe should use PLL or switch to double edge version	
	
   always @(posedge in_clk or posedge reset)
     if(reset)
       counter <= #1 8'd0;
     else if(counter == 0)
       counter <= #1 ratio[7:1] + (ratio[0] & out_clk) - 8'b1;
     else
       counter <= #1 counter-8'd1;
   
   always @(posedge in_clk or posedge reset)
     if(reset)
       out_clk <= #1 1'b0;
     else if(counter == 0)
       out_clk <= #1 ~out_clk;
   
endmodule // clk_divider

