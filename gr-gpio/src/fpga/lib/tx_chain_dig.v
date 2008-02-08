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

module tx_chain_dig
  (input clock,
   input reset,
   input enable,
   input wire [15:0] i_in,
   input wire [15:0] q_in,
   output wire [15:0] i_out_ana,
   output wire [15:0] q_out_ana,
   output wire i_out_dig,
   output wire q_out_dig
   );

   //assign upper 15 bits to analog processing, discard lowest bit
   //output lower two bits of I and Q  as digital signal (to be output on gpio pins)
   assign i_out_ana = (enable)?{i_in[15:1],1'b0}:i_in;
   assign q_out_ana = (enable)?{q_in[15:1],1'b0}:q_in;
   //wire out_dig   = (enable)?{i_in[0],q_in[0]}:2'b00;
   assign i_out_dig = (enable)?i_in[0]:1'b0;
   assign q_out_dig = (enable)?q_in[0]:1'b0;

endmodule // tx_chain_dig
