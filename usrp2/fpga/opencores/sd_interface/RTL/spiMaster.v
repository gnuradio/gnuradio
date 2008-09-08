//////////////////////////////////////////////////////////////////////
////                                                              ////
//// spiMaster.v                                                    ////
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
//// Copyright (C) 2008 Steve Fielding and OPENCORES.ORG          ////
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

module spiMaster(
  clk_i,
  rst_i,
  address_i,
  data_i,
  data_o,
  strobe_i,
  we_i,
  ack_o,

  // SPI logic clock
  spiSysClk,

  //SPI bus
  spiClkOut,
  spiDataIn,
  spiDataOut,
  spiCS_n
);

//Wishbone bus
input clk_i;
input rst_i;
input [7:0] address_i;
input [7:0] data_i;
output [7:0] data_o;
input strobe_i;
input we_i;
output ack_o;

// SPI logic clock
input spiSysClk;

//SPI bus
output spiClkOut;
input spiDataIn;
output spiDataOut;
output spiCS_n;

// local wires and regs
wire spiSysClk;
wire [7:0] spiClkDelayFromInitSD;
wire rstSyncToSpiClk;
wire [7:0] rxDataFromRWSPIWireData;
wire rxDataRdySetFromRWSPIWireData;
wire txDataFullFromSpiTxRxData;
wire txDataFullClrFromRWSPIWireData;
wire [7:0] txDataToRWSPIWireData;
wire rxDataRdyClrFromRWSDBlock;
wire rxDataRdyClrFromSendCmd;
wire [7:0] rxDataFromSpiTxRxData;
wire rxDataRdy;
wire [7:0] txDataFromRWSDBlock;
wire txDataWenFromRWSDBlock;
wire [7:0] txDataFromSendCmd;
wire txDataWenFromSendCmd;
wire [7:0] txDataFromInitSD;
wire txDataWenFromInitSD;
wire [7:0] dataFromCtrlStsReg;
wire [7:0] dataFromTxFifo;
wire [7:0] dataFromRxFifo;
wire [1:0] spiTransType;
wire [7:0] spiDirectAccessTxData;
wire [1:0] readWriteSDBlockReq;
wire [1:0] SDWriteError;
wire [1:0] SDReadError;
wire [1:0] SDInitError;
wire [7:0] cmdByteFromInitSD;
wire [7:0] dataByte1FromInitSD;
wire [7:0] dataByte2FromInitSD;
wire [7:0] dataByte3FromInitSD;
wire [7:0] dataByte4FromInitSD;
wire [7:0] checkSumByteFromInitSD;
wire [7:0] sendCmdRespByte;
wire [7:0] cmdByteFromRWSDBlock;
wire [7:0] dataByte1FromRWSDBlock;
wire [7:0] dataByte2FromRWSDBlock;
wire [7:0] dataByte3FromRWSDBlock;
wire [7:0] dataByte4FromRWSDBlock;
wire [7:0] checkSumByteFromRWSDBlock;
wire [7:0] txFifoDataOut;
wire [7:0] rxFifoDataIn;
wire [31:0] SDAddr;
wire [7:0] spiClkDelayFromCtrlStsReg;
wire spiCS_nFromInitSD;
wire spiCS_nFromRWSDBlock;
wire spiCS_nFromSpiCtrl;


assign spiCS_n = spiCS_nFromInitSD & spiCS_nFromRWSDBlock & spiCS_nFromSpiCtrl;

// -----------------------------------
// Instance of Module: wishBoneBI
// -----------------------------------
wishBoneBI u_wishBoneBI(
  .ack_o(               ack_o                 ),
  .address(             address_i             ),
  .clk(                 clk_i                 ),
  .ctrlStsRegSel(       ctrlStsRegSel         ),
  .dataFromCtrlStsReg(  dataFromCtrlStsReg    ),
  .dataFromRxFifo(      dataFromRxFifo        ),
  .dataFromTxFifo(      dataFromTxFifo        ),
  .dataIn(              data_i                ),
  .dataOut(             data_o                ),
  .rst(                 rst_i                 ),
  .rxFifoSel(           rxFifoSel             ),
  .strobe_i(            strobe_i              ),
  .txFifoSel(           txFifoSel             ),
  .writeEn(             we_i                  )
	);

