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

`include "../top/config.vh"

module dac_interface(clk_i,rst_i,ena_i,strobe_i,tx_i_i,tx_q_i,tx_data_o,tx_sync_o);
   input clk_i;
   input rst_i;
   input ena_i;
   input strobe_i;
   
   input [13:0] tx_i_i;
   input [13:0] tx_q_i;

   output [13:0] tx_data_o;
   output 	 tx_sync_o;

`ifdef TX_RATE_MAX
   wire clk128;
   reg clk64_d;
   reg [13:0] tx_data_o;
   
   // Create a 128 MHz clock
   dacpll pll128(.areset(rst_i),.inclk0(clk_i),.c0(clk128));

   // Register the clk64 clock in the clk128 domain
   always @(posedge clk128)
     clk64_d <= #1 clk_i;

   // Register the tx data in the clk128 domain
   always @(posedge clk128)
     tx_data_o <= #1 clk64_d ? tx_i_i : tx_q_i;

   assign tx_sync_o = clk64_d;
   

`else // !`ifdef TX_RATE_MAX
   assign tx_data_o = strobe_i ? tx_q_i : tx_i_i;
   assign tx_sync_o = strobe_i;
`endif // !`ifdef TX_RATE_MAX
   
endmodule // dac_interface
