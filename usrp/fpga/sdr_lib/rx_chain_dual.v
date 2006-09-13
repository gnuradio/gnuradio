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

module rx_chain_dual
  (input clock,
   input clock_2x,
   input reset,
   input enable,
   input wire [7:0] decim_rate,
   input sample_strobe,
   input decimator_strobe,
   input wire [31:0] freq0,
   input wire [15:0] i_in0,
   input wire [15:0] q_in0,
   output wire [15:0] i_out0,
   output wire [15:0] q_out0,
   input wire [31:0] freq1,
   input wire [15:0] i_in1,
   input wire [15:0] q_in1,
   output wire [15:0] i_out1,
   output wire [15:0] q_out1
   );

   wire [15:0] phase;
   wire [15:0] bb_i, bb_q;
   wire [15:0] i_in, q_in;
   
   wire [31:0] phase0;
   wire [31:0] phase1;
   reg [15:0] bb_i0, bb_q0;
   reg [15:0] bb_i1, bb_q1;

   // We want to time-share the CORDIC by double-clocking it
   
   phase_acc rx_phase_acc_0
     (.clk(clock),.reset(reset),.enable(enable),
      .strobe(sample_strobe),.freq(freq0),.phase(phase0) );

   phase_acc rx_phase_acc_1
     (.clk(clock),.reset(reset),.enable(enable),
      .strobe(sample_strobe),.freq(freq1),.phase(phase1) );
   
   assign phase = clock ? phase0[31:16] : phase1[31:16];
   assign i_in = clock ? i_in0 : i_in1;
   assign q_in = clock ? q_in0 : q_in1;

// This appears reversed because of the number of CORDIC stages
   always @(posedge clock_2x)
     if(clock)
       begin
	  bb_i1 <= #1 bb_i;
	  bb_q1 <= #1 bb_q;
       end
     else
       begin
	  bb_i0 <= #1 bb_i;
	  bb_q0 <= #1 bb_q;
       end
	
   cordic rx_cordic
     ( .clock(clock_2x),.reset(reset),.enable(enable), 
       .xi(i_in),.yi(q_in),.zi(phase),
       .xo(bb_i),.yo(bb_q),.zo() );
   
   cic_decim cic_decim_i_0
     ( .clock(clock),.reset(reset),.enable(enable),
       .rate(decim_rate),.strobe_in(sample_strobe),.strobe_out(decimator_strobe),
       .signal_in(bb_i0),.signal_out(i_out0) );

   cic_decim cic_decim_q_0
     ( .clock(clock),.reset(reset),.enable(enable),
       .rate(decim_rate),.strobe_in(sample_strobe),.strobe_out(decimator_strobe),
       .signal_in(bb_q0),.signal_out(q_out0) );

   cic_decim cic_decim_i_1
     ( .clock(clock),.reset(reset),.enable(enable),
       .rate(decim_rate),.strobe_in(sample_strobe),.strobe_out(decimator_strobe),
       .signal_in(bb_i1),.signal_out(i_out1) );

   cic_decim cic_decim_q_1
     ( .clock(clock),.reset(reset),.enable(enable),
       .rate(decim_rate),.strobe_in(sample_strobe),.strobe_out(decimator_strobe),
       .signal_in(bb_q1),.signal_out(q_out1) );

endmodule // rx_chain
