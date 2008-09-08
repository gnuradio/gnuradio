
//////////////////////////////////////////////////////////////////////
////                                                              ////
//// sendCmd.v                                           ////
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


module sendCmd (checkSumByte_1, checkSumByte_2, clk, cmdByte_1, cmdByte_2, dataByte1_1, dataByte1_2, dataByte2_1, dataByte2_2, dataByte3_1, dataByte3_2, dataByte4_1, dataByte4_2, respByte, respTout, rst, rxDataIn, rxDataRdy, rxDataRdyClr, sendCmdRdy, sendCmdReq1, sendCmdReq2, txDataEmpty, txDataFull, txDataOut, txDataWen);
input   [7:0]checkSumByte_1;
input   [7:0]checkSumByte_2;
input   clk;
input   [7:0]cmdByte_1;
input   [7:0]cmdByte_2;
input   [7:0]dataByte1_1;
input   [7:0]dataByte1_2;
input   [7:0]dataByte2_1;
input   [7:0]dataByte2_2;
input   [7:0]dataByte3_1;
input   [7:0]dataByte3_2;
input   [7:0]dataByte4_1;
input   [7:0]dataByte4_2;
input   rst;
input   [7:0]rxDataIn;
input   rxDataRdy;
input   sendCmdReq1;
input   sendCmdReq2;
input   txDataEmpty;
input   txDataFull;
output  [7:0]respByte;
output  respTout;
output  rxDataRdyClr;
output  sendCmdRdy;
output  [7:0]txDataOut;
output  txDataWen;

wire    [7:0]checkSumByte_1;
wire    [7:0]checkSumByte_2;
wire    clk;
wire    [7:0]cmdByte_1;
wire    [7:0]cmdByte_2;
wire    [7:0]dataByte1_1;
wire    [7:0]dataByte1_2;
wire    [7:0]dataByte2_1;
wire    [7:0]dataByte2_2;
wire    [7:0]dataByte3_1;
wire    [7:0]dataByte3_2;
wire    [7:0]dataByte4_1;
wire    [7:0]dataByte4_2;
reg     [7:0]respByte, next_respByte;
reg     respTout, next_respTout;
wire    rst;
wire    [7:0]rxDataIn;
wire    rxDataRdy;
reg     rxDataRdyClr, next_rxDataRdyClr;
reg     sendCmdRdy, next_sendCmdRdy;
wire    sendCmdReq1;
wire    sendCmdReq2;
wire    txDataEmpty;
wire    txDataFull;
reg     [7:0]txDataOut, next_txDataOut;
reg     txDataWen, next_txDataWen;

// diagram signals declarations
reg  [7:0]checkSumByte, next_checkSumByte;
reg  [7:0]cmdByte, next_cmdByte;
reg  [7:0]dataByte1, next_dataByte1;
reg  [7:0]dataByte2, next_dataByte2;
reg  [7:0]dataByte3, next_dataByte3;
reg  [7:0]dataByte4, next_dataByte4;
reg sendCmdReq, next_sendCmdReq;
reg  [9:0]timeOutCnt, next_timeOutCnt;

// BINARY ENCODED state machine: sndCmdSt
// State codes definitions:
`define CMD_D_BYTE2_FIN 5'b00000
`define CMD_D_BYTE2_ST 5'b00001
`define CMD_SEND_FF_FIN 5'b00010
`define CMD_CMD_BYTE_FIN 5'b00011
`define CMD_D_BYTE1_FIN 5'b00100
`define CMD_REQ_RESP_ST 5'b00101
`define CMD_REQ_RESP_FIN 5'b00110
`define CMD_CHK_RESP 5'b00111
`define CMD_D_BYTE1_ST 5'b01000
`define CMD_D_BYTE3_FIN 5'b01001
`define CMD_D_BYTE3_ST 5'b01010
`define CMD_D_BYTE4_FIN 5'b01011
`define CMD_D_BYTE4_ST 5'b01100
`define CMD_CS_FIN 5'b01101
`define CMD_CS_ST 5'b01110
`define CMD_SEND_FF_ST 5'b01111
`define CMD_CMD_BYTE_ST 5'b10000
`define WT_CMD 5'b10001
`define ST_S_CMD 5'b10010
`define CMD_DEL 5'b10011

