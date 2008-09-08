//////////////////////////////////////////////////////////////////////
////                                                              ////
////  uart_receiver.v                                             ////
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
////  UART core receiver logic                                    ////
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
// $Log: uart_receiver.v,v $
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
// Revision 1.0  2001-05-17 21:27:11+02  jacob
// Initial revision
//
//

`include "timescale.v"
`include "uart_defines.v"

module uart_receiver (clk, wb_rst_i, lcr, rf_pop, srx_pad_i, enable, rda_int,
	counter_t, counter_b, rf_count, rf_data_out, rf_error_bit, rf_overrun, rx_reset, rx_lsr_mask);

input				clk;
input				wb_rst_i;
input	[7:0]			lcr;
input				rf_pop;
input				srx_pad_i;
input				enable;
input				rda_int;
input				rx_reset;
input       rx_lsr_mask;

output	[5:0]			counter_t;
output	[3:0]			counter_b;
output	[`UART_FIFO_COUNTER_W-1:0]	rf_count;
output	[`UART_FIFO_REC_WIDTH-1:0]	rf_data_out;
output				rf_overrun;
output				rf_error_bit;

reg	[3:0]	rstate;
reg	[3:0]	rcounter16;
reg	[2:0]	rbit_counter;
reg	[7:0]	rshift;			// receiver shift register
reg		rparity;		// received parity
reg		rparity_error;
reg		rframing_error;		// framing error flag
reg		rbit_in;
reg		rparity_xor;

// RX FIFO signals
reg	[`UART_FIFO_REC_WIDTH-1:0]	rf_data_in;
wire	[`UART_FIFO_REC_WIDTH-1:0]	rf_data_out;
reg				rf_push;
wire				rf_pop;
wire				rf_underrun;
wire				rf_overrun;
wire	[`UART_FIFO_COUNTER_W-1:0]	rf_count;
wire				rf_error_bit; // an error (parity or framing) is inside the fifo

// RX FIFO instance
uart_fifo #(`UART_FIFO_REC_WIDTH) fifo_rx(
	.clk(		clk		), 
	.wb_rst_i(	wb_rst_i	),
	.data_in(	rf_data_in	),
	.data_out(	rf_data_out	),
	.push(		rf_push		),
	.pop(		rf_pop		),
	.underrun(	rf_underrun	),
	.overrun(	rf_overrun	),
	.count(		rf_count	),
	.error_bit(	rf_error_bit	),
	.fifo_reset(	rx_reset	),
	.reset_status(rx_lsr_mask)
);

