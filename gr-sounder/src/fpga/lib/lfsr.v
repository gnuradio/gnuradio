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

module lfsr(clk_i,rst_i,ena_i,strobe_i,mask_i,pn_o);
   parameter width = 16;

   input clk_i;
   input rst_i;
   input ena_i;
   input strobe_i;
   input [width-1:0] mask_i;
   
   output pn_o;

   reg  [width-1:0] shifter;

   wire parity = ^(shifter & mask_i);
   
   always @(posedge clk_i)
     if (rst_i | ~ena_i)
       shifter <= #5 1;
     else
       if (strobe_i)
	 shifter <= #5 {shifter[width-2:0],parity};

   assign pn_o = shifter[0];
   
endmodule // lfsr