// -----------------------------------
// Instance of Module: ctrlStsRegBI
// -----------------------------------
ctrlStsRegBI u_ctrlStsRegBI(
  .busClk(              clk_i                 ),
  .spiSysClk(           spiSysClk             ),
  .rstSyncToBusClkOut(  rstSyncToBusClk       ),
  .rstSyncToSpiClkOut(  rstSyncToSpiClk       ),
  .rstFromWire(         rst_i                 ),
  .address(             address_i             ),
  .strobe_i(            strobe_i              ),
  .dataIn(              data_i                ),
  .dataOut(             dataFromCtrlStsReg    ),
  .ctrlStsRegSel(       ctrlStsRegSel         ),
  .spiTransType(        spiTransType          ),
  .spiTransCtrl(        spiTransCtrl          ), 
  .spiTransStatus(      spiTransSts           ),
  .spiDirectAccessTxData(spiDirectAccessTxData),
  .spiDirectAccessRxData(rxDataFromSpiTxRxData), 
  .writeEn(             we_i                  ),
  .SDWriteError(        SDWriteError          ),
  .SDReadError(         SDReadError           ),
  .SDInitError(         SDInitError           ),
  .SDAddr(              SDAddr                ),
  .spiClkDelay(         spiClkDelayFromCtrlStsReg)
	);

// -----------------------------------
// Instance of Module: spiCtrl
// -----------------------------------
spiCtrl u_spiCtrl(
  .clk(                 spiSysClk             ),
  .rst(                 rstSyncToSpiClk       ),
  .SDInitReq(           SDInitReq             ),
  .SDInitRdy(           SDInitRdy             ),
  .readWriteSDBlockReq( readWriteSDBlockReq   ),
  .readWriteSDBlockRdy( readWriteSDBlockRdy   ),
  .rxDataRdy(           rxDataRdyFromSpiTxRxData),
  .rxDataRdyClr(        rxDataRdyClrFromSpiCtrl),
  .spiTransType(        spiTransType          ),
  .spiTransCtrl(        spiTransCtrl          ),
  .spiTransSts(         spiTransSts           ),
  .txDataWen(           txDataWenFromSpiCtrl  ),
  .spiCS_n(             spiCS_nFromSpiCtrl    )
	);


// -----------------------------------
// Instance of Module: initSD
// -----------------------------------
initSD u_initSD(
  .clk(                 spiSysClk             ),
  .rst(                 rstSyncToSpiClk       ),
  .SDInitReq(           SDInitReq             ),
  .SDInitRdy(           SDInitRdy             ),
  .initError(           SDInitError           ),
  .sendCmdReq(          sendCmdReqFromInitSD  ),
  .sendCmdRdy(          sendCmdRdy            ),
  .cmdByte(             cmdByteFromInitSD     ),
  .dataByte1(           dataByte1FromInitSD   ),
  .dataByte2(           dataByte2FromInitSD   ),
  .dataByte3(           dataByte3FromInitSD   ),
  .dataByte4(           dataByte4FromInitSD   ),
  .checkSumByte(        checkSumByteFromInitSD),
  .respByte(            sendCmdRespByte       ),
  .respTout(            sendCmdRespTout       ),
  .spiCS_n(             spiCS_nFromInitSD    ),
  .spiClkDelayOut(      spiClkDelayFromInitSD ),
  .spiClkDelayIn(       spiClkDelayFromCtrlStsReg),
  .txDataFull(          txDataFullFromSpiTxRxData),
  .txDataEmpty(         txDataEmptyFromRWSPIWireData),
  .txDataOut(           txDataFromInitSD      ),
  .txDataWen(           txDataWenFromInitSD   ),
  .rxDataRdy(           rxDataRdyFromSpiTxRxData),
  .rxDataRdyClr(        rxDataRdyClrFromInitSD)
	);

