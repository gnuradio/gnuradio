
//////////////////////////////////////////////////////////////////////
////                                                              ////
//// readWriteSDBlock.v                             ////
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

module readWriteSDBlock (blockAddr, checkSumByte, clk, cmdByte, dataByte1, dataByte2, dataByte3, dataByte4, readError, readWriteSDBlockRdy, readWriteSDBlockReq, respByte, respTout, rst, rxDataIn, rxDataRdy, rxDataRdyClr, rxFifoData, rxFifoWen, sendCmdRdy, sendCmdReq, spiCS_n, txDataEmpty, txDataFull, txDataOut, txDataWen, txFifoData, txFifoRen, writeError);
input   [31:0]blockAddr;
input   clk;
input   [1:0]readWriteSDBlockReq;
input   [7:0]respByte;
input   respTout;
input   rst;
input   [7:0]rxDataIn;
input   rxDataRdy;
input   sendCmdRdy;
input   txDataEmpty;
input   txDataFull;
input   [7:0]txFifoData;
output  [7:0]checkSumByte;
output  [7:0]cmdByte;
output  [7:0]dataByte1;
output  [7:0]dataByte2;
output  [7:0]dataByte3;
output  [7:0]dataByte4;
output  [1:0]readError;
output  readWriteSDBlockRdy;
output  rxDataRdyClr;
output  [7:0]rxFifoData;
output  rxFifoWen;
output  sendCmdReq;
output  spiCS_n;
output  [7:0]txDataOut;
output  txDataWen;
output  txFifoRen;
output  [1:0]writeError;

wire    [31:0]blockAddr;
reg     [7:0]checkSumByte, next_checkSumByte;
wire    clk;
reg     [7:0]cmdByte, next_cmdByte;
reg     [7:0]dataByte1, next_dataByte1;
reg     [7:0]dataByte2, next_dataByte2;
reg     [7:0]dataByte3, next_dataByte3;
reg     [7:0]dataByte4, next_dataByte4;
reg     [1:0]readError, next_readError;
reg     readWriteSDBlockRdy, next_readWriteSDBlockRdy;
wire    [1:0]readWriteSDBlockReq;
wire    [7:0]respByte;
wire    respTout;
wire    rst;
wire    [7:0]rxDataIn;
wire    rxDataRdy;
reg     rxDataRdyClr, next_rxDataRdyClr;
reg     [7:0]rxFifoData, next_rxFifoData;
reg     rxFifoWen, next_rxFifoWen;
wire    sendCmdRdy;
reg     sendCmdReq, next_sendCmdReq;
reg     spiCS_n, next_spiCS_n;
wire    txDataEmpty;
wire    txDataFull;
reg     [7:0]txDataOut, next_txDataOut;
reg     txDataWen, next_txDataWen;
wire    [7:0]txFifoData;
reg     txFifoRen, next_txFifoRen;
reg     [1:0]writeError, next_writeError;

// diagram signals declarations
reg  [7:0]delCnt1, next_delCnt1;
reg  [7:0]delCnt2, next_delCnt2;
reg  [7:0]locRespByte, next_locRespByte;
reg  [8:0]loopCnt, next_loopCnt;
reg  [11:0]timeOutCnt, next_timeOutCnt;

