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


module sizetest(input clock, 
				input reset, 
				input enable, 
				input [15:0]xi, 
				input [15:0] yi, 
				input [15:0] zi, 
				output [15:0] xo, 
				output [15:0] yo,
				output [15:0] zo
//				input [15:0] constant 
				);

wire [16:0] zo;

cordic_stage cordic_stage(clock, reset, enable, xi, yi, zi, 16'd16383, xo, yo, zo );

endmodule
