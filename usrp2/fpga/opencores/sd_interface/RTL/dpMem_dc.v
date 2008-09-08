//////////////////////////////////////////////////////////////////////
////                                                              ////
//// dpMem_dc.v                                                 ////
////                                                              ////
//// This file is part of the usbhostslave opencores effort.
//// <http://www.opencores.org/cores//>                           ////
////                                                              ////
//// Module Description:                                          ////
//// Synchronous dual port memory with dual clocks
////                                                              ////
//// To Do:                                                       ////
//// 
////                                                              ////
//// Author(s):                                                   ////
//// - Steve Fielding, sfielding@base2designs.com                 ////
////                                                              ////
//////////////////////////////////////////////////////////////////////
////                                                              ////
//// Copyright (C) 2004 Steve Fielding and OPENCORES.ORG          ////
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
//// PURPOSE. See the GNU Lesser General Public License for more  ////
//// details.                                                     ////
////                                                              ////
//// You should have received a copy of the GNU Lesser General    ////
//// Public License along with this source; if not, download it   ////
//// from <http://www.opencores.org/lgpl.shtml>                   ////
////                                                              ////
//////////////////////////////////////////////////////////////////////
//
`include "timescale.v"

module dpMem_dc(  addrIn, addrOut, wrClk, rdClk, dataIn, writeEn, readEn, dataOut);
  //FIFO_DEPTH = ADDR_WIDTH^2
  parameter FIFO_WIDTH = 8;
  parameter FIFO_DEPTH = 64; 
  parameter ADDR_WIDTH = 6;   
  
input wrClk;
input rdClk;
input [FIFO_WIDTH-1:0] dataIn;
output [FIFO_WIDTH-1:0] dataOut;
input writeEn;
input readEn;
input [ADDR_WIDTH-1:0] addrIn;
input [ADDR_WIDTH-1:0] addrOut;

wire wrClk;
wire rdClk;
wire [FIFO_WIDTH-1:0] dataIn;
reg [FIFO_WIDTH-1:0] dataOut;
wire writeEn;
wire readEn;
wire [ADDR_WIDTH-1:0] addrIn;
wire [ADDR_WIDTH-1:0] addrOut;

reg [FIFO_WIDTH-1:0] buffer [0:FIFO_DEPTH-1];

// synchronous read. Introduces one clock cycle delay
always @(posedge rdClk) begin
  dataOut <= buffer[addrOut];
end

// synchronous write
always @(posedge wrClk) begin
  if (writeEn == 1'b1)
    buffer[addrIn] <= dataIn;
end                  


endmodule