// -----------------------------------
// Instance of Module: readWriteSDBlock
// -----------------------------------
readWriteSDBlock u_readWriteSDBlock(
  .clk(                 spiSysClk             ),
  .rst(                 rstSyncToSpiClk       ),
  .readWriteSDBlockReq( readWriteSDBlockReq   ),
  .readWriteSDBlockRdy( readWriteSDBlockRdy   ),
  .cmdByte(             cmdByteFromRWSDBlock  ),
  .dataByte1(           dataByte1FromRWSDBlock),
  .dataByte2(           dataByte2FromRWSDBlock),
  .dataByte3(           dataByte3FromRWSDBlock),
  .dataByte4(           dataByte4FromRWSDBlock),
  .checkSumByte(        checkSumByteFromRWSDBlock),
  .readError(           SDReadError             ),
  .respByte(            sendCmdRespByte       ),
  .respTout(            sendCmdRespTout       ),
  .rxDataIn(            rxDataFromSpiTxRxData ),
  .rxDataRdy(           rxDataRdyFromSpiTxRxData),
  .rxDataRdyClr(        rxDataRdyClrFromRWSDBlock),
  .sendCmdRdy(          sendCmdRdy            ),
  .sendCmdReq(          sendCmdReqFromRWSDBlock),
  .spiCS_n(             spiCS_nFromRWSDBlock ),
  .txDataFull(          txDataFullFromSpiTxRxData),
  .txDataEmpty(         txDataEmptyFromRWSPIWireData),
  .txDataOut(           txDataFromRWSDBlock   ),
  .txDataWen(           txDataWenFromRWSDBlock),
  .txFifoData(          txFifoDataOut         ),
  .txFifoRen(           txFifoRE              ),
  .rxFifoData(          rxFifoDataIn          ),
  .rxFifoWen(           rRxFifoWE             ),
  .writeError(          SDWriteError          ),
  .blockAddr(           SDAddr                )

	);

// -----------------------------------
// Instance of Module: sendCmd
// -----------------------------------
sendCmd u_sendCmd(
  .clk(                 spiSysClk             ),
  .rst(                 rstSyncToSpiClk       ),
  .sendCmdReq1(         sendCmdReqFromInitSD  ),
  .sendCmdReq2(         sendCmdReqFromRWSDBlock),
  .sendCmdRdy(          sendCmdRdy            ),
  .cmdByte_1(           cmdByteFromInitSD     ),
  .cmdByte_2(           cmdByteFromRWSDBlock  ),
  .dataByte1_1(         dataByte1FromInitSD   ),
  .dataByte1_2(         dataByte1FromRWSDBlock),
  .dataByte2_1(         dataByte2FromInitSD   ),
  .dataByte2_2(         dataByte2FromRWSDBlock),
  .dataByte3_1(         dataByte3FromInitSD   ),
  .dataByte3_2(         dataByte3FromRWSDBlock),
  .dataByte4_1(         dataByte4FromInitSD   ),
  .dataByte4_2(         dataByte4FromRWSDBlock),
  .checkSumByte_1(      checkSumByteFromInitSD),
  .checkSumByte_2(      checkSumByteFromRWSDBlock),
  .respByte(            sendCmdRespByte       ),
  .respTout(            sendCmdRespTout       ),
  .rxDataIn(            rxDataFromSpiTxRxData ),
  .rxDataRdy(           rxDataRdyFromSpiTxRxData),
  .rxDataRdyClr(        rxDataRdyClrFromSendCmd),
  .txDataFull(          txDataFullFromSpiTxRxData),
  .txDataEmpty(         txDataEmptyFromRWSPIWireData),
  .txDataOut(           txDataFromSendCmd     ),
  .txDataWen(           txDataWenFromSendCmd  )
	);

