// -*- verilog -*-
//
//  USRP - Universal Software Radio Peripheral
//
//  Copyright (C) 2008 Corgan Enterprises LLC
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

module gpio_input
  (input clock, input reset, input enable,
   input out_strobe,
   input wire [6:0] serial_addr, input wire [31:0] serial_data, input serial_strobe,
   input wire [15:0] io_rx_a_in, input wire [15:0] io_rx_b_in,
   //input wire [15:0] io_tx_a_in, input wire [15:0] io_tx_b_in, 
   output reg rx_dig0_i, output reg rx_dig0_q, 
   output reg rx_dig1_i, output reg rx_dig1_q );
      
    // Buffer at input to chip

   reg rx_dig_rx_a_a,rx_dig_rx_b_a,rx_dig_rx_a_b,rx_dig_rx_b_b;
   //TODO possibly use a flancter here
   //This code can optionally be extended to do streaming input from gpio of tx boards
   //The code can also be extended to input more bits
 
   always @(posedge clock)
     begin
        //This is the first point where is determined which physical input gpio pins are used for streaming digital input
        //The other point is the code which overrides these pins as input  (oe = 0)
	//rx_dig_tx_a_a <= #1 io_tx_a_in[14];
	//rx_dig_tx_b_a <= #1 io_tx_a_in[15];
	rx_dig_rx_a_a <= #1 io_rx_a_in[14];
	rx_dig_rx_b_a <= #1 io_rx_a_in[15];
	//rx_dig_tx_a_b <= #1 io_tx_b_in[14];
	//rx_dig_tx_b_b <= #1 io_tx_b_in[15];
	rx_dig_rx_a_b <= #1 io_rx_b_in[14];
	rx_dig_rx_b_b <= #1 io_rx_b_in[15];
     end
   
   // Now mux to the appropriate outputs
   wire [3:0] 	ddc3mux,ddc2mux,ddc1mux,ddc0mux;
   wire 	rx_realsignals;
   wire [3:0]  rx_numchan;//not used here
   //TODO This setting reg readout is a duplicate of the one in adc_interface.v.
   //     Change code so this is done in only one place, or give this code its own register.
   setting_reg #(`FR_RX_MUX) sr_rxmux(.clock(clock),.reset(reset),.strobe(serial_strobe),.addr(serial_addr),
				      .in(serial_data),.out({ddc3mux,ddc2mux,ddc1mux,ddc0mux,rx_realsignals,rx_numchan[3:1]}));
   //assign 	rx_numchan[0] = 1'b0;
  
   always @(posedge clock)
     if (out_strobe) //out_strobe determines the time at which the digital inputs are sampled (with a delay of one sample)
     begin
	rx_dig0_i <= #1 ddc0mux[1] ? (ddc0mux[0] ? rx_dig_rx_b_b : rx_dig_rx_a_b) : (ddc0mux[0] ? rx_dig_rx_b_a : rx_dig_rx_a_a);
	rx_dig0_q <= #1 rx_realsignals ? 1'b0 : ddc0mux[3] ? (ddc0mux[2] ? rx_dig_rx_b_b : rx_dig_rx_a_b) : (ddc0mux[2] ? rx_dig_rx_b_a : rx_dig_rx_a_a);
	rx_dig1_i <= #1 ddc1mux[1] ? (ddc1mux[0] ? rx_dig_rx_b_b : rx_dig_rx_a_b) : (ddc1mux[0] ? rx_dig_rx_b_a : rx_dig_rx_a_a);
	rx_dig1_q <= #1 rx_realsignals ? 1'b0 : ddc1mux[3] ? (ddc1mux[2] ? rx_dig_rx_b_b : rx_dig_rx_a_b) : (ddc1mux[2] ? rx_dig_rx_b_a : rx_dig_rx_a_a);
	//rx_dig2_i <= #1 ddc2mux[1] ? (ddc2mux[0] ? rx_dig_rx_b_b : rx_dig_rx_a_b) : (ddc2mux[0] ? rx_dig_rx_b_a : rx_dig_rx_a_a);
	//rx_dig2_q <= #1 rx_realsignals ? 1'b0 : ddc2mux[3] ? (ddc2mux[2] ? rx_dig_rx_b_b : rx_dig_rx_a_b) : (ddc2mux[2] ? rx_dig_rx_b_a : rx_dig_rx_a_a);
	//rx_dig3_i <= #1 ddc3mux[1] ? (ddc3mux[0] ? rx_dig_rx_b_b : rx_dig_rx_a_b) : (ddc3mux[0] ? rx_dig_rx_b_a : rx_dig_rx_a_a);
	//rx_dig3_q <= #1 rx_realsignals ? 1'b0 : ddc3mux[3] ? (ddc3mux[2] ? rx_dig_rx_b_b : rx_dig_rx_a_b) : (ddc3mux[2] ? rx_dig_rx_b_a : rx_dig_rx_a_a);
     end

endmodule // gpio_input

   