// BINARY ENCODED state machine: rwBlkSt
// State codes definitions:
`define ST_RW_SD 6'b000000
`define WR_CMD_SEND_CMD 6'b000001
`define WR_CMD_WT_FIN 6'b000010
`define WR_CMD_DEL 6'b000011
`define WT_REQ 6'b000100
`define WR_TOKEN_FF1_FIN 6'b000101
`define WR_TOKEN_FF1_ST 6'b000110
`define WR_TOKEN_FF2_FIN 6'b000111
`define WR_TOKEN_FF2_ST 6'b001000
`define WR_TOKEN_FE_FIN 6'b001001
`define WR_TOKEN_FE_ST 6'b001010
`define WR_DATA_D_FIN 6'b001011
`define WR_DATA_D_ST 6'b001100
`define WR_DATA_RD_FIFO1 6'b001101
`define WR_DATA_RD_FIFO2 6'b001110
`define WR_DATA_LOOP_INIT 6'b001111
`define WR_DATA_CS_ST1 6'b010000
`define WR_DATA_CS_FIN1 6'b010001
`define WR_DATA_CS_FIN2 6'b010010
`define WR_DATA_CS_ST2 6'b010011
`define WR_DATA_CHK_RESP 6'b010100
`define WR_DATA_REQ_RESP_ST 6'b010101
`define WR_DATA_REQ_RESP_FIN 6'b010110
`define RD_CMD_SEND_CMD 6'b010111
`define RD_CMD_WT_FIN 6'b011000
`define RD_CMD_DEL 6'b011001
`define RD_TOKEN_CHK_LOOP 6'b011010
`define RD_TOKEN_WT_FIN 6'b011011
`define RD_TOKEN_SEND_CMD 6'b011100
`define RD_TOKEN_DEL2 6'b011101
`define RD_TOKEN_INIT_LOOP 6'b011110
`define RD_TOKEN_DEL1 6'b011111
`define RD_DATA_ST_LOOP 6'b100000
`define RD_DATA_WT_DATA 6'b100001
`define RD_DATA_CHK_LOOP 6'b100010
`define RD_DATA_CLR_RX 6'b100011
`define RD_DATA_CS_FIN2 6'b100100
`define RD_DATA_CS_FIN1 6'b100101
`define RD_DATA_CS_ST1 6'b100110
`define RD_DATA_CS_ST2 6'b100111
`define WR_BUSY_CHK_FIN 6'b101000
`define WR_BUSY_WT_FIN1 6'b101001
`define WR_BUSY_DEL1 6'b101010
`define WR_BUSY_SEND_CMD1 6'b101011
`define WR_BUSY_DEL2 6'b101100
`define WR_BUSY_INIT_LOOP 6'b101101
`define RD_TOKEN_DEL3 6'b101110
`define WR_DATA_DEL 6'b101111

reg [5:0]CurrState_rwBlkSt, NextState_rwBlkSt;

// Diagram actions (continuous assignments allowed only: assign ...)
// diagram ACTION


// Machine: rwBlkSt

