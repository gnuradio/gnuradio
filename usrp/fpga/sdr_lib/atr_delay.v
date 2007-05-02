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

module atr_delay(clk_i,rst_i,ena_i,tx_empty_i,tx_delay_i,rx_delay_i,atr_tx_o);
   input        clk_i;
   input        rst_i;
   input        ena_i;
   input        tx_empty_i;
   input [11:0] tx_delay_i;
   input [11:0] rx_delay_i;
   output       atr_tx_o;

   reg [3:0] 	state;
   reg [11:0] 	count;

   `define ST_RX_DELAY 4'b0001
   `define ST_RX       4'b0010
   `define ST_TX_DELAY 4'b0100
   `define ST_TX       4'b1000

   always @(posedge clk_i)
     if (rst_i | ~ena_i)
       begin
	  state <= `ST_RX;
	  count <= 12'b0;
       end
     else
       case (state)
	 `ST_RX:
	   if (!tx_empty_i)
	     begin
		state <= `ST_TX_DELAY;
		count <= tx_delay_i;
	     end

	 `ST_TX_DELAY:
	   if (count == 0)
	     state <= `ST_TX;
	   else
	     count <= count - 1;

	 `ST_TX:
	   if (tx_empty_i)
	     begin
		state <= `ST_RX_DELAY;
		count <= rx_delay_i;
	     end

	 `ST_RX_DELAY:
	   if (count == 0)
	     state <= `ST_RX;
	   else
	     count <= count - 1;
	 
	 default:		// Error
	   begin
	      state <= `ST_RX;
	      count <= 0;
	   end
       endcase
   
   assign atr_tx_o = (state == `ST_TX) | (state == `ST_RX_DELAY);
   
endmodule // atr_delay

