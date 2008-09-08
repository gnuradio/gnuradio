//////////////////////////////////////////////////////////////////////
////                                                              ////
////  uart_fifo.v                                                 ////
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
////  UART core receiver FIFO                                     ////
////                                                              ////
////  Known problems (limits):                                    ////
////  Note that the same FIFO is used for both transmission  and  ////
////  reception but the error bit generation is ignored in tx.    ////
////                                                              ////
////  To Do:                                                      ////
////  Nothing.                                                    ////
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
// $Log: uart_fifo.v,v $
// Revision 1.3  2001/05/31 20:08:01  gorban
// FIFO changes and other corrections.
//
// Revision 1.3  2001/05/27 17:37:48  gorban
// Fixed many bugs. Updated spec. Changed FIFO files structure. See CHANGES.txt file.
//
// Revision 1.2  2001/05/17 18:34:18  gorban
// First 'stable' release. Should be sythesizable now. Also added new header.
//
// Revision 1.0  2001-05-17 21:27:12+02  jacob
// Initial revision
//
//

`include "timescale.v"
`include "uart_defines.v"

module uart_fifo (clk, 
	wb_rst_i, data_in, data_out,
// Control signals
	push, // push strobe, active high
	pop,   // pop strobe, active high
// status signals
	underrun,
	overrun,
	count,
	error_bit,
	fifo_reset,
	reset_status
	);


// FIFO parameters
parameter fifo_width = `UART_FIFO_WIDTH;
parameter fifo_depth = `UART_FIFO_DEPTH;
parameter fifo_pointer_w = `UART_FIFO_POINTER_W;
parameter fifo_counter_w = `UART_FIFO_COUNTER_W;

input				clk;
input				wb_rst_i;
input				push;
input				pop;
input	[fifo_width-1:0]	data_in;
input				fifo_reset;
input       reset_status;

output	[fifo_width-1:0]	data_out;
output				overrun;
output				underrun;
output	[fifo_counter_w-1:0]	count;
output				error_bit;

wire	[fifo_width-1:0]	data_out;

// FIFO itself
reg	[fifo_width-1:0]	fifo[fifo_depth-1:0];

// FIFO pointers
reg	[fifo_pointer_w-1:0]	top;
reg	[fifo_pointer_w-1:0]	bottom;

reg	[fifo_counter_w-1:0]	count;
reg				overrun;
reg				underrun;

// These registers and signals are to detect rise of of the signals.
// Not that it slows the maximum rate by 2, meaning you must reset the signals and then
// assert them again for the operation to repeat
// This is done to accomodate wait states
reg				push_delay; 
reg				pop_delay;

wire				push_rise = push_delay & push;
wire				pop_rise  = pop_delay  & pop;

wire [fifo_pointer_w-1:0] top_plus_1 = top + 1;

always @(posedge clk or posedge wb_rst_i)
begin
	if (wb_rst_i)
		push_delay <= #1 1'b0;
	else
		push_delay <= #1 ~push;
end

always @(posedge clk or posedge wb_rst_i)
begin
	if (wb_rst_i)
		pop_delay <= #1 1'b0;
	else
		pop_delay <= #1 ~pop;
end


always @(posedge clk or posedge wb_rst_i) // synchronous FIFO
begin
	if (wb_rst_i)
	begin
		top		<= #1 0;
//		bottom		<= #1 1; igor
		bottom		<= #1 1'b0;
		underrun	<= #1 1'b0;
		overrun		<= #1 1'b0;
		count		<= #1 0;
		fifo[0]		<= #1 0;
		fifo[1]		<= #1 0;
		fifo[2]		<= #1 0;
		fifo[3]		<= #1 0;
		fifo[4]		<= #1 0;
		fifo[5]		<= #1 0;
		fifo[6]		<= #1 0;
		fifo[7]		<= #1 0;
		fifo[8]		<= #1 0;
		fifo[9]		<= #1 0;
		fifo[10]	<= #1 0;
		fifo[11]	<= #1 0;
		fifo[12]	<= #1 0;
		fifo[13]	<= #1 0;
		fifo[14]	<= #1 0;
		fifo[15]	<= #1 0;
	end
	else
	if (fifo_reset) begin
		top		<= #1 0;