// -----------------------------------
// Instance of Module: spiTxRxData
// -----------------------------------
spiTxRxData u_spiTxRxData(
  .clk(                 spiSysClk             ),
  .rst(                 rstSyncToSpiClk       ),
  .rx1DataRdyClr(       rxDataRdyClrFromRWSDBlock),
  .rx2DataRdyClr(       rxDataRdyClrFromSendCmd),
  .rx3DataRdyClr(       rxDataRdyClrFromInitSD),
  .rx4DataRdyClr(       rxDataRdyClrFromSpiCtrl),
  .rxDataIn(            rxDataFromRWSPIWireData),
  .rxDataOut(           rxDataFromSpiTxRxData ),
  .rxDataRdy(           rxDataRdyFromSpiTxRxData),
  .rxDataRdySet(        rxDataRdySetFromRWSPIWireData),
  .tx1DataIn(           txDataFromRWSDBlock   ),
  .tx1DataWEn(          txDataWenFromRWSDBlock),
  .tx2DataIn(           txDataFromSendCmd     ),
  .tx2DataWEn(          txDataWenFromSendCmd  ),
  .tx3DataIn(           txDataFromInitSD      ),
  .tx3DataWEn(          txDataWenFromInitSD   ),
  .tx4DataIn(           spiDirectAccessTxData ),
  .tx4DataWEn(          txDataWenFromSpiCtrl  ),
  .txDataFull(          txDataFullFromSpiTxRxData),
  .txDataFullClr(       txDataFullClrFromRWSPIWireData),
  .txDataOut(           txDataToRWSPIWireData )
	);

// -----------------------------------
// Instance of Module: readWriteSPIWireData
// -----------------------------------
readWriteSPIWireData u_readWriteSPIWireData(
  .clk(                 spiSysClk             ),
  .clkDelay(            spiClkDelayFromInitSD           ),
  .rst(                 rstSyncToSpiClk       ),
  .rxDataOut(           rxDataFromRWSPIWireData),
  .rxDataRdySet(        rxDataRdySetFromRWSPIWireData),
  .spiClkOut(           spiClkOut             ),
  .spiDataIn(           spiDataIn             ),
  .spiDataOut(          spiDataOut            ),
  .txDataFull(          txDataFullFromSpiTxRxData),
  .txDataFullClr(       txDataFullClrFromRWSPIWireData),
  .txDataIn(            txDataToRWSPIWireData ),
  .txDataEmpty(         txDataEmptyFromRWSPIWireData)
	);

TxFifo #(`TX_FIFO_DEPTH, `TX_FIFO_ADDR_WIDTH) u_txFifo (
  .spiSysClk(spiSysClk), 
  .busClk(clk_i), 
  .rstSyncToBusClk(rstSyncToBusClk), 
  .rstSyncToSpiClk(rstSyncToSpiClk), 
  .fifoREn(txFifoRE), 
  .fifoEmpty(hostTxFifoEmpty),
  .busAddress(address_i[2:0]), 
  .busWriteEn(we_i), 
  .busStrobe_i(strobe_i),
  .busFifoSelect(txFifoSel),
  .busDataIn(data_i), 
  .busDataOut(dataFromTxFifo),
  .fifoDataOut(txFifoDataOut) );


RxFifo #(`RX_FIFO_DEPTH, `RX_FIFO_ADDR_WIDTH) u_rxFifo(
  .spiSysClk(spiSysClk), 
  .busClk(clk_i),
  .rstSyncToBusClk(rstSyncToBusClk), 
  .rstSyncToSpiClk(rstSyncToSpiClk), 
  .fifoWEn(rRxFifoWE), 
  .fifoFull(hostRxFifoFull),
  .busAddress(address_i[2:0]), 
  .busWriteEn(we_i), 
  .busStrobe_i(strobe_i),
  .busFifoSelect(rxFifoSel),
  .busDataIn(data_i), 
  .busDataOut(dataFromRxFifo),
  .fifoDataIn(rxFifoDataIn)  );

endmodule

