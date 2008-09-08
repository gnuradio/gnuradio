// $Header: /devl/xcs/repo/env/Databases/CAEInterfaces/verunilibs/data/unisims/SRL16E.v,v 1.7 2005/03/14 22:32:58 yanx Exp $
///////////////////////////////////////////////////////////////////////////////
// Copyright (c) 1995/2004 Xilinx, Inc.
// All Right Reserved.
///////////////////////////////////////////////////////////////////////////////
//   ____  ____
//  /   /\/   /
// /___/  \  /    Vendor : Xilinx
// \   \   \/     Version : 8.1i (I.13)
//  \   \         Description : Xilinx Functional Simulation Library Component
//  /   /                  16-Bit Shift Register Look-Up-Table with Clock Enable
// /___/   /\     Filename : SRL16E.v
// \   \  /  \    Timestamp : Thu Mar 25 16:43:40 PST 2004
//  \___\/\___\
//
// Revision:
//    03/23/04 - Initial version.
// End Revision

`timescale  1 ps / 1 ps


module SRL16E (Q, A0, A1, A2, A3, CE, CLK, D);

    parameter INIT = 16'h0000;

    output Q;

    input  A0, A1, A2, A3, CE, CLK, D;

    reg  [15:0] data;


    assign Q = data[{A3, A2, A1, A0}];

    initial
    begin
          assign  data = INIT;
          while (CLK === 1'b1 || CLK===1'bX) 
            #10; 
          deassign data;
    end

    always @(posedge CLK)
    begin
	if (CE == 1'b1) begin
	    {data[15:0]} <= #100 {data[14:0], D};
	end
    end


endmodule

