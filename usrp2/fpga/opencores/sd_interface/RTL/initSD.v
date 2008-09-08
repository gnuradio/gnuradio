
//////////////////////////////////////////////////////////////////////
////                                                              ////
//// initSD.v                                                 ////
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

module initSD (checkSumByte, clk, cmdByte, dataByte1, dataByte2, dataByte3, dataByte4, initError, respByte, respTout, rst, rxDataRdy, rxDataRdyClr, SDInitRdy, SDInitReq, sendCmdRdy, sendCmdReq, spiClkDelayIn, spiClkDelayOut, spiCS_n, txDataEmpty, txDataFull, txDataOut, txDataWen);
input   clk;
input   [7:0]respByte;
input   respTout;
input   rst;
input   rxDataRdy;
input   SDInitReq;
input   sendCmdRdy;
input   [7:0]spiClkDelayIn;
input   txDataEmpty;
input   txDataFull;
output  [7:0]checkSumByte;
output  [7:0]cmdByte;
output  [7:0]dataByte1;
output  [7:0]dataByte2;
output  [7:0]dataByte3;
output  [7:0]dataByte4;
output  [1:0]initError;
output  rxDataRdyClr;
output  SDInitRdy;
output  sendCmdReq;
output  [7:0]spiClkDelayOut;
output  spiCS_n;
output  [7:0]txDataOut;
output  txDataWen;

reg     [7:0]checkSumByte, next_checkSumByte;
wire    clk;
reg     [7:0]cmdByte, next_cmdByte;
reg     [7:0]dataByte1, next_dataByte1;
reg     [7:0]dataByte2, next_dataByte2;
reg     [7:0]dataByte3, next_dataByte3;
reg     [7:0]dataByte4, next_dataByte4;
reg     [1:0]initError, next_initError;
wire    [7:0]respByte;
wire    respTout;
wire    rst;
wire    rxDataRdy;
reg     rxDataRdyClr, next_rxDataRdyClr;
reg     SDInitRdy, next_SDInitRdy;
wire    SDInitReq;
wire    sendCmdRdy;
reg     sendCmdReq, next_sendCmdReq;
wire    [7:0]spiClkDelayIn;
reg     [7:0]spiClkDelayOut, next_spiClkDelayOut;
reg     spiCS_n, next_spiCS_n;
wire    txDataEmpty;
wire    txDataFull;
reg     [7:0]txDataOut, next_txDataOut;
reg     txDataWen, next_txDataWen;

// diagram signals declarations
reg  [9:0]delCnt1, next_delCnt1;
reg  [7:0]delCnt2, next_delCnt2;
reg  [7:0]loopCnt, next_loopCnt;

// BINARY ENCODED state machine: initSDSt
// State codes definitions:
`define START 4'b0000
`define WT_INIT_REQ 4'b0001
`define CLK_SEQ_SEND_FF 4'b0010
`define CLK_SEQ_CHK_FIN 4'b0011
`define RESET_SEND_CMD 4'b0100
`define RESET_DEL 4'b0101
`define RESET_WT_FIN 4'b0110
`define RESET_CHK_FIN 4'b0111
`define INIT_WT_FIN 4'b1000
`define INIT_CHK_FIN 4'b1001
`define INIT_SEND_CMD 4'b1010
`define INIT_DEL1 4'b1011
`define INIT_DEL2 4'b1100
`define CLK_SEQ_WT_DATA_EMPTY 4'b1101

reg [3:0]CurrState_initSDSt, NextState_initSDSt;

// Diagram actions (continuous assignments allowed only: assign ...)
// diagram ACTION


// Machine: initSDSt

