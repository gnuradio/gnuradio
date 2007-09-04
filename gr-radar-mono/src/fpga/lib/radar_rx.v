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

module radar_rx(clk_i,rst_i,ena_i,dbg_i,
		rx_in_i_i,rx_in_q_i,
		rx_i_o,rx_q_o,rx_strobe_o);
   
   input clk_i;
   input rst_i;
   input ena_i;
   input dbg_i;
   
   input [15:0] rx_in_i_i;
   input [15:0] rx_in_q_i;
   
   output [15:0] rx_i_o;
   output [15:0] rx_q_o;
   output reg    rx_strobe_o;

   reg [15:0] count;

   always @(posedge clk_i)
     if (rst_i | ~ena_i)
       count <= 16'b0;
     else
       count <= count + 16'b1;

   wire [31:0] fifo_data = dbg_i ? {count[15:0],16'hAA55} : {rx_in_i_i,rx_in_q_i};

   // Need to buffer received samples as they come in at 32 bits per cycle
   // but the rx_buffer.v fifo is only 16 bits wide.
   //
   reg         fifo_read;
   wire [31:0] fifo_out;
   wire        fifo_empty;
   
   fifo32_4k fifo(.clock(clk_i),.sclr(rst_i),
		  .data(fifo_data),.wrreq(ena_i),
		  .q(fifo_out),.rdreq(fifo_read),
		  .empty(fifo_empty) );

   `define ST_RD_IDLE     4'b0001
   `define ST_RD_REQ      4'b0010
   `define ST_WR_FIFO     4'b0100
   `define ST_RD_DELAY    4'b1000

   reg [3:0] state;
   reg [3:0] delay;
   
   always @(posedge clk_i)
     if (rst_i | ~ena_i)
       begin
	  state <= `ST_RD_IDLE;
	  delay <= 4'd0;
	  rx_strobe_o <= 1'b0;
	  fifo_read <= 1'b0;
       end
     else
       case (state)
	 `ST_RD_IDLE:
	   begin
	      if (!fifo_empty)
		begin
		   fifo_read <= 1'b1;
		   state <= `ST_RD_REQ;
		end
	   end

	 `ST_RD_REQ:
	   begin
	      fifo_read <= 1'b0;
	      rx_strobe_o <= 1'b1;
	      state <= `ST_WR_FIFO;
	   end

	 `ST_WR_FIFO:
	   begin
	      rx_strobe_o <= 1'b0;
	      state <= `ST_RD_DELAY;
	   end

	 `ST_RD_DELAY:
	   if (delay == 7)
	     begin
		delay <= 0;
		state <= `ST_RD_IDLE;
	     end
	   else
	     delay <= delay + 1'b1;

       endcase // case(state)
   
   assign rx_i_o = fifo_out[31:16];
   assign rx_q_o = fifo_out[15:0];
   
endmodule // radar_rx
