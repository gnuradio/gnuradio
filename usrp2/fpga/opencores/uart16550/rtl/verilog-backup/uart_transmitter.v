//////////////////////////////////////////////////////////////////////
////                                                              ////
////  uart_transmitter.v                                          ////
////                                                              ////
////                                                              ////
////  This file is part of the "UART 16550 compatible" project    ////
////  http://www.opencores.org/cores/uart16550/                   ////
////                                                              ////
////  Documentation related to this project:                      ////
////  - http://www.opencores.org/cores/uart16550/                 ////
////                                                              ////
////  Projects compatibility:                                     ////
////  - WISHBONE                                                  ////
////  RS232 Protocol                                              ////
////  16550D uart (mostly supported)                              ////
////                                                              ////
////  Overview (main Features):                                   ////
////  UART core transmitter logic                                 ////
////                                                              ////
////  Known problems (limits):                                    ////
////  None known                                                  ////
////                                                              ////
////  To Do:                                                      ////
////  Thourough testing.                                          ////
////                                                              ////
////  Author(s):                                                  ////
////      - gorban@opencores.org                                  ////
////      - Jacob Gorban                                          ////
////                                                              ////
////  Created:        2001/05/12                                  ////
////  Last Updated:   2001/05/17                                  ////
////                  (See log for the revision history)          ////
////                                                              ////
////                                                              ////
//////////////////////////////////////////////////////////////////////
////                                                              ////
//// Copyright (C) 2000 Jacob Gorban, gorban@opencores.org        ////
////                                                              ////
//// This source file may be used and distributed without         ////
//// restriction provided that this copyright statement is not    ////
//// removed from the file and that any derivative work contains  ////
//// the original copyright notice and the associated disclaimer. ////
////                                                              ////
//// This source file is free software; you can redistribute it   ////
//// and/or modify it under the terms of the GNU Lesser General   ////
//// Public License as published by the Free Software Foundation; ////
//// either version 2.1 of the License, or (at your option) any   ////
//// later version.                                               ////
////                                                              ////
//// This source is distributed in the hope that it will be       ////
//// useful, but WITHOUT ANY WARRANTY; without even the implied   ////
//// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR      ////
//// PURPOSE.  See the GNU Lesser General Public License for more ////
//// details.                                                     ////
////                                                              ////
//// You should have received a copy of the GNU Lesser General    ////
//// Public License along with this source; if not, download it   ////
//// from http://www.opencores.org/lgpl.shtml                     ////
////                                                              ////
//////////////////////////////////////////////////////////////////////
//
// CVS Revision History
//
// $Log: uart_transmitter.v,v $
// Revision 1.6  2001/06/23 11:21:48  gorban
// DL made 16-bit long. Fixed transmission/reception bugs.
//
// Revision 1.5  2001/06/02 14:28:14  gorban
// Fixed receiver and transmitter. Major bug fixed.
//
// Revision 1.4  2001/05/31 20:08:01  gorban
// FIFO changes and other corrections.
//
// Revision 1.3  2001/05/27 17:37:49  gorban
// Fixed many bugs. Updated spec. Changed FIFO files structure. See CHANGES.txt file.
//
// Revision 1.2  2001/05/21 19:12:02  gorban
// Corrected some Linter messages.
//
// Revision 1.1  2001/05/17 18:34:18  gorban
// First 'stable' release. Should be sythesizable now. Also added new header.
//
// Revision 1.0  2001-05-17 21:27:12+02  jacob
// Initial revision
//
//

`include "timescale.v"
`include "uart_defines.v"

module uart_transmitter (clk, wb_rst_i, lcr, tf_push, wb_dat_i, enable,	stx_pad_o, state, tf_count, tx_reset);

input				clk;
input				wb_rst_i;
input	[7:0]			lcr;
input				tf_push;
input	[7:0]			wb_dat_i;
input				enable;
input				tx_reset;
output				stx_pad_o;
output	[2:0]			state;
output	[`UART_FIFO_COUNTER_W-1:0]	tf_count;

reg	[2:0]	state;
reg	[4:0]	counter;
reg	[2:0]	bit_counter;   // counts the bits to be sent
reg	[6:0]	shift_out;	// output shift register
reg		stx_o_tmp;
reg		parity_xor;  // parity of the word
reg		tf_pop;
reg		bit_out;

// TX FIFO instance
//
// Transmitter FIFO signals
wire	[`UART_FIFO_WIDTH-1:0]	tf_data_in;
wire	[`UART_FIFO_WIDTH-1:0]	tf_data_out;
wire				tf_push;
wire				tf_underrun;
wire				tf_overrun;
wire	[`UART_FIFO_COUNTER_W-1:0]	tf_count;

assign tf_data_in = wb_dat_i;

