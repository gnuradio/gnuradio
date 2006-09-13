// -*- verilog -*-
//
//  USRP - Universal Software Radio Peripheral
//
//  Copyright (C) 2005,2006 Matt Ettus
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

module shifter(input wire [33:0] in, output wire [15:0] out, input wire [7:0] shift);
   // Wish we could do  assign out = in[15+shift:shift];

   reg [15:0] quotient, remainder;
   wire [15:0] out_unclipped;
   reg [18:0]  msbs;
   wire        in_range;
   
   always @*
     case(shift)
       0 : quotient = in[15:0];
       1 : quotient = in[16:1];
       2 : quotient = in[17:2];
       3 : quotient = in[18:3];
       4 : quotient = in[19:4];
       5 : quotient = in[20:5];
       6 : quotient = in[21:6];
       7 : quotient = in[22:7];
       8 : quotient = in[23:8];
       9 : quotient = in[24:9];
       10 : quotient = in[25:10];
       11 : quotient = in[26:11];
       12 : quotient = in[27:12];
       13 : quotient = in[28:13];
       14 : quotient = in[29:14];
       15 : quotient = in[30:15];
       16 : quotient = in[31:16];
       17 : quotient = in[32:17];
       18 : quotient = in[33:18];
       default : quotient = in[15:0];
     endcase // case(shift)

   always @*
     case(shift)
       0 : remainder = 16'b0;
       1 : remainder = {in[0],15'b0};
       2 : remainder = {in[1:0],14'b0};
       3 : remainder = {in[2:0],13'b0};
       4 : remainder = {in[3:0],12'b0};
       5 : remainder = {in[4:0],11'b0};
       6 : remainder = {in[5:0],10'b0};
       7 : remainder = {in[6:0],9'b0};
       8 : remainder = {in[7:0],8'b0};
       9 : remainder = {in[8:0],7'b0};
       10 : remainder = {in[9:0],6'b0};
       11 : remainder = {in[10:0],5'b0};
       12 : remainder = {in[11:0],4'b0};
       13 : remainder = {in[12:0],3'b0};
       14 : remainder = {in[13:0],2'b0};
       15 : remainder = {in[14:0],1'b0};
       16 : remainder = in[15:0];
       17 : remainder = in[16:1];
       18 : remainder = in[17:2];
       default : remainder = 16'b0;
     endcase // case(shift)

   always @*
     case(shift)
       0 : msbs = in[33:15];
       1 : msbs = {in[33],in[33:16]};
       2 : msbs = {{2{in[33]}},in[33:17]};
       3 : msbs = {{3{in[33]}},in[33:18]};
       4 : msbs = {{4{in[33]}},in[33:19]};
       5 : msbs = {{5{in[33]}},in[33:20]};
       6 : msbs = {{6{in[33]}},in[33:21]};
       7 : msbs = {{7{in[33]}},in[33:22]};
       8 : msbs = {{8{in[33]}},in[33:23]};
       9 : msbs = {{9{in[33]}},in[33:24]};
       10 : msbs = {{10{in[33]}},in[33:25]};
       11 : msbs = {{11{in[33]}},in[33:26]};
       12 : msbs = {{12{in[33]}},in[33:27]};
       13 : msbs = {{13{in[33]}},in[33:28]};
       14 : msbs = {{14{in[33]}},in[33:29]};
       15 : msbs = {{15{in[33]}},in[33:30]};
       16 : msbs = {{16{in[33]}},in[33:31]};
       17 : msbs = {{17{in[33]}},in[33:32]};
       18 : msbs = {{18{in[33]}},in[33]};
       default : msbs = in[33:15];
     endcase // case(shift)

   assign     in_range = &msbs | ~(|msbs);
   assign     out_unclipped = quotient + (in[33] & |remainder);
   assign     out = in_range ? out_unclipped : {in[33],{15{~in[33]}}};
   
endmodule // shifter
