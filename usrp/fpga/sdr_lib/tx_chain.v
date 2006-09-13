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

module tx_chain
  (input clock,
   input reset,
   input enable,
   input wire [7:0] interp_rate,
   input sample_strobe,
   input interpolator_strobe,
   input wire [31:0] freq,
   input wire [15:0] i_in,
   input wire [15:0] q_in,
   output wire [15:0] i_out,
   output wire [15:0] q_out
   );

   wire [15:0] bb_i, bb_q;

   cic_interp cic_interp_i
     ( .clock(clock),.reset(reset),.enable(enable),
       .rate(interp_rate),.strobe_in(interpolator_strobe),.strobe_out(sample_strobe),
       .signal_in(i_in),.signal_out(bb_i) );

   cic_interp cic_interp_q
     ( .clock(clock),.reset(reset),.enable(enable),
       .rate(interp_rate),.strobe_in(interpolator_strobe),.strobe_out(sample_strobe),
       .signal_in(q_in),.signal_out(bb_q) );
   
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