uart_fifo fifo_tx(	// error bit signal is not used in transmitter FIFO
	.clk(		clk		), 
	.wb_rst_i(	wb_rst_i	),
	.data_in(	tf_data_in	),
	.data_out(	tf_data_out	),
	.push(		tf_push		),
	.pop(		tf_pop		),
	.underrun(	tf_underrun	),
	.overrun(	tf_overrun	),
	.count(		tf_count	),
	.error_bit(),                 // Ta ni priklopljen. Prej je manjkal, dodal Igor
	.fifo_reset(	tx_reset	),
	.reset_status(1'b0)
);

// TRANSMITTER FINAL STATE MACHINE

parameter s_idle        = 3'd0;
parameter s_send_start  = 3'd1;
parameter s_send_byte   = 3'd2;
parameter s_send_parity = 3'd3;
parameter s_send_stop   = 3'd4;
parameter s_pop_byte    = 3'd5;

always @(posedge clk or posedge wb_rst_i)
begin
  if (wb_rst_i)
  begin
	state       <= #1 s_idle;
	stx_o_tmp       <= #1 1'b1;
	counter   <= #1 5'b0;
	shift_out   <= #1 7'b0;
	bit_out     <= #1 1'b0;
	parity_xor  <= #1 1'b0;
	tf_pop      <= #1 1'b0;
	bit_counter <= #1 3'b0;
  end
  else
  if (enable)
  begin
	case (state)
	s_idle	 :	if (~|tf_count) // if tf_count==0
			begin
				state <= #1 s_idle;
				stx_o_tmp <= #1 1'b1;
			end
			else
			begin
				tf_pop <= #1 1'b0;
				stx_o_tmp  <= #1 1'b1;
				state  <= #1 s_pop_byte;
			end
	s_pop_byte :	begin
				tf_pop <= #1 1'b1;
				case (lcr[/*`UART_LC_BITS*/1:0])  // number of bits in a word
				2'b00 : begin
					bit_counter <= #1 3'b100;
					parity_xor  <= #1 ^tf_data_out[4:0];
				     end
				2'b01 : begin
					bit_counter <= #1 3'b101;
					parity_xor  <= #1 ^tf_data_out[5:0];
				     end
				2'b10 : begin
					bit_counter <= #1 3'b110;
					parity_xor  <= #1 ^tf_data_out[6:0];
				     end
				2'b11 : begin
					bit_counter <= #1 3'b111;
					parity_xor  <= #1 ^tf_data_out[7:0];
				     end
				endcase
				{shift_out[6:0], bit_out} <= #1 tf_data_out;
				state <= #1 s_send_start;
			end
	s_send_start :	begin
				tf_pop <= #1 1'b0;
				if (~|counter)
					counter <= #1 5'b01111;
				else
				if (counter == 5'b00001)
				begin
					counter <= #1 0;
					state <= #1 s_send_byte;
				end
				else
					counter <= #1 counter - 5'b00001;
				stx_o_tmp <= #1 1'b0;
			end
	s_send_byte :	begin
				if (~|counter)
					counter <= #1 5'b01111;
				else
				if (counter == 5'b00001)
				begin
					if (bit_counter > 3'b0)
					begin
						bit_counter <= #1 bit_counter - 1;
						{shift_out[5:0],bit_out  } <= #1 {shift_out[6:1], shift_out[0]};
						state <= #1 s_send_byte;
					end
					else   // end of byte
					if (~lcr[`UART_LC_PE])
					begin
						state <= #1 s_send_stop;
					end
					else
					begin
						case ({lcr[`UART_LC_EP],lcr[`UART_LC_SP]})
						2'b00:	bit_out <= #1 parity_xor;
						2'b01:	bit_out <= #1 1'b1;
						2'b10:	bit_out <= #1 ~parity_xor;
						2'b11:	bit_out <= #1 1'b0;
						endcase
						state <= #1 s_send_parity;
					end
					counter <= #1 0;
				end
				else
					counter <= #1 counter - 5'b00001;
				stx_o_tmp <= #1 bit_out; // set output pin
			end
	s_send_parity :	begin
				if (~|counter)
					counter <= #1 5'b01111;
				else
				if (counter == 5'b00001)
				begin
					counter <= #1 4'b0;
					state <= #1 s_send_stop;
				end
				else
					counter <= #1 counter - 5'b00001;
				stx_o_tmp <= #1 bit_out;
			end
	s_send_stop :  begin
				if (~|counter)
				  begin
						casex ({lcr[`UART_LC_SB],lcr[`UART_LC_BITS]})
  						3'b0xx:	  counter <= #1 5'b01101;     // 1 stop bit ok igor
  						3'b100:	  counter <= #1 5'b10101;     // 1.5 stop bit
  						3'b1xx:	  counter <= #1 5'b11101;     // 2 stop bits
						endcase
					end
				else
				if (counter == 5'b00001)
				begin
					counter <= #1 0;
					state <= #1 s_idle;
				end
				else
					counter <= #1 counter - 5'b00001;
				stx_o_tmp <= #1 1'b1;
			end

		default : // should never get here
			state <= #1 s_idle;
	endcase
  end // end if enable
end // transmitter logic

assign stx_pad_o = lcr[`UART_LC_BC] ? 1'b0 : stx_o_tmp;    // Break condition
	
endmodule