//		bottom		<= #1 1; igor
		bottom		<= #1 1'b0;
		underrun	<= #1 1'b0;
		overrun		<= #1 1'b0;
		count		<= #1 0;
	end
  else
  if(reset_status)
    begin
  		underrun	<= #1 1'b0;
  		overrun		<= #1 1'b0;
    end
	else
	begin
		case ({push_rise, pop_rise})
		2'b00 : begin
				underrun <= #1 1'b0;
//				overrun  <= #1 1'b0;// Igor Ko se postavita ostaneta aktivna tako dolgo, dokler se ne naredi read LSR registra
	 	        end
		2'b10 : if (count==fifo_depth)  // overrun condition
			begin
				overrun   <= #1 1'b1;
				underrun  <= #1 1'b0;
			end
			else
			begin
				top       <= #1 top_plus_1;
//				fifo[top_plus_1] <= #1 data_in; igor
				fifo[top] <= #1 data_in;
//				overrun   <= #1 0;// Igor Ko se postavita ostaneta aktivna tako dolgo, dokler se ne naredi read LSR registra
				overrun   <= #1 0;
				count     <= #1 count + 1;
			end
		2'b01 : if (~|count)
			begin
//				overrun  <= #1 1'b0;  Igor Ko se postavita ostaneta aktivna tako dolgo, dokler se ne naredi read LSR registra
				overrun  <= #1 1'b0;
			end
			else
			begin
				bottom   <= #1 bottom + 1;
//				overrun  <= #1 1'b0;  Igor Ko se postavita ostaneta aktivna tako dolgo, dokler se ne naredi read LSR registra
				overrun  <= #1 1'b0;
				count	 <= #1 count - 1;
			end
		2'b11 : begin
				bottom   <= #1 bottom + 1;
				top       <= #1 top_plus_1;
//				fifo[top_plus_1] <= #1 data_in; igor
				fifo[top] <= #1 data_in;
				underrun <= #1 1'b0;
//				overrun  <= #1 1'b0;  Igor Ko se postavita ostaneta aktivna tako dolgo, dokler se ne naredi read LSR registra
		        end
		endcase
	end

end   // always

// please note though that data_out is only valid one clock after pop signal
assign data_out = fifo[bottom];

// Additional logic for detection of error conditions (parity and framing) inside the FIFO
// for the Line Status Register bit 7

wire	[fifo_width-1:0]	word0 = fifo[0];
wire	[fifo_width-1:0]	word1 = fifo[1];
wire	[fifo_width-1:0]	word2 = fifo[2];
wire	[fifo_width-1:0]	word3 = fifo[3];
wire	[fifo_width-1:0]	word4 = fifo[4];
wire	[fifo_width-1:0]	word5 = fifo[5];
wire	[fifo_width-1:0]	word6 = fifo[6];
wire	[fifo_width-1:0]	word7 = fifo[7];

wire	[fifo_width-1:0]	word8 = fifo[8];
wire	[fifo_width-1:0]	word9 = fifo[9];
wire	[fifo_width-1:0]	word10 = fifo[10];
wire	[fifo_width-1:0]	word11 = fifo[11];
wire	[fifo_width-1:0]	word12 = fifo[12];
wire	[fifo_width-1:0]	word13 = fifo[13];
wire	[fifo_width-1:0]	word14 = fifo[14];
wire	[fifo_width-1:0]	word15 = fifo[15];

// a 1 is returned if any of the error bits in the fifo is 1
assign	error_bit = |(word0[1:0]  | word1[1:0]  | word2[1:0]  | word3[1:0]  |
            		      word4[1:0]  | word5[1:0]  | word6[1:0]  | word7[1:0]  |
            		      word8[1:0]  | word9[1:0]  | word10[1:0] | word11[1:0] |
            		      word12[1:0] | word13[1:0] | word14[1:0] | word15[1:0] );

endmodule
