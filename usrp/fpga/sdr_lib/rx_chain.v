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

// Following defines conditionally include RX path circuitry

`include "config.vh"	// resolved relative to project root

module rx_chain
  (input clock,
   input reset,
   input enable,
   input wire [7:0] decim_rate,
   input sample_strobe,
   input decimator_strobe,
   output wire hb_strobe,
   input [6:0] serial_addr, input [31:0] serial_data, input serial_strobe,
   input wire [15:0] i_in,
   input wire [15:0] q_in,
   output wire [15:0] i_out,
   output wire [15:0] q_out,
   output wire [15:0] debugdata,output wire [15:0] debugctrl
   );

   parameter FREQADDR = 0;
   parameter PHASEADDR = 0;
   
   wire [31:0] phase;
   wire [15:0] bb_i, bb_q;
   wire [15:0] hb_in_i, hb_in_q;
   
   assign      debugdata = hb_in_i;

`ifdef RX_NCO_ON
    phase_acc #(FREQADDR,PHASEADDR,32) rx_phase_acc
     (.clk(clock),.reset(reset),.enable(enable),
      .serial_addr(serial_addr),.serial_data(serial_data),.serial_strobe(serial_strobe),
      .strobe(sample_strobe),.phase(phase) );

   cordic rx_cordic
     ( .clock(clock),.reset(reset),.enable(enable), 
       .xi(i_in),.yi(q_in),.zi(phase[31:16]),
       .xo(bb_i),.yo(bb_q),.zo() );
`else
   assign bb_i = i_in;
   assign bb_q = q_in;
   assign sample_strobe = 1;
`endif // !`ifdef RX_NCO_ON
   
`ifdef RX_CIC_ON
   cic_decim cic_decim_i_0
     ( .clock(clock),.reset(reset),.enable(enable),
       .rate(decim_rate),.strobe_in(sample_strobe),.strobe_out(decimator_strobe),
       .signal_in(bb_i),.signal_out(hb_in_i) );
`else
   assign hb_in_i = bb_i;
   assign decimator_strobe = sample_strobe;
`endif
   
`ifdef RX_HB_ON
   halfband_decim hbd_i_0
     ( .clock(clock),.reset(reset),.enable(enable),
       .strobe_in(decimator_strobe),.strobe_out(hb_strobe),
       .data_in(hb_in_i),.data_out(i_out),.debugctrl(debugctrl) );
`else
   assign i_out = hb_in_i;
   assign hb_strobe = decimator_strobe;
`endif
   
`ifdef RX_CIC_ON
   cic_decim cic_decim_q_0
     ( .clock(clock),.reset(reset),.enable(enable),
       .rate(decim_rate),.strobe_in(sample_strobe),.strobe_out(decimator_strobe),
       .signal_in(bb_q),.signal_out(hb_in_q) );
`else
   assign hb_in_q = bb_q;
`endif

`ifdef RX_HB_ON
   halfband_decim hbd_q_0
     ( .clock(clock),.reset(reset),.enable(enable),
       .strobe_in(decimator_strobe),.strobe_out(),
       .data_in(hb_in_q),.data_out(q_out) );   
`else
   assign q_out = hb_in_q;
`endif


endmodule // rx_chain
