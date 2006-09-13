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



module cordic_tb();

   cordic cordic(clk, reset, enable, xi, yi, zi, xo, yo, zo );

   reg reset;
   reg clk;
   reg enable;
   reg [15:0] xi, yi, zi;
   
   initial reset = 1'b1;
   initial #1000 reset = 1'b0;

   initial clk = 1'b0;
   always #50 clk <= ~clk;
   
   initial enable = 1'b1;

   initial zi = 16'b0;

   always @(posedge clk)
     zi <= #1 zi + 16'd0;
   
   wire [15:0] xo,yo,zo;

   initial $dumpfile("cordic.vcd");
  initial $dumpvars(0,cordic_tb);
   initial
     begin
`include "sine.txt"
     end

	wire [15:0] xiu = {~xi[15],xi[14:0]};
	wire [15:0] yiu = {~yi[15],yi[14:0]};
	wire [15:0] xou = {~xo[15],xo[14:0]};
	wire [15:0] you = {~yo[15],yo[14:0]};
   initial $monitor("%d\t%d\t%d\t%d\t%d",$time,xiu,yiu,xou,you);

endmodule // cordic_tb
