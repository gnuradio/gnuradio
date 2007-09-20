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

`include "../lib/radar_config.vh"

module radar(clk_i,saddr_i,sdata_i,s_strobe_i,
	     tx_side_o,tx_strobe_o,tx_dac_i_o,tx_dac_q_o,
	     rx_adc_i_i,rx_adc_q_i,
	     rx_strobe_o,rx_ech_i_o,rx_ech_q_o,io_tx_ena_o);
   
   // System interface
   input         clk_i;		// Master clock @ 64 MHz
   input  [6:0]  saddr_i;	// Configuration bus address
   input  [31:0] sdata_i;	// Configuration bus data
   input 	 s_strobe_i;    // Configuration bus write
   
   // Transmit subsystem
   output        tx_side_o;     // Transmitter slot
   output        tx_strobe_o;	// Generate an transmitter output sample
   output [13:0] tx_dac_i_o;	// I channel transmitter output to DAC
   output [13:0] tx_dac_q_o;    // Q channel transmitter output to DAC
   output        io_tx_ena_o;   // Transmit/Receive switching
      
   // Receive subsystem
   input  [15:0] rx_adc_i_i;	// I channel input from ADC
   input  [15:0] rx_adc_q_i;	// Q channel input from ADC
   output 	 rx_strobe_o;	// Indicates output samples ready for Rx FIFO
   output [15:0] rx_ech_i_o;	// I channel processed echos to Rx FIFO
   output [15:0] rx_ech_q_o;	// Q channel processed echos to Rx FIFO

   // Application control
   wire          reset;		// Master application reset
   wire          tx_side;	// Transmitter slot
   wire          debug_enabled; // Enable debugging mode; 	 
   wire 	 tx_enable;     // Transmitter enable
   wire 	 rx_enable;     // Receiver enable
   wire          tx_ctrl;       // Transmitter on control
   wire          rx_ctrl;       // Receiver on control
   wire [15:0] 	 pulse_num;     // Count of pulses since tx_enabled
 	 
   // Configuration
   wire [15:0] 	 ampl;		// Pulse amplitude
   wire [31:0] 	 fstart;	// Chirp start frequency
   wire [31:0] 	 fincr;         // Chirp per strobe frequency increment

   radar_control controller
     (.clk_i(clk_i),.saddr_i(saddr_i),.sdata_i(sdata_i),.s_strobe_i(s_strobe_i),
      .reset_o(reset),.tx_side_o(tx_side_o),.dbg_o(debug_enabled),
      .tx_strobe_o(tx_strobe_o),.tx_ctrl_o(tx_ctrl),.rx_ctrl_o(rx_ctrl),
      .ampl_o(ampl),.fstart_o(fstart),.fincr_o(fincr),.pulse_num_o(pulse_num),
      .io_tx_ena_o(io_tx_ena_o));

   radar_tx transmitter
     ( .clk_i(clk_i),.rst_i(reset),.ena_i(tx_ctrl),.strobe_i(tx_strobe_o),
       .ampl_i(ampl),.fstart_i(fstart),.fincr_i(fincr),
       .tx_i_o(tx_dac_i_o),.tx_q_o(tx_dac_q_o) );
   
   radar_rx receiver
     ( .clk_i(clk_i),.rst_i(reset),.ena_i(rx_ctrl),.dbg_i(debug_enabled),
       .pulse_num_i(pulse_num),.rx_in_i_i(rx_adc_i_i),.rx_in_q_i(rx_adc_q_i),
       .rx_strobe_o(rx_strobe_o),.rx_i_o(rx_ech_i_o),.rx_q_o(rx_ech_q_o) );
   
endmodule // radar
