
//////////////////////////////////////////////////////////////////////
////                                                              ////
//// readWriteSPIWireData.v                      ////
////                                                              ////
//// This file is part of the spiMaster opencores effort.
//// <http://www.opencores.org/cores//>                           ////
////                                                              ////
//// Module Description:                                          ////
////  parameterized dual clock domain fifo. 
////  fifo depth is restricted to 2^ADDR_WIDTH
////  No protection against over runs and under runs.
//// 
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
`include "spiMaster_defines.v"

module readWriteSPIWireData (clk, clkDelay, rst, rxDataOut, rxDataRdySet, spiClkOut, spiDataIn, spiDataOut, txDataEmpty, txDataFull, txDataFullClr, txDataIn);
input   clk;
input   [7:0]clkDelay;
input   rst;
input   spiDataIn;
input   txDataFull;
input   [7:0]txDataIn;
output  [7:0]rxDataOut;
output  rxDataRdySet;
output  spiClkOut;
output  spiDataOut;
output  txDataEmpty;
output  txDataFullClr;

wire    clk;
wire    [7:0]clkDelay;
wire    rst;
reg     [7:0]rxDataOut, next_rxDataOut;
reg     rxDataRdySet, next_rxDataRdySet;
reg     spiClkOut, next_spiClkOut;
wire    spiDataIn;
reg     spiDataOut, next_spiDataOut;
reg     txDataEmpty, next_txDataEmpty;
wire    txDataFull;
reg     txDataFullClr, next_txDataFullClr;
wire    [7:0]txDataIn;

// diagram signals declarations
reg  [3:0]bitCnt, next_bitCnt;
reg  [7:0]clkDelayCnt, next_clkDelayCnt;
reg  [7:0]rxDataShiftReg, next_rxDataShiftReg;
reg  [7:0]txDataShiftReg, next_txDataShiftReg;

// BINARY ENCODED state machine: rwSPISt
// State codes definitions:
`define WT_TX_DATA 2'b00
`define CLK_HI 2'b01
`define CLK_LO 2'b10
`define ST_RW_WIRE 2'b11

reg [1:0]CurrState_rwSPISt, NextState_rwSPISt;

// Diagram actions (continuous assignments allowed only: assign ...)
// diagram ACTION


// Machine: rwSPISt

// NextState logic (combinatorial)
always @ (txDataFull or txDataIn or clkDelayCnt or clkDelay or txDataShiftReg or bitCnt or rxDataShiftReg or spiDataIn or rxDataRdySet or txDataEmpty or txDataFullClr or spiClkOut or spiDataOut or rxDataOut or CurrState_rwSPISt)
begin
  NextState_rwSPISt <= CurrState_rwSPISt;
  // Set default values for outputs and signals
  next_rxDataRdySet <= rxDataRdySet;
  next_txDataEmpty <= txDataEmpty;
  next_txDataShiftReg <= txDataShiftReg;
  next_rxDataShiftReg <= rxDataShiftReg;
  next_bitCnt <= bitCnt;
  next_clkDelayCnt <= clkDelayCnt;
  next_txDataFullClr <= txDataFullClr;
  next_spiClkOut <= spiClkOut;
  next_spiDataOut <= spiDataOut;
  next_rxDataOut <= rxDataOut;
  case (CurrState_rwSPISt)  // synopsys parallel_case full_case
    `WT_TX_DATA:
    begin
      next_rxDataRdySet <= 1'b0;
      next_txDataEmpty <= 1'b1;
      if (txDataFull == 1'b1)
      begin
        NextState_rwSPISt <= `CLK_HI;
        next_txDataShiftReg <= txDataIn;
        next_rxDataShiftReg <= 8'h00;
        next_bitCnt <= 4'h0;
        next_clkDelayCnt <= 8'h00;
        next_txDataFullClr <= 1'b1;
        next_txDataEmpty <= 1'b0;
      end
    end
    `CLK_HI:
    begin
      next_clkDelayCnt <= clkDelayCnt + 1'b1;
      next_txDataFullClr <= 1'b0;
      next_rxDataRdySet <= 1'b0;
      if (clkDelayCnt == clkDelay)
      begin
        NextState_rwSPISt <= `CLK_LO;
        next_spiClkOut <= 1'b0;
        next_spiDataOut <= txDataShiftReg[7];
        next_txDataShiftReg <= {txDataShiftReg[6:0], 1'b0};
        next_clkDelayCnt <= 8'h00;
      end
    end
    `CLK_LO:
    begin
      next_clkDelayCnt <= clkDelayCnt + 1'b1;
      if ((bitCnt == 4'h8) && (txDataFull == 1'b1))
      begin
        NextState_rwSPISt <= `CLK_HI;
        next_rxDataRdySet <= 1'b1;
        next_rxDataOut <= rxDataShiftReg;
        next_txDataShiftReg <= txDataIn;
        next_bitCnt <= 3'b000;
        next_clkDelayCnt <= 8'h00;
        next_txDataFullClr <= 1'b1;
      end
      else if (bitCnt == 4'h8)
      begin
        NextState_rwSPISt <= `WT_TX_DATA;
        next_rxDataRdySet <= 1'b1;
        next_rxDataOut <= rxDataShiftReg;
      end
      else if (clkDelayCnt == clkDelay)
      begin
        NextState_rwSPISt <= `CLK_HI;
        next_spiClkOut <= 1'b1;
        next_bitCnt <= bitCnt + 1'b1;
        next_clkDelayCnt <= 8'h00;
        next_rxDataShiftReg <= {rxDataShiftReg[6:0], spiDataIn};
      end
    end
    `ST_RW_WIRE:
    begin
      next_bitCnt <= 4'h0;
      next_clkDelayCnt <= 8'h00;
      next_txDataFullClr <= 1'b0;
      next_rxDataRdySet <= 1'b0;
      next_txDataShiftReg <= 8'h00;
      next_rxDataShiftReg <= 8'h00;
      next_rxDataOut <= 8'h00;
      next_spiDataOut <= 1'b0;
      next_spiClkOut <= 1'b0;
      next_txDataEmpty <= 1'b0;
      NextState_rwSPISt <= `WT_TX_DATA;
    end
  endcase
end

// Current State Logic (sequential)
always @ (posedge clk)
begin
  if (rst == 1'b1)
    CurrState_rwSPISt <= `ST_RW_WIRE;
  else
    CurrState_rwSPISt <= NextState_rwSPISt;
end

// Registered outputs logic
always @ (posedge clk)
begin
  if (rst == 1'b1)
  begin
    rxDataRdySet <= 1'b0;
    txDataEmpty <= 1'b0;
    txDataFullClr <= 1'b0;
    spiClkOut <= 1'b0;
    spiDataOut <= 1'b0;
    rxDataOut <= 8'h00;
    txDataShiftReg <= 8'h00;
    rxDataShiftReg <= 8'h00;
    bitCnt <= 4'h0;
    clkDelayCnt <= 8'h00;
  end
  else 
  begin
    rxDataRdySet <= next_rxDataRdySet;
    txDataEmpty <= next_txDataEmpty;
    txDataFullClr <= next_txDataFullClr;
    spiClkOut <= next_spiClkOut;
    spiDataOut <= next_spiDataOut;
    rxDataOut <= next_rxDataOut;
    txDataShiftReg <= next_txDataShiftReg;
    rxDataShiftReg <= next_rxDataShiftReg;
    bitCnt <= next_bitCnt;
    clkDelayCnt <= next_clkDelayCnt;
  end
end

endmodule