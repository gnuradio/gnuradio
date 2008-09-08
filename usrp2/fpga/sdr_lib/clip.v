// -*- verilog -*-
//
//  USRP - Universal Software Radio Peripheral
//
//  Copyright (C) 2008 Matt Ettus
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

// Clipping "macro", keeps the bottom bits

module clip
  #(parameter bits_in=0,
    parameter bits_out=0)
    (input [bits_in-1:0] in,
     output [bits_out-1:0] out);
   
   wire 		   overflow = |in[bits_in-1:bits_out-1] & ~(&in[bits_in-1:bits_out-1]);   
   assign 		   out = overflow ? 
			   (in[bits_in-1] ? {1'b1,{(bits_out-1){1'b0}}} : {1'b0,{(bits_out-1){1'b1}}}) :
			   in[bits_out-1:0];
   
endmodule // clip