wire 		rcounter16_eq_7 = (rcounter16 == 4'd7);
wire		rcounter16_eq_0 = (rcounter16 == 4'd0);
wire		rcounter16_eq_1 = (rcounter16 == 4'd1);

wire [3:0] rcounter16_minus_1 = rcounter16 - 4'd1;

parameter  sr_idle 					= 4'd0;
parameter  sr_rec_start 			= 4'd1;
parameter  sr_rec_bit 				= 4'd2;
parameter  sr_rec_parity			= 4'd3;
parameter  sr_rec_stop 				= 4'd4;
parameter  sr_check_parity 		= 4'd5;
parameter  sr_rec_prepare 			= 4'd6;
parameter  sr_end_bit				= 4'd7;
parameter  sr_ca_lc_parity	      = 4'd8;
parameter  sr_wait1 					= 4'd9;
parameter  sr_push 					= 4'd10;
parameter  sr_last 					= 4'd11;

always @(posedge clk or posedge wb_rst_i)
begin
  if (wb_rst_i)
  begin
     rstate 			<= #1 sr_idle;
	  rbit_in 				<= #1 1'b0;
	  rcounter16 			<= #1 0;
	  rbit_counter 		<= #1 0;
	  rparity_xor 		<= #1 1'b0;
	  rframing_error 	<= #1 1'b0;
	  rparity_error 		<= #1 1'b0;
	  rparity 				<= #1 1'b0;
	  rshift 				<= #1 0;
	  rf_push 				<= #1 1'b0;
	  rf_data_in 			<= #1 0;
  end
  else
  if (enable)
  begin
	case (rstate)
	sr_idle :	if (srx_pad_i==1'b0)   // detected a pulse (start bit?)
			begin
				rstate <= #1 sr_rec_start;
				rcounter16 <= #1 4'b1110;
			end
			else
				rstate <= #1 sr_idle;
	sr_rec_start :	begin
				if (rcounter16_eq_7)    // check the pulse
					if (srx_pad_i==1'b1)   // no start bit
						rstate <= #1 sr_idle;
					else            // start bit detected
						rstate <= #1 sr_rec_prepare;
				rcounter16 <= #1 rcounter16_minus_1;
			end
	sr_rec_prepare:begin
				case (lcr[/*`UART_LC_BITS*/1:0])  // number of bits in a word
				2'b00 : rbit_counter <= #1 3'b100;
				2'b01 : rbit_counter <= #1 3'b101;
				2'b10 : rbit_counter <= #1 3'b110;
				2'b11 : rbit_counter <= #1 3'b111;
				endcase
				if (rcounter16_eq_0)
				begin
					rstate		<= #1 sr_rec_bit;
					rcounter16	<= #1 4'b1110;
					rshift		<= #1 0;
				end
				else
					rstate <= #1 sr_rec_prepare;
				rcounter16 <= #1 rcounter16_minus_1;
			end
	sr_rec_bit :	begin
				if (rcounter16_eq_0)
					rstate <= #1 sr_end_bit;
				if (rcounter16_eq_7) // read the bit
					case (lcr[/*`UART_LC_BITS*/1:0])  // number of bits in a word
					2'b00 : rshift[4:0]  <= #1 {srx_pad_i, rshift[4:1]};
					2'b01 : rshift[5:0]  <= #1 {srx_pad_i, rshift[5:1]};
					2'b10 : rshift[6:0]  <= #1 {srx_pad_i, rshift[6:1]};
					2'b11 : rshift[7:0]  <= #1 {srx_pad_i, rshift[7:1]};
					endcase
				rcounter16 <= #1 rcounter16_minus_1;
			end
	sr_end_bit :   begin
				if (rbit_counter==3'b0) // no more bits in word
					if (lcr[`UART_LC_PE]) // choose state based on parity
						rstate <= #1 sr_rec_parity;
					else
					begin
						rstate <= #1 sr_rec_stop;
						rparity_error <= #1 1'b0;  // no parity - no error :)
					end
				else		// else we have more bits to read
				begin
					rstate <= #1 sr_rec_bit;
					rbit_counter <= #1 rbit_counter - 3'b1;
				end
				rcounter16 <= #1 4'b1110;
			end
	sr_rec_parity: begin
				if (rcounter16_eq_7)	// read the parity
				begin
					rparity <= #1 srx_pad_i;
					rstate <= #1 sr_ca_lc_parity;
				end
				rcounter16 <= #1 rcounter16_minus_1;
			end
	sr_ca_lc_parity : begin    // rcounter equals 6
				rcounter16  <= #1 rcounter16_minus_1;
				rparity_xor <= #1 ^{rshift,rparity}; // calculate parity on all incoming data
				rstate      <= #1 sr_check_parity;
			  end
	sr_check_parity: begin	  // rcounter equals 5
				case ({lcr[`UART_LC_EP],lcr[`UART_LC_SP]})
//				2'b00: rparity_error <= #1 ~rparity_xor;  // no error if parity 1
//				2'b01: rparity_error <= #1 ~rparity;      // parity should sticked to 1
//				2'b10: rparity_error <= #1 rparity_xor;   // error if parity is odd
//				2'b11: rparity_error <= #1 rparity;	  // parity should be sticked to 0
				2'b00: rparity_error <= #1 rparity_xor;  // no error if parity 1
				2'b01: rparity_error <= #1 1'b1;      // parity should sticked to 1
				2'b10: rparity_error <= #1 ~rparity_xor;   // error if parity is odd
				2'b11: rparity_error <= #1 1'b0;	  // parity should be sticked to 0
				endcase
				rcounter16 <= #1 rcounter16_minus_1;
				rstate <= #1 sr_wait1;
			  end
	sr_wait1 :	if (rcounter16_eq_0)
			begin
				rstate <= #1 sr_rec_stop;
				rcounter16 <= #1 4'b1110;
			end
			else
				rcounter16 <= #1 rcounter16_minus_1;
	sr_rec_stop :	begin
				if (rcounter16_eq_7)	// read the parity
				begin
					rframing_error <= #1 !srx_pad_i; // no framing error if input is 1 (stop bit)
					rf_data_in <= #1 {rshift, rparity_error, rframing_error};
					rstate <= #1 sr_push;
				end
				rcounter16 <= #1 rcounter16_minus_1;
			end
	sr_push :	begin
///////////////////////////////////////
//				$display($time, ": received: %b", rf_data_in);
				rf_push    <= #1 1'b1;
				rstate     <= #1 sr_last;
			end
	sr_last :	begin
				if (rcounter16_eq_1)
					rstate <= #1 sr_idle;
				rcounter16 <= #1 rcounter16_minus_1;
				rf_push    <= #1 1'b0;
			end
	default : rstate <= #1 sr_idle;
	endcase
  end  // if (enable)
end // always of receiver

//
// Break condition detection.
// Works in conjuction with the receiver state machine
reg	[3:0]	counter_b;	// counts the 1 (idle) signals

always @(posedge clk or posedge wb_rst_i)
begin
	if (wb_rst_i)
		counter_b <= #1 4'd11;
	else
	if (enable)  // only work on enable times
		if (!srx_pad_i)                                                       // Ta vrstica je bila spremenjena igor !!!
			counter_b <= #1 4'd11; // maximum character time length - 1
		else
			if (counter_b != 4'b0)            // break reached
				counter_b <= #1 counter_b - 4'd1;  // decrement break counter
end // always of break condition detection

///
/// Timeout condition detection
reg	[5:0]	counter_t;	// counts the timeout condition clocks

always @(posedge clk or posedge wb_rst_i)
begin
	if (wb_rst_i)
		counter_t <= #1 6'd44;
	else
	if (enable)
		if(rf_push || rf_pop || rda_int) // counter is reset when RX FIFO is accessed or above trigger level
			counter_t <= #1 6'd44;
		else
			if (counter_t != 6'b0)  // we don't want to underflow
				counter_t <= #1 counter_t - 6'd1;		
end
	
endmodule
