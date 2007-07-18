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

module cordic_nco(clk_i,rst_i,ena_i,strobe_i,ampl_i,freq_i,phs_i,data_i_o,data_q_o);
   input clk_i;
   input rst_i;
   input ena_i;
   input strobe_i;

   input [15:0] ampl_i;
   input [31:0] freq_i;
   input [31:0] phs_i;

   output [15:0] data_i_o;
   output [15:0] data_q_o;

   reg [31:0] phase_reg;
   wire [31:0] phase = phase_reg + phs_i;
   wire [15:0] ampl;
   
   always @(posedge clk_i)
     begin
	if (rst_i | ~ena_i)
	  phase_reg <= 32'b0;
	else if (strobe_i)
	  phase_reg <= phase_reg + freq_i;
     end

   assign ampl = ena_i ? ampl_i : 16'b0;

   cordic tx_cordic
     (.clock(clk_i),.reset(rst_i),.enable(strobe_i), 
       .xi(ampl),.yi(16'b0),.zi(phase[31:16]),
       .xo(data_i_o),.yo(data_q_o),.zo());
	  
endmodule // cordic_nco
