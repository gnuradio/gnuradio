// $Header: /devl/xcs/repo/env/Databases/CAEInterfaces/verunilibs/data/unisims/SRLC16E.v,v 1.6.158.1 2007/03/09 18:13:20 patrickp Exp $
///////////////////////////////////////////////////////////////////////////////
// Copyright (c) 1995/2004 Xilinx, Inc.
// All Right Reserved.
///////////////////////////////////////////////////////////////////////////////
//   ____  ____
//  /   /\/   /
// /___/  \  /    Vendor : Xilinx
// \   \   \/     Version : 8.1i (I.13)
//  \   \         Description : Xilinx Functional Simulation Library Component
//  /   /                  16-Bit Shift Register Look-Up-Table with Carry and Clock Enable
// /___/   /\     Filename : SRLC16E.v
// \   \  /  \    Timestamp : Thu Mar 25 16:43:40 PST 2004
//  \___\/\___\
//
// Revision:
//    03/23/04 - Initial version.
// End Revision

`timescale  1 ps / 1 ps


module SRLC16E (Q, Q15, A0, A1, A2, A3, CE, CLK, D);

    parameter INIT = 16'h0000;

    output Q, Q15;

    input  A0, A1, A2, A3, CE, CLK, D;

    reg  [15:0] data;
    wire [3:0]  addr;
    wire	q_int;

    buf b_a3 (addr[3], A3);
    buf b_a2 (addr[2], A2);
    buf b_a1 (addr[1], A1);
    buf b_a0 (addr[0], A0);

    buf b_q_int (q_int, data[addr]);
    buf b_q (Q, q_int);
    buf b_q15_int (q15_int, data[15]);
    buf b_q15 (Q15, q15_int);

    initial
    begin
          assign  data = INIT;
          while (CLK === 1'b1 || CLK===1'bX) 
            #10; 
          deassign data;
    end

    always @(posedge CLK) begin
	if (CE == 1'b1) begin
	    {data[15:0]} <= #100 {data[14:0], D};
	end
    end


endmodule