// NextState logic (combinatorial)
always @ (blockAddr or sendCmdRdy or respTout or respByte or readWriteSDBlockReq or txDataFull or loopCnt or txFifoData or txDataEmpty or timeOutCnt or locRespByte or rxDataRdy or rxDataIn or delCnt1 or delCnt2 or readWriteSDBlockRdy or spiCS_n or readError or writeError or txDataOut or txDataWen or rxDataRdyClr or cmdByte or dataByte1 or dataByte2 or dataByte3 or dataByte4 or checkSumByte or sendCmdReq or txFifoRen or rxFifoWen or rxFifoData or CurrState_rwBlkSt)
begin
  NextState_rwBlkSt <= CurrState_rwBlkSt;
  // Set default values for outputs and signals
  next_readWriteSDBlockRdy <= readWriteSDBlockRdy;
  next_spiCS_n <= spiCS_n;
  next_readError <= readError;
  next_writeError <= writeError;
  next_txDataOut <= txDataOut;
  next_txDataWen <= txDataWen;
  next_rxDataRdyClr <= rxDataRdyClr;
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
  next_txFifoRen <= txFifoRen;
  next_rxFifoWen <= rxFifoWen;
  next_rxFifoData <= rxFifoData;
  next_timeOutCnt <= timeOutCnt;
  next_locRespByte <= locRespByte;
  case (CurrState_rwBlkSt)  // synopsys parallel_case full_case
    `ST_RW_SD:
    begin
      next_readWriteSDBlockRdy <= 1'b0;
      next_spiCS_n <= 1'b1;
      next_readError <= 1'b0;
      next_writeError <= 1'b0;
      next_txDataOut <= 8'h00;
      next_txDataWen <= 1'b0;
      next_rxDataRdyClr <= 1'b0;
      next_cmdByte <= 8'h00;
      next_dataByte1 <= 8'h00;
      next_dataByte2 <= 8'h00;
      next_dataByte3 <= 8'h00;
      next_dataByte4 <= 8'h00;
      next_checkSumByte <= 8'h00;
      next_sendCmdReq <= 1'b0;
      next_loopCnt <= 8'h00;
      next_delCnt1 <= 8'h00;
      next_delCnt2 <= 8'h00;
      next_readError <= `READ_NO_ERROR;
      next_writeError <= `WRITE_NO_ERROR;
      next_txFifoRen <= 1'b0;
      next_rxFifoWen <= 1'b0;
      next_rxFifoData <= 8'h00;
      next_timeOutCnt <= 12'h000;
      next_locRespByte <= 8'h00;
      NextState_rwBlkSt <= `WT_REQ;
    end
    `WT_REQ:
    begin
      next_spiCS_n <= 1'b1;
      next_readWriteSDBlockRdy <= 1'b1;
      next_cmdByte <= 8'h00;
      next_dataByte1 <= 8'h00;
      next_dataByte2 <= 8'h00;
      next_dataByte3 <= 8'h00;
      next_dataByte4 <= 8'h00;
      next_checkSumByte <= 8'h00;
      if (readWriteSDBlockReq == `READ_SD_BLOCK)
      begin
        NextState_rwBlkSt <= `RD_CMD_SEND_CMD;
        next_spiCS_n <= 1'b0;
        next_readWriteSDBlockRdy <= 1'b0;
        next_readError <= `READ_NO_ERROR;
      end
      else if (readWriteSDBlockReq == `WRITE_SD_BLOCK)
      begin
        NextState_rwBlkSt <= `WR_CMD_SEND_CMD;
        next_spiCS_n <= 1'b0;
        next_readWriteSDBlockRdy <= 1'b0;
        next_writeError <= `WRITE_NO_ERROR;
      end
    end
    `WR_CMD_SEND_CMD:
    begin
      next_cmdByte <= 8'h58;
      //CMD24 Block Write
      next_dataByte1 <= blockAddr[31:24];
      next_dataByte2 <= blockAddr[23:16];
      next_dataByte3 <= blockAddr[15:8];
      next_dataByte4 <= blockAddr[7:0];
      next_checkSumByte <= 8'hff;
      next_sendCmdReq <= 1'b1;
      NextState_rwBlkSt <= `WR_CMD_DEL;
    end
    `WR_CMD_WT_FIN:
    begin
      if ((sendCmdRdy == 1'b1) && (respTout == 1'b1 || respByte != 8'h00))
      begin
        NextState_rwBlkSt <= `WT_REQ;
        next_writeError <= `WRITE_CMD_ERROR;
      end
      else if (sendCmdRdy == 1'b1)
      begin
        NextState_rwBlkSt <= `WR_TOKEN_FF1_ST;
      end
    end
    `WR_CMD_DEL:
    begin
      next_sendCmdReq <= 1'b0;
      NextState_rwBlkSt <= `WR_CMD_WT_FIN;
    end
    `WR_TOKEN_FF1_FIN:
    begin
      next_txDataWen <= 1'b0;
      NextState_rwBlkSt <= `WR_TOKEN_FF2_ST;
    end
    `WR_TOKEN_FF1_ST:
    begin
      if (txDataFull == 1'b0)
      begin
        NextState_rwBlkSt <= `WR_TOKEN_FF1_FIN;
        next_txDataOut <= 8'hff;
        next_txDataWen <= 1'b1;
      end
    end
    `WR_TOKEN_FF2_FIN:
    begin
      next_txDataWen <= 1'b0;
      NextState_rwBlkSt <= `WR_TOKEN_FE_ST;
    end
    `WR_TOKEN_FF2_ST:
    begin
      if (txDataFull == 1'b0)
      begin
        NextState_rwBlkSt <= `WR_TOKEN_FF2_FIN;
        next_txDataOut <= 8'hff;
        next_txDataWen <= 1'b1;
      end
    end
    `WR_TOKEN_FE_FIN:
    begin
      next_txDataWen <= 1'b0;
      NextState_rwBlkSt <= `WR_DATA_LOOP_INIT;
    end
    `WR_TOKEN_FE_ST:
    begin
      if (txDataFull == 1'b0)
      begin
        NextState_rwBlkSt <= `WR_TOKEN_FE_FIN;
        next_txDataOut <= 8'hfe;
        next_txDataWen <= 1'b1;
      end
    end
    `WR_BUSY_CHK_FIN:
    begin
      if (locRespByte == 8'h00 && timeOutCnt != `TWO_FIFTY_MS)
      begin
        NextState_rwBlkSt <= `WR_BUSY_SEND_CMD1;
        next_timeOutCnt <= timeOutCnt + 1'b1;
      end
      else if (timeOutCnt == `TWO_FIFTY_MS)
      begin
        NextState_rwBlkSt <= `WT_REQ;
        next_writeError <= `WRITE_BUSY_ERROR;
      end
      else
      begin
        NextState_rwBlkSt <= `WT_REQ;
      end
    end
    `WR_BUSY_WT_FIN1:
    begin
      if (rxDataRdy == 1'b1)
      begin
        NextState_rwBlkSt <= `WR_BUSY_CHK_FIN;
        next_locRespByte <= rxDataIn;
      end
    end
    `WR_BUSY_DEL1:
    begin
      next_txDataWen <= 1'b0;
      next_rxDataRdyClr <= 1'b0;
      next_delCnt1 <= delCnt1 + 1'b1;
      next_delCnt2 <= 8'h00;
      if (delCnt1 == `MAX_8_BIT)
      begin
        NextState_rwBlkSt <= `WR_BUSY_WT_FIN1;
      end
      else
      begin
        NextState_rwBlkSt <= `WR_BUSY_DEL2;
      end
    end
    `WR_BUSY_SEND_CMD1:
    begin
      next_txDataOut <= 8'hff;
      next_txDataWen <= 1'b1;
      next_rxDataRdyClr <= 1'b1;
      next_delCnt1 <= 8'h00;
      NextState_rwBlkSt <= `WR_BUSY_DEL1;
    end
    `WR_BUSY_DEL2:
    begin
      next_delCnt2 <= delCnt2 + 1'b1;
      if (delCnt2 == 8'hff)
      begin
        NextState_rwBlkSt <= `WR_BUSY_DEL1;
      end
    end
    `WR_BUSY_INIT_LOOP:
    begin
      next_timeOutCnt <= 12'h000;
      NextState_rwBlkSt <= `WR_BUSY_SEND_CMD1;
    end
    `RD_CMD_SEND_CMD:
    begin
      next_cmdByte <= 8'h51;
      //CMD17 Block Read
      next_dataByte1 <= blockAddr[31:24];
      next_dataByte2 <= blockAddr[23:16];
      next_dataByte3 <= blockAddr[15:8];
      next_dataByte4 <= blockAddr[7:0];
      next_checkSumByte <= 8'hff;
      next_sendCmdReq <= 1'b1;
      NextState_rwBlkSt <= `RD_CMD_DEL;
    end
    `RD_CMD_WT_FIN:
    begin
      if ((sendCmdRdy == 1'b1) && (respTout == 1'b1 || respByte != 8'h00))
      begin
        NextState_rwBlkSt <= `WT_REQ;
        next_readError <= `READ_CMD_ERROR;
      end
      else if (sendCmdRdy == 1'b1)
      begin
        NextState_rwBlkSt <= `RD_TOKEN_INIT_LOOP;
      end
    end
    `RD_CMD_DEL:
    begin
      next_sendCmdReq <= 1'b0;
      NextState_rwBlkSt <= `RD_CMD_WT_FIN;
    end
    `RD_TOKEN_CHK_LOOP:
    begin
      if (locRespByte != 8'hfe && timeOutCnt != `ONE_HUNDRED_MS)
      begin
        NextState_rwBlkSt <= `RD_TOKEN_DEL2;
        next_timeOutCnt <= timeOutCnt + 1'b1;
        next_delCnt1 <= 8'h00;
      end
      else if (timeOutCnt == `ONE_HUNDRED_MS)
      begin
        NextState_rwBlkSt <= `WT_REQ;
        next_readError <= `READ_TOKEN_ERROR;
      end
      else
      begin
        NextState_rwBlkSt <= `RD_DATA_CLR_RX;
        next_rxDataRdyClr <= 1'b1;
      end
    end
    `RD_TOKEN_WT_FIN:
    begin
      if (rxDataRdy == 1'b1)
      begin
        NextState_rwBlkSt <= `RD_TOKEN_CHK_LOOP;
        next_locRespByte <= rxDataIn;
      end
    end
    `RD_TOKEN_SEND_CMD:
    begin
      next_txDataOut <= 8'hff;
      next_txDataWen <= 1'b1;
      next_rxDataRdyClr <= 1'b1;
      NextState_rwBlkSt <= `RD_TOKEN_DEL1;
    end
    `RD_TOKEN_DEL2:
    begin
      next_delCnt1 <= delCnt1 + 1'b1;
      next_delCnt2 <= 8'h00;
      if (delCnt1 == `MAX_8_BIT)
      begin
        NextState_rwBlkSt <= `RD_TOKEN_SEND_CMD;
      end
      else
      begin
        NextState_rwBlkSt <= `RD_TOKEN_DEL3;
      end
    end
    `RD_TOKEN_INIT_LOOP:
    begin
      next_timeOutCnt <= 12'h000;
      NextState_rwBlkSt <= `RD_TOKEN_SEND_CMD;
    end
    `RD_TOKEN_DEL1:
    begin
      next_txDataWen <= 1'b0;
      next_rxDataRdyClr <= 1'b0;
      NextState_rwBlkSt <= `RD_TOKEN_WT_FIN;
    end
    `RD_TOKEN_DEL3:
    begin
      next_delCnt2 <= delCnt2 + 1'b1;
      if (delCnt2 == 8'hff)
      begin
        NextState_rwBlkSt <= `RD_TOKEN_DEL2;
      end
    end
    `RD_DATA_ST_LOOP:
    begin
      next_txDataWen <= 1'b1;
      next_txDataOut <= 8'hff;
      next_loopCnt <= loopCnt + 1'b1;
      NextState_rwBlkSt <= `RD_DATA_WT_DATA;
    end
    `RD_DATA_WT_DATA:
    begin
      next_txDataWen <= 1'b0;
      if (rxDataRdy == 1'b1)
      begin
        NextState_rwBlkSt <= `RD_DATA_CHK_LOOP;
        next_rxFifoWen <= 1'b1;
        next_rxDataRdyClr <= 1'b1;
        next_rxFifoData <= rxDataIn;
      end
    end
    `RD_DATA_CHK_LOOP:
    begin
      if (loopCnt == 9'b000000000)
      begin
        NextState_rwBlkSt <= `RD_DATA_CS_ST1;
        next_rxDataRdyClr <= 1'b0;
        next_rxFifoWen <= 1'b0;
      end
      else
      begin
        NextState_rwBlkSt <= `RD_DATA_ST_LOOP;
        next_rxDataRdyClr <= 1'b0;
        next_rxFifoWen <= 1'b0;
      end
    end
    `RD_DATA_CLR_RX:
    begin
      NextState_rwBlkSt <= `RD_DATA_ST_LOOP;
      next_rxDataRdyClr <= 1'b0;
      next_loopCnt <= 9'b000000000;
    end
    `RD_DATA_CS_FIN2:
    begin
      next_txDataWen <= 1'b0;
      if (txDataEmpty == 1'b1)
      begin
        NextState_rwBlkSt <= `WT_REQ;
      end
    end
    `RD_DATA_CS_FIN1:
    begin
      next_txDataWen <= 1'b0;
      NextState_rwBlkSt <= `RD_DATA_CS_ST2;
    end
    `RD_DATA_CS_ST1:
    begin
      if (txDataFull == 1'b0)
      begin
        NextState_rwBlkSt <= `RD_DATA_CS_FIN1;
        next_txDataOut <= 8'hff;
        next_txDataWen <= 1'b1;
      end
    end
    `RD_DATA_CS_ST2:
    begin
      if (txDataFull == 1'b0)
      begin
        NextState_rwBlkSt <= `RD_DATA_CS_FIN2;
        next_txDataOut <= 8'hff;
        next_txDataWen <= 1'b1;
      end
    end
    `WR_DATA_D_FIN:
    begin
      next_txDataWen <= 1'b0;
      if (loopCnt == 9'b000000000)
      begin
        NextState_rwBlkSt <= `WR_DATA_CS_ST1;
      end
      else
      begin
        NextState_rwBlkSt <= `WR_DATA_RD_FIFO1;
      end
    end
    `WR_DATA_D_ST:
    begin
      if (txDataFull == 1'b0)
      begin
        NextState_rwBlkSt <= `WR_DATA_D_FIN;
        next_txDataOut <= txFifoData;
        next_txDataWen <= 1'b1;
      end
    end
    `WR_DATA_RD_FIFO1:
    begin
      next_txFifoRen <= 1'b1;
      next_loopCnt <= loopCnt + 1'b1;
      NextState_rwBlkSt <= `WR_DATA_RD_FIFO2;
    end
    `WR_DATA_RD_FIFO2:
    begin
      next_txFifoRen <= 1'b0;
      NextState_rwBlkSt <= `WR_DATA_D_ST;
    end
    `WR_DATA_LOOP_INIT:
    begin
      next_loopCnt <= 9'b000000000;
      NextState_rwBlkSt <= `WR_DATA_RD_FIFO1;
    end
    `WR_DATA_CS_ST1:
    begin
      if (txDataFull == 1'b0)
      begin
        NextState_rwBlkSt <= `WR_DATA_CS_FIN1;
        next_txDataOut <= 8'hff;
        next_txDataWen <= 1'b1;
      end
    end
    `WR_DATA_CS_FIN1:
    begin
      next_txDataWen <= 1'b0;
      NextState_rwBlkSt <= `WR_DATA_CS_ST2;
    end
    `WR_DATA_CS_FIN2:
    begin
      next_txDataWen <= 1'b0;
      next_timeOutCnt <= 12'h000;
      if (txDataEmpty == 1'b1)
      begin
        NextState_rwBlkSt <= `WR_DATA_REQ_RESP_ST;
      end
    end
    `WR_DATA_CS_ST2:
    begin
      if (txDataFull == 1'b0)
      begin
        NextState_rwBlkSt <= `WR_DATA_CS_FIN2;
        next_txDataOut <= 8'hff;
        next_txDataWen <= 1'b1;
      end
    end
    `WR_DATA_CHK_RESP:
    begin
      if (timeOutCnt == `WR_RESP_TOUT)
      begin
        NextState_rwBlkSt <= `WT_REQ;
        next_writeError <= `WRITE_DATA_ERROR;
      end
      else if (locRespByte[4:0] == 5'h5)
      begin
        NextState_rwBlkSt <= `WR_BUSY_INIT_LOOP;
      end
      else
      begin
        NextState_rwBlkSt <= `WR_DATA_REQ_RESP_ST;
      end
    end
    `WR_DATA_REQ_RESP_ST:
    begin
      NextState_rwBlkSt <= `WR_DATA_DEL;
      next_txDataOut <= 8'hff;
      next_txDataWen <= 1'b1;
      next_timeOutCnt <= timeOutCnt + 1'b1;
      next_rxDataRdyClr <= 1'b1;
    end
    `WR_DATA_REQ_RESP_FIN:
    begin
      if (rxDataRdy == 1'b1)
      begin
        NextState_rwBlkSt <= `WR_DATA_CHK_RESP;
        next_locRespByte <= rxDataIn;
      end
    end
    `WR_DATA_DEL:
    begin
      NextState_rwBlkSt <= `WR_DATA_REQ_RESP_FIN;
      next_txDataWen <= 1'b0;
      next_rxDataRdyClr <= 1'b0;
    end
  endcase
end

// Current State Logic (sequential)
always @ (posedge clk)
begin
  if (rst == 1'b1)
    CurrState_rwBlkSt <= `ST_RW_SD;
  else
    CurrState_rwBlkSt <= NextState_rwBlkSt;
end

// Registered outputs logic
always @ (posedge clk)
begin
  if (rst == 1'b1)
  begin
    readWriteSDBlockRdy <= 1'b0;
    spiCS_n <= 1'b1;
    readError <= 1'b0;
    writeError <= 1'b0;
    txDataOut <= 8'h00;
    txDataWen <= 1'b0;
    rxDataRdyClr <= 1'b0;
    cmdByte <= 8'h00;
    dataByte1 <= 8'h00;
    dataByte2 <= 8'h00;
    dataByte3 <= 8'h00;
    dataByte4 <= 8'h00;
    checkSumByte <= 8'h00;
    sendCmdReq <= 1'b0;
    txFifoRen <= 1'b0;
    rxFifoWen <= 1'b0;
    rxFifoData <= 8'h00;
    loopCnt <= 8'h00;
    delCnt1 <= 8'h00;
    delCnt2 <= 8'h00;
    timeOutCnt <= 12'h000;
    locRespByte <= 8'h00;
  end
  else 
  begin
    readWriteSDBlockRdy <= next_readWriteSDBlockRdy;
    spiCS_n <= next_spiCS_n;
    readError <= next_readError;
    writeError <= next_writeError;
    txDataOut <= next_txDataOut;
    txDataWen <= next_txDataWen;
    rxDataRdyClr <= next_rxDataRdyClr;
    cmdByte <= next_cmdByte;
    dataByte1 <= next_dataByte1;
    dataByte2 <= next_dataByte2;
    dataByte3 <= next_dataByte3;
    dataByte4 <= next_dataByte4;
    checkSumByte <= next_checkSumByte;
    sendCmdReq <= next_sendCmdReq;
    txFifoRen <= next_txFifoRen;
    rxFifoWen <= next_rxFifoWen;
    rxFifoData <= next_rxFifoData;
    loopCnt <= next_loopCnt;
    delCnt1 <= next_delCnt1;
    delCnt2 <= next_delCnt2;
    timeOutCnt <= next_timeOutCnt;
    locRespByte <= next_locRespByte;
  end
end

endmodule