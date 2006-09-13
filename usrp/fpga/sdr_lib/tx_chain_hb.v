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

module tx_chain_hb
  (input clock,
   input reset,
   input enable,
   input wire [7:0] interp_rate,
   input sample_strobe,
   input interpolator_strobe,
   input hb_strobe,
   input wire [31:0] freq,
   input wire [15:0] i_in,
   input wire [15:0] q_in,
   output wire [15:0] i_out,
   output wire [15:0] q_out,
output wire [15:0] debug, output [15:0] hb_i_out
   );
assign debug[15:13] = {sample_strobe,hb_strobe,interpolator_strobe};

   wire [15:0] bb_i, bb_q;
   wire [15:0] hb_i_out, hb_q_out;
   
   halfband_interp hb
     (.clock(clock),.reset(reset),.enable(enable),
      .strobe_in(interpolator_strobe),.strobe_out(hb_strobe),
      .signal_in_i(i_in),.signal_in_q(q_in),
      .signal_out_i(hb_i_out),.signal_out_q(hb_q_out),
	.debug(debug[12:0]));
      
   cic_interp cic_interp_i
     ( .clock(clock),.reset(reset),.enable(enable),
       .rate(interp_rate),.strobe_in(hb_strobe),.strobe_out(sample_strobe),
       .signal_in(hb_i_out),.signal_out(bb_i) );

   cic_interp cic_interp_q
     ( .clock(clock),.reset(reset),.enable(enable),
       .rate(interp_rate),.strobe_in(hb_strobe),.strobe_out(sample_strobe),
       .signal_in(hb_q_out),.signal_out(bb_q) );
   
`define NOCORDIC_TX
`ifdef NOCORDIC_TX
   assign      i_out = bb_i;
   assign      q_out = bb_q;
`else
   wire [31:0] phase;

   phase_acc phase_acc_tx
     (.clk(clock),.reset(reset),.enable(enable),
      .strobe(sample_strobe),.freq(freq),.phase(phase) );
   
   cordic tx_cordic_0
     ( .clock(clock),.reset(reset),.enable(sample_strobe), 
       .xi(bb_i),.yi(bb_q),.zi(phase[31:16]),
       .xo(i_out),.yo(q_out),.zo() );
`endif

endmodule // tx_chain
