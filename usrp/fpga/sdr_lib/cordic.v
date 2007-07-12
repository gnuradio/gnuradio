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

module cordic(clock, reset, enable, xi, yi, zi, xo, yo, zo );
   parameter bitwidth = 16;
   parameter zwidth = 16;
   
   input clock;
   input reset;
   input enable;
   input [bitwidth-1:0] xi, yi;
   output [bitwidth-1:0] xo, yo;
   input [zwidth-1:0] zi;
   output [zwidth-1:0] zo;
   
   reg [bitwidth+1:0] 	 x0,y0;
   reg [zwidth-2:0] 	 z0;
   wire [bitwidth+1:0] 	 x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12;
   wire [bitwidth+1:0] 	 y1,y2,y3,y4,y5,y6,y7,y8,y9,y10,y11,y12;
   wire [zwidth-2:0] z1,z2,z3,z4,z5,z6,z7,z8,z9,z10,z11,z12;
   
   wire [bitwidth+1:0] xi_ext = {{2{xi[bitwidth-1]}},xi};
   wire [bitwidth+1:0] yi_ext = {{2{yi[bitwidth-1]}},yi};

   // Compute consts.  Would be easier if vlog had atan...
   // see gen_cordic_consts.py
   
`define c00 16'd8192
`define c01 16'd4836
`define c02 16'd2555
`define c03 16'd1297
`define c04 16'd651
`define c05 16'd326
`define c06 16'd163
`define c07 16'd81
`define c08 16'd41
`define c09 16'd20
`define c10 16'd10
`define c11 16'd5
`define c12 16'd3
`define c13 16'd1
`define c14 16'd1
`define c15 16'd0
`define c16 16'd0

   always @(posedge clock)
     if(reset)
       begin
	  x0   <= #1 0; y0   <= #1 0;  z0   <= #1 0;
       end
     else if(enable)
       begin
	  z0 <= #1 zi[zwidth-2:0];
	  case (zi[zwidth-1:zwidth-2])
	    2'b00, 2'b11 : 
	      begin
		 x0 <= #1 xi_ext;
		 y0 <= #1 yi_ext;
	      end
	    2'b01, 2'b10 :
	      begin
		 x0 <= #1 -xi_ext;
		 y0 <= #1 -yi_ext;
	      end
	  endcase // case(zi[zwidth-1:zwidth-2])
       end // else: !if(reset)
   
   // FIXME need to handle variable number of stages
   // FIXME should be able to narrow zwidth but quartus makes it bigger...
   // This would be easier if arrays worked better in vlog...
   cordic_stage #(bitwidth+2,zwidth-1,0) cordic_stage0 (clock,reset,enable,x0,y0,z0,`c00,x1,y1,z1);
   cordic_stage #(bitwidth+2,zwidth-1,1) cordic_stage1 (clock,reset,enable,x1,y1,z1,`c01,x2,y2,z2);
   cordic_stage #(bitwidth+2,zwidth-1,2) cordic_stage2 (clock,reset,enable,x2,y2,z2,`c02,x3,y3,z3);
   cordic_stage #(bitwidth+2,zwidth-1,3) cordic_stage3 (clock,reset,enable,x3,y3,z3,`c03,x4,y4,z4);
   cordic_stage #(bitwidth+2,zwidth-1,4) cordic_stage4 (clock,reset,enable,x4,y4,z4,`c04,x5,y5,z5);
   cordic_stage #(bitwidth+2,zwidth-1,5) cordic_stage5 (clock,reset,enable,x5,y5,z5,`c05,x6,y6,z6);
   cordic_stage #(bitwidth+2,zwidth-1,6) cordic_stage6 (clock,reset,enable,x6,y6,z6,`c06,x7,y7,z7);
   cordic_stage #(bitwidth+2,zwidth-1,7) cordic_stage7 (clock,reset,enable,x7,y7,z7,`c07,x8,y8,z8);
   cordic_stage #(bitwidth+2,zwidth-1,8) cordic_stage8 (clock,reset,enable,x8,y8,z8,`c08,x9,y9,z9);
   cordic_stage #(bitwidth+2,zwidth-1,9) cordic_stage9 (clock,reset,enable,x9,y9,z9,`c09,x10,y10,z10);
   cordic_stage #(bitwidth+2,zwidth-1,10) cordic_stage10 (clock,reset,enable,x10,y10,z10,`c10,x11,y11,z11);
   cordic_stage #(bitwidth+2,zwidth-1,11) cordic_stage11 (clock,reset,enable,x11,y11,z11,`c11,x12,y12,z12);

   assign xo = x12[bitwidth:1];  
   assign yo = y12[bitwidth:1];
   //assign xo = x12[bitwidth+1:2];  // CORDIC gain is ~1.6, plus gain from rotating vectors
   //assign yo = y12[bitwidth+1:2];
   assign zo = z12;		  

endmodule // cordic