reg [4:0]CurrState_sndCmdSt, NextState_sndCmdSt;

// Diagram actions (continuous assignments allowed only: assign ...)
// diagram ACTION
always @(sendCmdReq1 or sendCmdReq2 ) begin
sendCmdReq <= sendCmdReq1 | sendCmdReq2;
end
always @(posedge clk) begin
cmdByte <= cmdByte_1 | cmdByte_2;
dataByte1 <= dataByte1_1 | dataByte1_2;
dataByte2 <= dataByte2_1 | dataByte2_2;
dataByte3 <= dataByte3_1 | dataByte3_2;
dataByte4 <= dataByte4_1 | dataByte4_2;
checkSumByte <= checkSumByte_1 | checkSumByte_2;
end


// Machine: sndCmdSt

// NextState logic (combinatorial)
always @ (txDataFull or dataByte2 or timeOutCnt or rxDataRdy or rxDataIn or respByte or dataByte1 or dataByte3 or dataByte4 or txDataEmpty or checkSumByte or cmdByte or sendCmdReq or txDataWen or txDataOut or rxDataRdyClr or respTout or sendCmdRdy or CurrState_sndCmdSt)
begin
  NextState_sndCmdSt <= CurrState_sndCmdSt;
  // Set default values for outputs and signals
  next_txDataWen <= txDataWen;
  next_txDataOut <= txDataOut;
  next_timeOutCnt <= timeOutCnt;
  next_rxDataRdyClr <= rxDataRdyClr;
  next_respByte <= respByte;
  next_respTout <= respTout;
  next_sendCmdRdy <= sendCmdRdy;
  case (CurrState_sndCmdSt)  // synopsys parallel_case full_case
    `WT_CMD:
    begin
      next_sendCmdRdy <= 1'b1;
      if (sendCmdReq == 1'b1)
      begin
        NextState_sndCmdSt <= `CMD_SEND_FF_ST;
        next_sendCmdRdy <= 1'b0;
        next_respTout <= 1'b0;
      end
    end
    `ST_S_CMD:
    begin
      next_sendCmdRdy <= 1'b0;
      next_txDataWen <= 1'b0;
      next_txDataOut <= 8'h00;
      next_rxDataRdyClr <= 1'b0;
      next_respByte <= 8'h00;
      next_respTout <= 1'b0;
      next_timeOutCnt <= 10'h000;
      NextState_sndCmdSt <= `WT_CMD;
    end
    `CMD_D_BYTE2_FIN:
    begin
      next_txDataWen <= 1'b0;
      NextState_sndCmdSt <= `CMD_D_BYTE3_ST;
    end
    `CMD_D_BYTE2_ST:
    begin
      if (txDataFull == 1'b0)
      begin
        NextState_sndCmdSt <= `CMD_D_BYTE2_FIN;
        next_txDataOut <= dataByte2;
        next_txDataWen <= 1'b1;
      end
    end
    `CMD_SEND_FF_FIN:
    begin
      NextState_sndCmdSt <= `CMD_CMD_BYTE_ST;
    end
    `CMD_CMD_BYTE_FIN:
    begin
      next_txDataWen <= 1'b0;
      NextState_sndCmdSt <= `CMD_D_BYTE1_ST;
    end
    `CMD_D_BYTE1_FIN:
    begin
      next_txDataWen <= 1'b0;
      NextState_sndCmdSt <= `CMD_D_BYTE2_ST;
    end
    `CMD_REQ_RESP_ST:
    begin
      NextState_sndCmdSt <= `CMD_DEL;
      next_txDataOut <= 8'hff;
      next_txDataWen <= 1'b1;
      next_timeOutCnt <= timeOutCnt + 1'b1;
      next_rxDataRdyClr <= 1'b1;
    end
    `CMD_REQ_RESP_FIN:
    begin
      if (rxDataRdy == 1'b1)
      begin
        NextState_sndCmdSt <= `CMD_CHK_RESP;
        next_respByte <= rxDataIn;
      end
    end
    `CMD_CHK_RESP:
    begin
      if (timeOutCnt == 10'h200)
      begin
        NextState_sndCmdSt <= `WT_CMD;
        next_respTout <= 1'b1;
      end
      else if (respByte[7] == 1'b0)
      begin
        NextState_sndCmdSt <= `WT_CMD;
      end
      else
      begin
        NextState_sndCmdSt <= `CMD_REQ_RESP_ST;
      end
    end
    `CMD_D_BYTE1_ST:
    begin
      if (txDataFull == 1'b0)
      begin
        NextState_sndCmdSt <= `CMD_D_BYTE1_FIN;
        next_txDataOut <= dataByte1;
        next_txDataWen <= 1'b1;
      end
    end
    `CMD_D_BYTE3_FIN:
    begin
      next_txDataWen <= 1'b0;
      NextState_sndCmdSt <= `CMD_D_BYTE4_ST;
    end
    `CMD_D_BYTE3_ST:
    begin
      if (txDataFull == 1'b0)
      begin
        NextState_sndCmdSt <= `CMD_D_BYTE3_FIN;
        next_txDataOut <= dataByte3;
        next_txDataWen <= 1'b1;
      end
    end
    `CMD_D_BYTE4_FIN:
    begin
      next_txDataWen <= 1'b0;
      NextState_sndCmdSt <= `CMD_CS_ST;
    end
    `CMD_D_BYTE4_ST:
    begin
      if (txDataFull == 1'b0)
      begin
        NextState_sndCmdSt <= `CMD_D_BYTE4_FIN;
        next_txDataOut <= dataByte4;
        next_txDataWen <= 1'b1;
      end
    end
    `CMD_CS_FIN:
    begin
      next_txDataWen <= 1'b0;
      next_timeOutCnt <= 10'h000;
      if (txDataEmpty == 1'b1)
      begin
        NextState_sndCmdSt <= `CMD_REQ_RESP_ST;
      end
    end
    `CMD_CS_ST:
    begin
      if (txDataFull == 1'b0)
      begin
        NextState_sndCmdSt <= `CMD_CS_FIN;
        next_txDataOut <= checkSumByte;
        next_txDataWen <= 1'b1;
      end
    end
    `CMD_SEND_FF_ST:
    begin
      if (txDataFull == 1'b0)
      begin
        NextState_sndCmdSt <= `CMD_SEND_FF_FIN;
        next_txDataOut <= 8'hff;
        next_txDataWen <= 1'b1;
      end
    end
    `CMD_CMD_BYTE_ST:
    begin
      next_txDataWen <= 1'b0;
      if (txDataFull == 1'b0)
      begin
        NextState_sndCmdSt <= `CMD_CMD_BYTE_FIN;
        next_txDataOut <= cmdByte;
        next_txDataWen <= 1'b1;
      end
    end
    `CMD_DEL:
    begin
      NextState_sndCmdSt <= `CMD_REQ_RESP_FIN;
      next_txDataWen <= 1'b0;
      next_rxDataRdyClr <= 1'b0;
    end
  endcase
end

// Current State Logic (sequential)
always @ (posedge clk)
begin
  if (rst == 1'b1)
    CurrState_sndCmdSt <= `ST_S_CMD;
  else
    CurrState_sndCmdSt <= NextState_sndCmdSt;
end

// Registered outputs logic
always @ (posedge clk)
begin
  if (rst == 1'b1)
  begin
    txDataWen <= 1'b0;
    txDataOut <= 8'h00;
    rxDataRdyClr <= 1'b0;
    respByte <= 8'h00;
    respTout <= 1'b0;
    sendCmdRdy <= 1'b0;
    timeOutCnt <= 10'h000;
  end
  else 
  begin
    txDataWen <= next_txDataWen;
    txDataOut <= next_txDataOut;
    rxDataRdyClr <= next_rxDataRdyClr;
    respByte <= next_respByte;
    respTout <= next_respTout;
    sendCmdRdy <= next_sendCmdRdy;
    timeOutCnt <= next_timeOutCnt;
  end
end

endmodule