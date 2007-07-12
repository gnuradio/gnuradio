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

module cordic_stage( clock, reset, enable, xi,yi,zi,constant,xo,yo,zo);
   parameter bitwidth = 16;
   parameter zwidth = 16;
   parameter shift = 1;
   
   input     clock;
   input     reset;
   input     enable;
   input [bitwidth-1:0] xi,yi;
   input [zwidth-1:0] zi;
   input [zwidth-1:0] constant;
   output [bitwidth-1:0] xo,yo;
   output [zwidth-1:0] zo;
   
   wire z_is_pos = ~zi[zwidth-1];

   reg [bitwidth-1:0] 	 xo,yo;
   reg [zwidth-1:0] zo;
   
   always @(posedge clock)
     if(reset)
       begin
	  xo <= #1 0;
	  yo <= #1 0;
	  zo <= #1 0;
       end
     else if(enable)
       begin
	  xo <= #1 z_is_pos ?   
		xi - {{shift+1{yi[bitwidth-1]}},yi[bitwidth-2:shift]} :
		xi + {{shift+1{yi[bitwidth-1]}},yi[bitwidth-2:shift]};
	  yo <= #1 z_is_pos ?   
		yi + {{shift+1{xi[bitwidth-1]}},xi[bitwidth-2:shift]} :
		yi - {{shift+1{xi[bitwidth-1]}},xi[bitwidth-2:shift]};
	  zo <= #1 z_is_pos ?   
		zi - constant :
		zi + constant;
       end
endmodule
