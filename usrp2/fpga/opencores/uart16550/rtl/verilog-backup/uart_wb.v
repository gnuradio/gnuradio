//////////////////////////////////////////////////////////////////////
////                                                              ////
////  uart_TX_FIFO.v                                              ////
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
////  UART core WISHBONE interface.                               ////
////                                                              ////
////  Known problems (limits):                                    ////
////  Inserts one wait state on all transfers.                    ////
////  Note affected signals and the way they are affected.        ////
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
// $Log: uart_wb.v,v $
// Revision 1.4  2001/05/31 20:08:01  gorban
// FIFO changes and other corrections.
//
// Revision 1.3  2001/05/21 19:12:01  gorban
// Corrected some Linter messages.
//
// Revision 1.2  2001/05/17 18:34:18  gorban
// First 'stable' release. Should be sythesizable now. Also added new header.
//
// Revision 1.0  2001-05-17 21:27:13+02  jacob
// Initial revision
//
//

// UART core WISHBONE interface 
//
// Author: Jacob Gorban   (jacob.gorban@flextronicssemi.com)
// Company: Flextronics Semiconductor
//

`include "timescale.v"

module uart_wb (clk,
        wb_rst_i, 
	wb_we_i, wb_stb_i, wb_cyc_i, wb_ack_o,
	we_o, re_o // Write and read enable output for the core
	
        );

input				clk;

// WISHBONE interface	
input				wb_rst_i;
input				wb_we_i;
input				wb_stb_i;
input				wb_cyc_i;
output				wb_ack_o;
output				we_o;
output				re_o;

wire				we_o;
reg				wb_ack_o;

always @(posedge clk or posedge wb_rst_i)
begin
	if (wb_rst_i)
	begin
		wb_ack_o <= #1 1'b0;
	end
	else
	begin
//		wb_ack_o <= #1 wb_stb_i & wb_cyc_i; // 1 clock wait state on all transfers
		wb_ack_o <= #1 wb_stb_i & wb_cyc_i & ~wb_ack_o; // 1 clock wait state on all transfers
	end
end

assign we_o =  wb_we_i & wb_cyc_i & wb_stb_i; //WE for registers	
assign re_o = ~wb_we_i & wb_cyc_i & wb_stb_i; //RE for registers	

endmodule
