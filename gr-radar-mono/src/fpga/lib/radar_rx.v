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

module radar_rx(clk_i,rst_i,ena_i,dbg_i,pulse_num_i,rx_in_i_i,
		rx_in_q_i,rx_i_o,rx_q_o,rx_strobe_o);
   
   input clk_i;
   input rst_i;
   input ena_i;
   input dbg_i;
   
   input [15:0] rx_in_i_i;
   input [15:0] rx_in_q_i;
   input [15:0] pulse_num_i;
   
   output [15:0] rx_i_o;
   output [15:0] rx_q_o;
   output reg    rx_strobe_o;
   
   reg [15:0] count;

   always @(posedge clk_i)
     if (rst_i | ~ena_i)
       count <= 16'b0;
     else
       count <= count + 16'b1;

   wire [31:0] fifo_inp = dbg_i ? {count[15:0],pulse_num_i[15:0]} : {rx_in_i_i,rx_in_q_i};

   // Buffer incoming samples every clock
   wire [31:0] fifo_out;
   reg         fifo_ack;
   wire        fifo_empty;

// Use model if simulating, otherwise Altera Megacell
`ifdef SIMULATION
   fifo_1clk #(32, 2048) buffer(.clock(clk_i),.sclr(rst_i),
				.data(fifo_inp),.wrreq(ena_i),
				.rdreq(fifo_ack),.q(fifo_out),
				.empty(fifo_empty));
`else
   fifo32_2k buffer(.clock(clk_i),.sclr(rst_i),
		    .data(fifo_inp),.wrreq(ena_i),
		    .rdreq(fifo_ack),.q(fifo_out),
		    .empty(fifo_empty));
`endif
   
   // Write samples to rx_fifo every third clock
   `define ST_FIFO_IDLE   3'b001
   `define ST_FIFO_STROBE 3'b010
   `define ST_FIFO_ACK    3'b100

   reg [2:0]   state;

   always @(posedge clk_i)
     if (rst_i)
       begin
	  state <= `ST_FIFO_IDLE;
	  rx_strobe_o <= 1'b0;
	  fifo_ack <= 1'b0;
       end
     else
       case (state)
	 `ST_FIFO_IDLE:
	   if (!fifo_empty)
	     begin
		// Tell rx_fifo sample is ready
		rx_strobe_o <= 1'b1;
		state <= `ST_FIFO_STROBE;
	     end
	 `ST_FIFO_STROBE:
	   begin
	      rx_strobe_o <= 1'b0;
	      // Ack our FIFO
	      fifo_ack <= 1'b1;
	      state <= `ST_FIFO_ACK;
	   end
	 `ST_FIFO_ACK:
	   begin
	      fifo_ack <= 1'b0;
	      state <= `ST_FIFO_IDLE;
	   end
       endcase // case(state)

   assign rx_i_o = fifo_out[31:16];
   assign rx_q_o = fifo_out[15:0];
   
endmodule // radar_rx
