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

`include "../../../../usrp/firmware/include/fpga_regs_common.v"
`include "../../../../usrp/firmware/include/fpga_regs_standard.v"

module radar_rx(clk_i,rst_i,ena_i,strobe_i,saddr_i,sdata_i,s_strobe_i,
		rx_in_i_i,rx_in_q_i,rx_i_o,rx_q_o,rx_strobe_o);
   
   input clk_i;
   input rst_i;
   input ena_i;
   input strobe_i;

   input [6:0] saddr_i;
   input [31:0] sdata_i;
   input s_strobe_i;

   input [15:0] rx_in_i_i;
   input [15:0] rx_in_q_i;
   
   output [15:0] rx_i_o;
   output [15:0] rx_q_o;
   output rx_strobe_o;

   // Just count up for debugging
   reg [31:0] counter;

   always @(posedge clk_i)
     begin
	if (rst_i | ~ena_i)
	  counter <= 32'b0;
	else if (strobe_i & rx_strobe_o)
	  counter <= counter + 32'b1;
     end

   assign rx_i_o = ena_i ? counter[31:16] : 16'b0;
   assign rx_q_o = ena_i ? counter[15:0] : 16'b0;
   
   // Temporarily we duplicate what master_control.v did to generate decim_strobe
   // so we can do receive debugging. Later we'll drive rx_strobe_o in bursts to
   // load receiver data into the rx fifo.
   strobe_gen rx_strobe_gen
     ( .clock(clk_i),.reset(rst_i),.enable(ena_i),.rate(7),.strobe_in(strobe_i),.strobe(rx_strobe_o) );
   
endmodule // radar_rx