// NextState logic (combinatorial)
always @ (spiClkDelayIn or SDInitReq or txDataFull or loopCnt or sendCmdRdy or respTout or respByte or delCnt1 or delCnt2 or txDataEmpty or spiClkDelayOut or SDInitRdy or spiCS_n or initError or txDataOut or txDataWen or cmdByte or dataByte1 or dataByte2 or dataByte3 or dataByte4 or checkSumByte or sendCmdReq or rxDataRdyClr or CurrState_initSDSt)
begin
  NextState_initSDSt <= CurrState_initSDSt;
  // Set default values for outputs and signals
  next_spiClkDelayOut <= spiClkDelayOut;
  next_SDInitRdy <= SDInitRdy;
  next_spiCS_n <= spiCS_n;
  next_initError <= initError;
  next_txDataOut <= txDataOut;
  next_txDataWen <= txDataWen;
  next_cmdByte <= cmdByte;
  next_dataByte1 <= dataByte1;
  next_dataByte2 <= dataByte2;
  next_dataByte3 <= dataByte3;
  next_dataByte4 <= dataByte4;
  next_checkSumByte <= checkSumByte;
  next_sendCmdReq <= sendCmdReq;
  next_loopCnt <= loopCnt;
  next_delCnt1 <= delCnt1;
  next_delCnt2 <= delCnt2;
  next_rxDataRdyClr <= rxDataRdyClr;
  case (CurrState_initSDSt)  // synopsys parallel_case full_case
    `START:
    begin
      next_spiClkDelayOut <= spiClkDelayIn;
      next_SDInitRdy <= 1'b0;
      next_spiCS_n <= 1'b1;
      next_initError <= `INIT_NO_ERROR;
      next_txDataOut <= 8'h00;
      next_txDataWen <= 1'b0;
      next_cmdByte <= 8'h00;
      next_dataByte1 <= 8'h00;
      next_dataByte2 <= 8'h00;
      next_dataByte3 <= 8'h00;
      next_dataByte4 <= 8'h00;
      next_checkSumByte <= 8'h00;
      next_sendCmdReq <= 1'b0;
      next_loopCnt <= 8'h00;
      next_delCnt1 <= 10'h000;
      next_delCnt2 <= 8'h00;
      next_rxDataRdyClr <= 1'b0;
      NextState_initSDSt <= `WT_INIT_REQ;
    end
    `WT_INIT_REQ:
    begin
      next_SDInitRdy <= 1'b1;
      next_spiClkDelayOut <= spiClkDelayIn;
      next_cmdByte <= 8'h00;
      next_dataByte1 <= 8'h00;
      next_dataByte2 <= 8'h00;
      next_dataByte3 <= 8'h00;
      next_dataByte4 <= 8'h00;
      next_checkSumByte <= 8'h00;
      if (SDInitReq == 1'b1)
      begin
        NextState_initSDSt <= `CLK_SEQ_SEND_FF;
        next_SDInitRdy <= 1'b0;
        next_loopCnt <= 8'h00;
        next_spiClkDelayOut <= `SLOW_SPI_CLK;
        next_initError <= `INIT_NO_ERROR;
      end
    end
    `CLK_SEQ_SEND_FF:
    begin
      if (txDataFull == 1'b0)
      begin
        NextState_initSDSt <= `CLK_SEQ_CHK_FIN;
        next_txDataOut <= 8'hff;
        next_txDataWen <= 1'b1;
        next_loopCnt <= loopCnt + 1'b1;
      end
    end
    `CLK_SEQ_CHK_FIN:
    begin
      next_txDataWen <= 1'b0;
      if (loopCnt == `SD_INIT_START_SEQ_LEN)
      begin
        NextState_initSDSt <= `CLK_SEQ_WT_DATA_EMPTY;
      end
      else
      begin
        NextState_initSDSt <= `CLK_SEQ_SEND_FF;
      end
    end
    `CLK_SEQ_WT_DATA_EMPTY:
    begin
      if (txDataEmpty == 1'b1)
      begin
        NextState_initSDSt <= `RESET_SEND_CMD;
        next_loopCnt <= 8'h00;
      end
    end
    `RESET_SEND_CMD:
    begin
      next_cmdByte <= 8'h40;
      //CMD0
      next_dataByte1 <= 8'h00;
      next_dataByte2 <= 8'h00;
      next_dataByte3 <= 8'h00;
      next_dataByte4 <= 8'h00;
      next_checkSumByte <= 8'h95;
      next_sendCmdReq <= 1'b1;
      next_loopCnt <= loopCnt + 1'b1;
      next_spiCS_n <= 1'b0;
      NextState_initSDSt <= `RESET_DEL;
    end
    `RESET_DEL:
    begin
      next_sendCmdReq <= 1'b0;
      NextState_initSDSt <= `RESET_WT_FIN;
    end
    `RESET_WT_FIN:
    begin
      if (sendCmdRdy == 1'b1)
      begin
        NextState_initSDSt <= `RESET_CHK_FIN;
        next_spiCS_n <= 1'b1;
      end
    end
    `RESET_CHK_FIN:
    begin
      if ((respTout == 1'b1 || respByte != 8'h01) && loopCnt != 8'hff)
      begin
        NextState_initSDSt <= `RESET_SEND_CMD;
      end
      else if (respTout == 1'b1 || respByte != 8'h01)
      begin
        NextState_initSDSt <= `WT_INIT_REQ;
        next_initError <= `INIT_CMD0_ERROR;
      end
      else
      begin
        NextState_initSDSt <= `INIT_SEND_CMD;
      end
    end
    `INIT_WT_FIN:
    begin
      if (sendCmdRdy == 1'b1)
      begin
        NextState_initSDSt <= `INIT_CHK_FIN;
        next_spiCS_n <= 1'b1;
      end
    end
    `INIT_CHK_FIN:
    begin
      if ((respTout == 1'b1 || respByte != 8'h00) && loopCnt != 8'hff)
      begin
        NextState_initSDSt <= `INIT_SEND_CMD;
      end
      else if (respTout == 1'b1 || respByte != 8'h00)
      begin
        NextState_initSDSt <= `WT_INIT_REQ;
        next_initError <= `INIT_CMD1_ERROR;
      end
      else
      begin
        NextState_initSDSt <= `WT_INIT_REQ;
      end
    end
    `INIT_SEND_CMD:
    begin
      next_cmdByte <= 8'h41;
      //CMD1
      next_dataByte1 <= 8'h00;
      next_dataByte2 <= 8'h00;
      next_dataByte3 <= 8'h00;
      next_dataByte4 <= 8'h00;
      next_checkSumByte <= 8'hff;
      next_sendCmdReq <= 1'b1;
      next_loopCnt <= loopCnt + 1'b1;
      next_spiCS_n <= 1'b0;
      next_delCnt1 <= 10'h000;
      NextState_initSDSt <= `INIT_DEL1;
    end
    `INIT_DEL1:
    begin
      next_delCnt1 <= delCnt1 + 1'b1;
      next_delCnt2 <= 8'h00;
      next_sendCmdReq <= 1'b0;
      if (delCnt1 == `TWO_MS)
      begin
        NextState_initSDSt <= `INIT_WT_FIN;
      end
      else
      begin
        NextState_initSDSt <= `INIT_DEL2;
      end
    end
    `INIT_DEL2:
    begin
      next_delCnt2 <= delCnt2 + 1'b1;
      if (delCnt2 == 8'hff)
      begin
        NextState_initSDSt <= `INIT_DEL1;
      end
    end
  endcase
end

// Current State Logic (sequential)
always @ (posedge clk)
begin
  if (rst == 1'b1)
    CurrState_initSDSt <= `START;
  else
    CurrState_initSDSt <= NextState_initSDSt;
end

// Registered outputs logic
always @ (posedge clk)
begin
  if (rst == 1'b1)
  begin
    spiClkDelayOut <= spiClkDelayIn;
    SDInitRdy <= 1'b0;
    spiCS_n <= 1'b1;
    initError <= `INIT_NO_ERROR;
    txDataOut <= 8'h00;
    txDataWen <= 1'b0;
    cmdByte <= 8'h00;
    dataByte1 <= 8'h00;
    dataByte2 <= 8'h00;
    dataByte3 <= 8'h00;
    dataByte4 <= 8'h00;
    checkSumByte <= 8'h00;
    sendCmdReq <= 1'b0;
    rxDataRdyClr <= 1'b0;
    loopCnt <= 8'h00;
    delCnt1 <= 10'h000;
    delCnt2 <= 8'h00;
  end
  else 
  begin
    spiClkDelayOut <= next_spiClkDelayOut;
    SDInitRdy <= next_SDInitRdy;
    spiCS_n <= next_spiCS_n;
    initError <= next_initError;
    txDataOut <= next_txDataOut;
    txDataWen <= next_txDataWen;
    cmdByte <= next_cmdByte;
    dataByte1 <= next_dataByte1;
    dataByte2 <= next_dataByte2;
    dataByte3 <= next_dataByte3;
    dataByte4 <= next_dataByte4;
    checkSumByte <= next_checkSumByte;
    sendCmdReq <= next_sendCmdReq;
    rxDataRdyClr <= next_rxDataRdyClr;
    loopCnt <= next_loopCnt;
    delCnt1 <= next_delCnt1;
    delCnt2 <= next_delCnt2;
  end
end

endmodule