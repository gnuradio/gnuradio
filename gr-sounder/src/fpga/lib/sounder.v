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

module sounder(clk_i, saddr_i, sdata_i, s_strobe_i,
	       tx_strobe_o, tx_dac_i_o, tx_dac_q_o,
	       rx_adc_i_i,rx_adc_q_i,
	       rx_strobe_o, rx_imp_i_o,rx_imp_q_o);

   // System interface
   input         clk_i;		// Master clock @ 64 MHz
   input  [6:0]  saddr_i;	// Configuration bus address
   input  [31:0] sdata_i;	// Configuration bus data
   input 	 s_strobe_i;    // Configuration bus write
   
   // Transmit subsystem
   output        tx_strobe_o;	// Generate an transmitter output sample
   output [13:0] tx_dac_i_o;	// I channel transmitter output to DAC
   output [13:0] tx_dac_q_o;    // Q channel transmitter output to DAC

   // Receive subsystem
   output 	 rx_strobe_o;	// Indicates output samples ready for Rx FIFO
   input  [15:0] rx_adc_i_i;	// I channel input from ADC interface module
   input  [15:0] rx_adc_q_i;	// Q channel input from ADC interface module
   output [15:0] rx_imp_i_o;	// I channel impulse response to Rx FIFO
   output [15:0] rx_imp_q_o;	// Q channel impulse response to Rx FIFO
 	 
   // Internal variables
   wire          reset;
   wire 	 transmit;
   wire 	 receive;
   wire 	 loopback;

   wire [4:0] 	 degree;
   wire [13:0] 	 ampl;
   wire [15:0] 	 mask;

   wire          ref_strobe;
   wire 	 sum_strobe;
   sounder_ctrl  master(.clk_i(clk_i),.rst_i(reset),.saddr_i(saddr_i),
			.sdata_i(sdata_i),.s_strobe_i(s_strobe_i),
			.reset_o(reset),.transmit_o(transmit),.receive_o(receive),.loopback_o(loopback),
			.degree_o(degree),.ampl_o(ampl),.mask_o(mask),.tx_strobe_o(tx_strobe_o),
			.rx_strobe_o(rx_strobe_o),.sum_strobe_o(sum_strobe),.ref_strobe_o(ref_strobe));
      
   // Loopback implementation
   wire [13:0] tx_i, tx_q;      
   wire [15:0] tx_i_ext, tx_q_ext;
   wire [15:0] rx_i, rx_q; 

   sign_extend #(14,16) tx_i_extender(tx_i, tx_i_ext);
   sign_extend #(14,16) tx_q_extender(tx_q, tx_q_ext);

   assign tx_dac_i_o = loopback ? 14'b0 : tx_i;
   assign tx_dac_q_o = loopback ? 14'b0 : tx_q;
   assign rx_i       = loopback ? tx_i_ext : rx_adc_i_i;
   assign rx_q       = loopback ? tx_q_ext : rx_adc_q_i;
   
   sounder_tx transmitter
     ( .clk_i(clk_i),.rst_i(reset),.ena_i(transmit),
       .strobe_i(tx_strobe_o),.mask_i(mask),.ampl_i(ampl),
       .tx_i_o(tx_i),.tx_q_o(tx_q) );
   
   sounder_rx receiver
     ( .clk_i(clk_i),.rst_i(reset),.ena_i(receive),
       .sum_strobe_i(sum_strobe),.ref_strobe_i(ref_strobe),
       .mask_i(mask),.degree_i(degree),
       .rx_in_i_i(rx_i),.rx_in_q_i(rx_q),.rx_i_o(rx_imp_i_o),.rx_q_o(rx_imp_q_o));

endmodule // sounder
