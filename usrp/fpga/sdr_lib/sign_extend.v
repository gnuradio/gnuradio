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


// Sign extension "macro"
// bits_out should be greater than bits_in

module sign_extend (in,out);
	parameter bits_in=0;  // FIXME Quartus insists on a default
	parameter bits_out=0;
	
	input [bits_in-1:0] in;
	output [bits_out-1:0] out;
	
	assign out = {{(bits_out-bits_in){in[bits_in-1]}},in};
	
endmodule
