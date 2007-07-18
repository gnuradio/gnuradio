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

module radar_tx(clk_i,rst_i,ena_i,strobe_i,
		ampl_i,fstart_i,fincr_i,
		tx_i_o,tx_q_o);

   // System control
   input clk_i;
   input rst_i;
   input ena_i;
   input strobe_i;
   
   // Configuration
   input [15:0]  ampl_i;
   input [31:0]  fstart_i;
   input [31:0]  fincr_i;
   
   // Chirp output
   output [13:0] tx_i_o;
   output [13:0] tx_q_o;
   wire   [15:0] cordic_i, cordic_q;

   // Chirp generator
   reg [31:0] freq;

   always @(posedge clk_i)
     if (rst_i | ~ena_i)
       freq <= fstart_i;
     else
       if (strobe_i)
	 freq <= freq + fincr_i;
   
   cordic_nco nco(.clk_i(clk_i),.rst_i(rst_i),.ena_i(ena_i),.strobe_i(strobe_i),
		  .ampl_i(ampl_i),.freq_i(freq),.phs_i(0),
		  .data_i_o(cordic_i),.data_q_o(cordic_q));

   assign tx_i_o = cordic_i[13:0];
   assign tx_q_o = cordic_q[13:0];
	  
endmodule // radar_tx
