// -*- verilog -*-
//
//  USRP - Universal Software Radio Peripheral
//
//  Copyright (C) 2008 Corgan Enterprises LLC
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

// Following defines conditionally include RX path circuitry

`include "../top/config.vh"	// resolved relative to project root

module rx_chain_dig
  (input clock,
   input reset,
   input enable,
   input wire [15:0] i_in_ana,
   input wire [15:0] q_in_ana,
   input wire i_in_dig,
   input wire q_in_dig,
   output wire [15:0] i_out,
   output wire [15:0] q_out
   );

   //assign upper 15 bits of output to analog input,
   // discards lsb of analog input and replace with digital input bit (which comes from gpio)
   assign i_out = (enable)?{i_in_ana[15:1],i_in_dig}:i_in_ana;
   assign q_out = (enable)?{q_in_ana[15:1],q_in_dig}:q_in_ana;

endmodule // rx_chain_dig
