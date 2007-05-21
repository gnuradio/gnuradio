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

module lfsr_constants(degree_i,mask_o,len_o);
   input  wire [4:0]  degree_i;
   output reg  [15:0] mask_o;
   output wire [15:0] len_o;
   
   assign len_o = (1 << degree_i) - 1;
   
   always @*
     case (degree_i)
       5'd00: mask_o = 16'h0000;
       5'd01: mask_o = 16'h0001;
       5'd02: mask_o = 16'h0003;
       5'd03: mask_o = 16'h0005;
       5'd04: mask_o = 16'h0009;
       5'd05: mask_o = 16'h0012;
       5'd06: mask_o = 16'h0021;
       5'd07: mask_o = 16'h0041;
       5'd08: mask_o = 16'h008E;
       5'd09: mask_o = 16'h0108;
       5'd10: mask_o = 16'h0204;
       5'd11: mask_o = 16'h0402;
       5'd12: mask_o = 16'h0829;
       5'd13: mask_o = 16'h100D;
       5'd14: mask_o = 16'h2015;
       5'd15: mask_o = 16'h4001;
       5'd16: mask_o = 16'h8016;
       default: mask_o = 16'h0000;
     endcase // case(degree_i)

endmodule // lfsr_constants
