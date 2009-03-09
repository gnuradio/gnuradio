/*
 * $RDCfile: $ $Revision: 1.1.2.15 $ $Date: 2007/07/25 15:58:33 $
 *******************************************************************************
 *
 * FIFO Generator v3.3 - Verilog Behavioral Model
 *
 *******************************************************************************
 *
 * Copyright(C) 2006 by Xilinx, Inc. All rights reserved.
 * This text/file contains proprietary, confidential
 * information of Xilinx, Inc., is distributed under
 * license from Xilinx, Inc., and may be used, copied
 * and/or disclosed only pursuant to the terms of a valid
 * license agreement with Xilinx, Inc. Xilinx hereby
 * grants you a license to use this text/file solely for
 * design, simulation, implementation and creation of
 * design files limited to Xilinx devices or technologies.
 * Use with non-Xilinx devices or technologies is expressly
 * prohibited and immediately terminates your license unless
 * covered by a separate agreement.
 *
 * Xilinx is providing theis design, code, or information
 * "as-is" solely for use in developing programs and
 * solutions for Xilinx devices, with no obligation on the
 * part of Xilinx to provide support. By providing this design,
 * code, or information as one possible implementation of
 * this feature, application or standard. Xilinx is making no
 * representation that this implementation is free from any
 * claims of infringement. You are responsible for obtaining
 * any rights you may require for your implementation.
 * Xilinx expressly disclaims any warranty whatsoever with
 * respect to the adequacy of the implementation, including
 * but not limited to any warranties or representations that this
 * implementation is free from claims of infringement, implied
 * warranties of merchantability or fitness for a particular
 * purpose.
 *
 * Xilinx products are not intended for use in life support
 * appliances, devices, or systems. Use in such applications is
 * expressly prohibited.
 *
 * This copyright and support notice must be retained as part
 * of this text at all times. (c)Copyright 1995-2006 Xilinx, Inc.
 * All rights reserved.
 *
 *******************************************************************************
 *
 * Filename: fifo_generator_v4_3_bhv.v
 *
 * Description:
 *  The verilog behavioral model for the FIFO generator core.
 *
 *******************************************************************************
 */

`timescale 1ps/1ps

/*******************************************************************************
 * Declaration of top-level module
 ******************************************************************************/
module FIFO_GENERATOR_V4_3
(
  BACKUP,
  BACKUP_MARKER,
  CLK,
  DIN,
  PROG_EMPTY_THRESH,
  PROG_EMPTY_THRESH_ASSERT,
  PROG_EMPTY_THRESH_NEGATE,
  PROG_FULL_THRESH,
  PROG_FULL_THRESH_ASSERT,
  PROG_FULL_THRESH_NEGATE,
  RD_CLK,
  RD_EN,
  RD_RST,
  RST,
  SRST,
  WR_CLK,
  WR_EN,
  WR_RST,
  INT_CLK,

  ALMOST_EMPTY,
  ALMOST_FULL,
  DATA_COUNT,
  DOUT,
  EMPTY,
  FULL,
  OVERFLOW,
  PROG_EMPTY,
  PROG_FULL,
  RD_DATA_COUNT,
  UNDERFLOW,
  VALID,
  WR_ACK,
  WR_DATA_COUNT,
  SBITERR,
  DBITERR
  );

  /****************************************************************************
  * Definition of Ports
  *
  *
  *****************************************************************************
  * Definition of Parameters
  *
  *
  *****************************************************************************/

  /****************************************************************************
  * Declare user parameters and their defaults
  *****************************************************************************/
  parameter  C_COMMON_CLOCK                 = 0;
  parameter  C_COUNT_TYPE                   = 0;
  parameter  C_DATA_COUNT_WIDTH             = 2;
  parameter  C_DEFAULT_VALUE                = "";
  parameter  C_DIN_WIDTH                    = 8;
  parameter  C_DOUT_RST_VAL                 = "";
  parameter  C_DOUT_WIDTH                   = 8;
  parameter  C_ENABLE_RLOCS                 = 0;

  parameter  C_FAMILY                       = "virtex2"; 
  //Not allowed in Verilog model
  
  parameter  C_HAS_ALMOST_EMPTY             = 0;
  parameter  C_HAS_ALMOST_FULL              = 0;
  parameter  C_HAS_BACKUP                   = 0;
  parameter  C_HAS_DATA_COUNT               = 0;
  parameter  C_HAS_MEMINIT_FILE             = 0;
  parameter  C_HAS_OVERFLOW                 = 0;
  parameter  C_HAS_RD_DATA_COUNT            = 0;
  parameter  C_HAS_RD_RST                   = 0;
  parameter  C_HAS_RST                      = 0;
  parameter  C_HAS_SRST                     = 0;
  parameter  C_HAS_UNDERFLOW                = 0;
  parameter  C_HAS_VALID                    = 0;
  parameter  C_HAS_WR_ACK                   = 0;
  parameter  C_HAS_WR_DATA_COUNT            = 0;
  parameter  C_HAS_WR_RST                   = 0;
  parameter  C_IMPLEMENTATION_TYPE          = 0;
  parameter  C_INIT_WR_PNTR_VAL             = 0;
  parameter  C_MEMORY_TYPE                  = 1;
  parameter  C_MIF_FILE_NAME                = "";
  parameter  C_OPTIMIZATION_MODE            = 0;
  parameter  C_OVERFLOW_LOW                 = 0;
  parameter  C_PRELOAD_LATENCY              = 1;
  parameter  C_PRELOAD_REGS                 = 0;
  parameter  C_PRIM_FIFO_TYPE               = 512;
  parameter  C_PROG_EMPTY_THRESH_ASSERT_VAL = 0;
  parameter  C_PROG_EMPTY_THRESH_NEGATE_VAL = 0;
  parameter  C_PROG_EMPTY_TYPE              = 0;
  parameter  C_PROG_FULL_THRESH_ASSERT_VAL  = 0;
  parameter  C_PROG_FULL_THRESH_NEGATE_VAL  = 0;
  parameter  C_PROG_FULL_TYPE               = 0;
  parameter  C_RD_DATA_COUNT_WIDTH          = 2;
  parameter  C_RD_DEPTH                     = 256;
  parameter  C_RD_FREQ                      = 1;
  parameter  C_RD_PNTR_WIDTH                = 8;
  parameter  C_UNDERFLOW_LOW                = 0;
  parameter  C_USE_FIFO16_FLAGS             = 0;
  parameter  C_VALID_LOW                    = 0;
  parameter  C_WR_ACK_LOW                   = 0;
  parameter  C_WR_DATA_COUNT_WIDTH          = 2;
  parameter  C_WR_DEPTH                     = 256;
  parameter  C_WR_FREQ                      = 1;
  parameter  C_WR_PNTR_WIDTH                = 8;
  parameter  C_WR_RESPONSE_LATENCY          = 1;
  parameter  C_USE_ECC                      = 0;
  parameter  C_FULL_FLAGS_RST_VAL           = 1;
  parameter  C_HAS_INT_CLK                  = 0;
  parameter  C_USE_EMBEDDED_REG             = 0;
  parameter  C_USE_FWFT_DATA_COUNT             = 0;

  //There are 2 Verilog behavioral models
  // 0 = Synchronous FIFO/ShiftRam FIFO
  // 1 = Asynchronous FIFO
  parameter  C_VERILOG_IMPL = (C_IMPLEMENTATION_TYPE==0 ? 0 :
                               (C_IMPLEMENTATION_TYPE==1 ? 0 :
                                (C_IMPLEMENTATION_TYPE==2 ? 1 : 0)));


  /******************************************************************************
   * Declare Input and Output Ports
   *****************************************************************************/
  input                              CLK;
  input                              BACKUP;
  input                              BACKUP_MARKER;
  input [C_DIN_WIDTH-1:0]            DIN;
  input [C_RD_PNTR_WIDTH-1:0]        PROG_EMPTY_THRESH;
  input [C_RD_PNTR_WIDTH-1:0]        PROG_EMPTY_THRESH_ASSERT;
  input [C_RD_PNTR_WIDTH-1:0]        PROG_EMPTY_THRESH_NEGATE;
  input [C_WR_PNTR_WIDTH-1:0]        PROG_FULL_THRESH;
  input [C_WR_PNTR_WIDTH-1:0]        PROG_FULL_THRESH_ASSERT;
  input [C_WR_PNTR_WIDTH-1:0]        PROG_FULL_THRESH_NEGATE;
  input                              RD_CLK;
  input                              RD_EN;
  input                              RD_RST;
  input                              RST;
  input                              SRST;
  input                              WR_CLK;
  input                              WR_EN;
  input                              WR_RST;
  input                              INT_CLK;

  output                             ALMOST_EMPTY;
  output                             ALMOST_FULL;
  output [C_DATA_COUNT_WIDTH-1:0]    DATA_COUNT;
  output [C_DOUT_WIDTH-1:0]          DOUT;
  output                             EMPTY;
  output                             FULL;
  output                             OVERFLOW;
  output                             PROG_EMPTY;
  output                             PROG_FULL;
  output                             VALID;
  output [C_RD_DATA_COUNT_WIDTH-1:0] RD_DATA_COUNT;
  output                             UNDERFLOW;
  output                             WR_ACK;
  output [C_WR_DATA_COUNT_WIDTH-1:0] WR_DATA_COUNT;
  output                             SBITERR;
  output                             DBITERR;


  wire                               ALMOST_EMPTY;
  wire                               ALMOST_FULL;
  wire [C_DATA_COUNT_WIDTH-1:0]      DATA_COUNT;
  wire [C_DOUT_WIDTH-1:0]            DOUT;
  wire                               EMPTY;
  wire                               FULL;
  wire                               OVERFLOW;
  wire                               PROG_EMPTY;
  wire                               PROG_FULL;
  wire                               VALID;
  wire [C_RD_DATA_COUNT_WIDTH-1:0]   RD_DATA_COUNT;
  wire                               UNDERFLOW;
  wire                               WR_ACK;
  wire [C_WR_DATA_COUNT_WIDTH-1:0]   WR_DATA_COUNT;


  wire                               RD_CLK_P0_IN;
  wire                               RST_P0_IN;
  wire                               RD_EN_FIFO_IN;
  wire                               RD_EN_P0_IN;

  wire                               ALMOST_EMPTY_FIFO_OUT;
  wire                               ALMOST_FULL_FIFO_OUT;
  wire [C_DATA_COUNT_WIDTH-1:0]      DATA_COUNT_FIFO_OUT;
  wire [C_DOUT_WIDTH-1:0]            DOUT_FIFO_OUT;
  wire                               EMPTY_FIFO_OUT;
  wire                               FULL_FIFO_OUT;
  wire                               OVERFLOW_FIFO_OUT;
  wire                               PROG_EMPTY_FIFO_OUT;
  wire                               PROG_FULL_FIFO_OUT;
  wire                               VALID_FIFO_OUT;
  wire [C_RD_DATA_COUNT_WIDTH-1:0]   RD_DATA_COUNT_FIFO_OUT;
  wire                               UNDERFLOW_FIFO_OUT;
  wire                               WR_ACK_FIFO_OUT;
  wire [C_WR_DATA_COUNT_WIDTH-1:0]   WR_DATA_COUNT_FIFO_OUT;


  //***************************************************************************
  // Internal Signals
  //   The core uses either the internal_ wires or the preload0_ wires depending
  //     on whether the core uses Preload0 or not.
  //   When using preload0, the internal signals connect the internal core to
  //     the preload logic, and the external core's interfaces are tied to the
  //     preload0 signals from the preload logic.
  //***************************************************************************
  wire [C_DOUT_WIDTH-1:0]            DATA_P0_OUT;
  wire                               VALID_P0_OUT;
  wire                               EMPTY_P0_OUT;
  wire                               ALMOSTEMPTY_P0_OUT;
  reg                                EMPTY_P0_OUT_Q;
  reg                                ALMOSTEMPTY_P0_OUT_Q;
  wire                               UNDERFLOW_P0_OUT;
  wire                               RDEN_P0_OUT;
  wire [C_DOUT_WIDTH-1:0]            DATA_P0_IN;
  wire                               EMPTY_P0_IN;

  assign SBITERR = 1'b0;
  assign DBITERR = 1'b0;

// choose the base FIFO implementation for simulation
generate
case (C_VERILOG_IMPL)
0 : begin : block1
  fifo_generator_v4_3_bhv_ver_ss
  #(
    C_COMMON_CLOCK,
    C_COUNT_TYPE,
    C_DATA_COUNT_WIDTH,
    C_DEFAULT_VALUE,
    C_DIN_WIDTH,
    C_DOUT_RST_VAL,
    C_DOUT_WIDTH,
    C_ENABLE_RLOCS,
    C_FAMILY,//Not allowed in Verilog model
    C_HAS_ALMOST_EMPTY,
    C_HAS_ALMOST_FULL,
    C_HAS_BACKUP,
    C_HAS_DATA_COUNT,
    C_HAS_MEMINIT_FILE,
    C_HAS_OVERFLOW,
    C_HAS_RD_DATA_COUNT,
    C_HAS_RD_RST,
    C_HAS_RST,
    C_HAS_SRST,
    C_HAS_UNDERFLOW,
    C_HAS_VALID,
    C_HAS_WR_ACK,
    C_HAS_WR_DATA_COUNT,
    C_HAS_WR_RST,
    C_IMPLEMENTATION_TYPE,
    C_INIT_WR_PNTR_VAL,
    C_MEMORY_TYPE,
    C_MIF_FILE_NAME,
    C_OPTIMIZATION_MODE,
    C_OVERFLOW_LOW,
    C_PRELOAD_LATENCY,
    C_PRELOAD_REGS,
    C_PROG_EMPTY_THRESH_ASSERT_VAL,
    C_PROG_EMPTY_THRESH_NEGATE_VAL,
    C_PROG_EMPTY_TYPE,
    C_PROG_FULL_THRESH_ASSERT_VAL,
    C_PROG_FULL_THRESH_NEGATE_VAL,
    C_PROG_FULL_TYPE,
    C_RD_DATA_COUNT_WIDTH,
    C_RD_DEPTH,
    C_RD_PNTR_WIDTH,
    C_UNDERFLOW_LOW,
    C_VALID_LOW,
    C_WR_ACK_LOW,
    C_WR_DATA_COUNT_WIDTH,
    C_WR_DEPTH,
    C_WR_PNTR_WIDTH,
    C_WR_RESPONSE_LATENCY,
    C_FULL_FLAGS_RST_VAL,
    C_USE_EMBEDDED_REG
  )
  gen_ss
  (
    .CLK                      (CLK),
    .RST                      (RST),
    .SRST                     (SRST),
    .DIN                      (DIN),
    .WR_EN                    (WR_EN),
    .RD_EN                    (RD_EN_FIFO_IN),
    .PROG_EMPTY_THRESH        (PROG_EMPTY_THRESH),
    .PROG_EMPTY_THRESH_ASSERT (PROG_EMPTY_THRESH_ASSERT),
    .PROG_EMPTY_THRESH_NEGATE (PROG_EMPTY_THRESH_NEGATE),
    .PROG_FULL_THRESH         (PROG_FULL_THRESH),
    .PROG_FULL_THRESH_ASSERT  (PROG_FULL_THRESH_ASSERT),
    .PROG_FULL_THRESH_NEGATE  (PROG_FULL_THRESH_NEGATE),
    .DOUT                     (DOUT_FIFO_OUT),
    .FULL                     (FULL_FIFO_OUT),
    .ALMOST_FULL              (ALMOST_FULL_FIFO_OUT),
    .WR_ACK                   (WR_ACK_FIFO_OUT),
    .OVERFLOW                 (OVERFLOW_FIFO_OUT),
    .EMPTY                    (EMPTY_FIFO_OUT),
    .ALMOST_EMPTY             (ALMOST_EMPTY_FIFO_OUT),
    .VALID                    (VALID_FIFO_OUT),
    .UNDERFLOW                (UNDERFLOW_FIFO_OUT),
    .DATA_COUNT               (DATA_COUNT_FIFO_OUT),
    .PROG_FULL                (PROG_FULL_FIFO_OUT),
    .PROG_EMPTY               (PROG_EMPTY_FIFO_OUT)
   );
end
1 : begin : block1
  fifo_generator_v4_3_bhv_ver_as
  #(
    C_COMMON_CLOCK,
    C_COUNT_TYPE,
    C_DATA_COUNT_WIDTH,
    C_DEFAULT_VALUE,
    C_DIN_WIDTH,
    C_DOUT_RST_VAL,
    C_DOUT_WIDTH,
    C_ENABLE_RLOCS,
    C_FAMILY,//Not allowed in Verilog model
    C_HAS_ALMOST_EMPTY,
    C_HAS_ALMOST_FULL,
    C_HAS_BACKUP,
    C_HAS_DATA_COUNT,
    C_HAS_MEMINIT_FILE,
    C_HAS_OVERFLOW,
    C_HAS_RD_DATA_COUNT,
    C_HAS_RD_RST,
    C_HAS_RST,
    C_HAS_UNDERFLOW,
    C_HAS_VALID,
    C_HAS_WR_ACK,
    C_HAS_WR_DATA_COUNT,
    C_HAS_WR_RST,
    C_IMPLEMENTATION_TYPE,
    C_INIT_WR_PNTR_VAL,
    C_MEMORY_TYPE,
    C_MIF_FILE_NAME,
    C_OPTIMIZATION_MODE,
    C_OVERFLOW_LOW,
    C_PRELOAD_LATENCY,
    C_PRELOAD_REGS,
    C_PROG_EMPTY_THRESH_ASSERT_VAL,
    C_PROG_EMPTY_THRESH_NEGATE_VAL,
    C_PROG_EMPTY_TYPE,
    C_PROG_FULL_THRESH_ASSERT_VAL,
    C_PROG_FULL_THRESH_NEGATE_VAL,
    C_PROG_FULL_TYPE,
    C_RD_DATA_COUNT_WIDTH,
    C_RD_DEPTH,
    C_RD_PNTR_WIDTH,
    C_UNDERFLOW_LOW,
    C_VALID_LOW,
    C_WR_ACK_LOW,
    C_WR_DATA_COUNT_WIDTH,
    C_WR_DEPTH,
    C_WR_PNTR_WIDTH,
    C_WR_RESPONSE_LATENCY,
    C_FULL_FLAGS_RST_VAL,
    C_USE_FWFT_DATA_COUNT,
    C_USE_EMBEDDED_REG
  )
  gen_as
  (
    .WR_CLK                   (WR_CLK),
    .RD_CLK                   (RD_CLK),
    .RST                      (RST),
    .DIN                      (DIN),
    .WR_EN                    (WR_EN),
    .RD_EN                    (RD_EN_FIFO_IN),
    .PROG_EMPTY_THRESH        (PROG_EMPTY_THRESH),
    .PROG_EMPTY_THRESH_ASSERT (PROG_EMPTY_THRESH_ASSERT),
    .PROG_EMPTY_THRESH_NEGATE (PROG_EMPTY_THRESH_NEGATE),
    .PROG_FULL_THRESH         (PROG_FULL_THRESH),
    .PROG_FULL_THRESH_ASSERT  (PROG_FULL_THRESH_ASSERT),
    .PROG_FULL_THRESH_NEGATE  (PROG_FULL_THRESH_NEGATE),
    .DOUT                     (DOUT_FIFO_OUT),
    .FULL                     (FULL_FIFO_OUT),
    .ALMOST_FULL              (ALMOST_FULL_FIFO_OUT),
    .WR_ACK                   (WR_ACK_FIFO_OUT),
    .OVERFLOW                 (OVERFLOW_FIFO_OUT),
    .EMPTY                    (EMPTY_FIFO_OUT),
    .ALMOST_EMPTY             (ALMOST_EMPTY_FIFO_OUT),
    .VALID                    (VALID_FIFO_OUT),
    .UNDERFLOW                (UNDERFLOW_FIFO_OUT),
    .RD_DATA_COUNT            (RD_DATA_COUNT_FIFO_OUT),
    .WR_DATA_COUNT            (WR_DATA_COUNT_FIFO_OUT),
    .PROG_FULL                (PROG_FULL_FIFO_OUT),
    .PROG_EMPTY               (PROG_EMPTY_FIFO_OUT)
   );
end

default : begin : block1
  fifo_generator_v4_3_bhv_ver_as
  #(
    C_COMMON_CLOCK,
    C_COUNT_TYPE,
    C_DATA_COUNT_WIDTH,
    C_DEFAULT_VALUE,
    C_DIN_WIDTH,
    C_DOUT_RST_VAL,
    C_DOUT_WIDTH,
    C_ENABLE_RLOCS,
    C_FAMILY,//Not allowed in Verilog model
    C_HAS_ALMOST_EMPTY,
    C_HAS_ALMOST_FULL,
    C_HAS_BACKUP,
    C_HAS_DATA_COUNT,
    C_HAS_MEMINIT_FILE,
    C_HAS_OVERFLOW,
    C_HAS_RD_DATA_COUNT,
    C_HAS_RD_RST,
    C_HAS_RST,
    C_HAS_UNDERFLOW,
    C_HAS_VALID,
    C_HAS_WR_ACK,
    C_HAS_WR_DATA_COUNT,
    C_HAS_WR_RST,
    C_IMPLEMENTATION_TYPE,
    C_INIT_WR_PNTR_VAL,
    C_MEMORY_TYPE,
    C_MIF_FILE_NAME,
    C_OPTIMIZATION_MODE,
    C_OVERFLOW_LOW,
    C_PRELOAD_LATENCY,
    C_PRELOAD_REGS,
    C_PROG_EMPTY_THRESH_ASSERT_VAL,
    C_PROG_EMPTY_THRESH_NEGATE_VAL,
    C_PROG_EMPTY_TYPE,
    C_PROG_FULL_THRESH_ASSERT_VAL,
    C_PROG_FULL_THRESH_NEGATE_VAL,
    C_PROG_FULL_TYPE,
    C_RD_DATA_COUNT_WIDTH,
    C_RD_DEPTH,
    C_RD_PNTR_WIDTH,
    C_UNDERFLOW_LOW,
    C_VALID_LOW,
    C_WR_ACK_LOW,
    C_WR_DATA_COUNT_WIDTH,
    C_WR_DEPTH,
    C_WR_PNTR_WIDTH,
    C_WR_RESPONSE_LATENCY,
    C_FULL_FLAGS_RST_VAL,
    C_USE_FWFT_DATA_COUNT,
    C_USE_EMBEDDED_REG
  )
  gen_as
  (
    .WR_CLK                   (WR_CLK),
    .RD_CLK                   (RD_CLK),
    .RST                      (RST),
    .DIN                      (DIN),
    .WR_EN                    (WR_EN),
    .RD_EN                    (RD_EN_FIFO_IN),
    .PROG_EMPTY_THRESH        (PROG_EMPTY_THRESH),
    .PROG_EMPTY_THRESH_ASSERT (PROG_EMPTY_THRESH_ASSERT),
    .PROG_EMPTY_THRESH_NEGATE (PROG_EMPTY_THRESH_NEGATE),
    .PROG_FULL_THRESH         (PROG_FULL_THRESH),
    .PROG_FULL_THRESH_ASSERT  (PROG_FULL_THRESH_ASSERT),
    .PROG_FULL_THRESH_NEGATE  (PROG_FULL_THRESH_NEGATE),
    .DOUT                     (DOUT_FIFO_OUT),
    .FULL                     (FULL_FIFO_OUT),
    .ALMOST_FULL              (ALMOST_FULL_FIFO_OUT),
    .WR_ACK                   (WR_ACK_FIFO_OUT),
    .OVERFLOW                 (OVERFLOW_FIFO_OUT),
    .EMPTY                    (EMPTY_FIFO_OUT),
    .ALMOST_EMPTY             (ALMOST_EMPTY_FIFO_OUT),
    .VALID                    (VALID_FIFO_OUT),
    .UNDERFLOW                (UNDERFLOW_FIFO_OUT),
    .RD_DATA_COUNT            (RD_DATA_COUNT_FIFO_OUT),
    .WR_DATA_COUNT            (WR_DATA_COUNT_FIFO_OUT),
    .PROG_FULL                (PROG_FULL_FIFO_OUT),
    .PROG_EMPTY               (PROG_EMPTY_FIFO_OUT)
   );
end

endcase
endgenerate


//**************************************************************************
// Connect Internal Signals
//   (Signals labeled internal_*)
//  In the normal case, these signals tie directly to the FIFO's inputs and
//    outputs.
//  In the case of Preload Latency 0 or 1, there are intermediate
//    signals between the internal FIFO and the preload logic.
//**************************************************************************

generate
if (C_PRELOAD_REGS==1 && C_PRELOAD_LATENCY==0)
 begin : block2

fifo_generator_v4_3_bhv_ver_preload0
 #(
    C_DOUT_RST_VAL,
    C_DOUT_WIDTH,
    C_HAS_RST,
    C_VALID_LOW,
    C_UNDERFLOW_LOW
   )
 fgpl0
(
 .RD_CLK           (RD_CLK_P0_IN),
 .RD_RST           (RST_P0_IN),
 .RD_EN            (RD_EN_P0_IN),
 .FIFOEMPTY        (EMPTY_P0_IN),
 .FIFODATA         (DATA_P0_IN),
 .USERDATA         (DATA_P0_OUT),
 .USERVALID        (VALID_P0_OUT),
 .USEREMPTY        (EMPTY_P0_OUT),
 .USERALMOSTEMPTY  (ALMOSTEMPTY_P0_OUT),
 .USERUNDERFLOW    (UNDERFLOW_P0_OUT),
 .RAMVALID         (RAMVALID_P0_OUT),
 .FIFORDEN         (RDEN_P0_OUT)
 );

 assign RD_CLK_P0_IN       = ((C_VERILOG_IMPL == 0) ? CLK : RD_CLK);
 assign RST_P0_IN          = RST;
 assign RD_EN_P0_IN        = RD_EN;

 assign RD_EN_FIFO_IN      = RDEN_P0_OUT;

 assign DOUT               = DATA_P0_OUT;
 assign DATA_P0_IN         = DOUT_FIFO_OUT;
 assign VALID              = VALID_P0_OUT ;
 assign EMPTY              = EMPTY_P0_OUT;
 assign ALMOST_EMPTY       = ALMOSTEMPTY_P0_OUT;
 assign EMPTY_P0_IN        = EMPTY_FIFO_OUT;
 assign UNDERFLOW          = UNDERFLOW_P0_OUT ;

 always @ (posedge RD_CLK or posedge RST)
  begin
   if (RST)
    begin
     EMPTY_P0_OUT_Q       <= 1;
     ALMOSTEMPTY_P0_OUT_Q <= 1;
    end
   else
    begin
     EMPTY_P0_OUT_Q       <= EMPTY_P0_OUT;
     ALMOSTEMPTY_P0_OUT_Q <= ALMOSTEMPTY_P0_OUT;
    end
  end

 end
else
 begin : block2

 assign RD_CLK_P0_IN       = 0;
 assign RST_P0_IN          = 0;
 assign RD_EN_P0_IN        = 0;

 assign RD_EN_FIFO_IN      = RD_EN;

 assign DOUT               = DOUT_FIFO_OUT;
 assign DATA_P0_IN         = 0;
 assign VALID              = VALID_FIFO_OUT;
 assign EMPTY              = EMPTY_FIFO_OUT;
 assign ALMOST_EMPTY       = ALMOST_EMPTY_FIFO_OUT;
 assign EMPTY_P0_IN        = 0;
 assign UNDERFLOW          = UNDERFLOW_FIFO_OUT;

 end
endgenerate


//Connect Data Count Signals
generate
if (C_USE_FWFT_DATA_COUNT==1) begin : block3
  assign RD_DATA_COUNT = (EMPTY_P0_OUT_Q | RST) ? 0 : (ALMOSTEMPTY_P0_OUT_Q ? 1 : RD_DATA_COUNT_FIFO_OUT);
end
else begin : block3
  assign RD_DATA_COUNT = RD_DATA_COUNT_FIFO_OUT;
end
endgenerate

generate
if (C_USE_FWFT_DATA_COUNT==1) begin : block4
  assign WR_DATA_COUNT = WR_DATA_COUNT_FIFO_OUT;
end
else begin : block4
  assign WR_DATA_COUNT = WR_DATA_COUNT_FIFO_OUT;
end
endgenerate


 assign FULL        = FULL_FIFO_OUT;
 assign ALMOST_FULL = ALMOST_FULL_FIFO_OUT;
 assign WR_ACK      = WR_ACK_FIFO_OUT;
 assign OVERFLOW    = OVERFLOW_FIFO_OUT;
 assign PROG_FULL   = PROG_FULL_FIFO_OUT;
 assign PROG_EMPTY  = PROG_EMPTY_FIFO_OUT;
 assign DATA_COUNT  = DATA_COUNT_FIFO_OUT;


 // if an asynchronous FIFO has been selected, display a message that the FIFO
 //   will not be cycle-accurate in simulation
 initial begin
   //if (C_IMPLEMENTATION_TYPE == 1) begin //bug in v3.1
   if (C_IMPLEMENTATION_TYPE == 2) begin   //fixed in v3.2 (IP2_Im)
     $display("Warning in %m at time %t: When using an asynchronous configuration for the FIFO Generator, the behavioral model is not cycle-accurate. You may wish to choose the structural simulation model instead of the behavioral model. This will ensure accurate behavior and latencies during simulation. You can enable this from CORE Generator by selecting Project -> Project Options -> Generation tab -> Structural Simulation. See the FIFO Generator User Guide for more information.", $time);
   end else if (C_IMPLEMENTATION_TYPE == 3 || C_IMPLEMENTATION_TYPE == 4) begin
     $display("Failure in %m at time %t: Use of Virtex-4 and Virtex-5 built-in FIFO configurations is currently not supported. Please use the structural simulation model. You can enable this from CORE Generator by selecting Project -> Project Options -> Generation tab -> Structural Simulation. See the FIFO Generator User Guide for more information.", $time);
     $finish;
   end

 end

endmodule //fifo_generator_v4_3_bhv_ver






/*******************************************************************************
 * Declaration of asynchronous FIFO Module
 ******************************************************************************/
module fifo_generator_v4_3_bhv_ver_as
  (
    WR_CLK, RD_CLK, RST, DIN, WR_EN, RD_EN,
    PROG_EMPTY_THRESH, PROG_EMPTY_THRESH_ASSERT, PROG_EMPTY_THRESH_NEGATE,
    PROG_FULL_THRESH, PROG_FULL_THRESH_ASSERT, PROG_FULL_THRESH_NEGATE,
    DOUT, FULL, ALMOST_FULL, WR_ACK, OVERFLOW, EMPTY, ALMOST_EMPTY, VALID,
    UNDERFLOW, RD_DATA_COUNT, WR_DATA_COUNT, PROG_FULL, PROG_EMPTY
  );

  /*****************************************************************************
  * Declare user parameters and their defaults
  *****************************************************************************/
  parameter  C_COMMON_CLOCK                 = 0;
  parameter  C_COUNT_TYPE                   = 0;
  parameter  C_DATA_COUNT_WIDTH             = 2;
  parameter  C_DEFAULT_VALUE                = "";
  parameter  C_DIN_WIDTH                    = 8;
  parameter  C_DOUT_RST_VAL                 = "";
  parameter  C_DOUT_WIDTH                   = 8;
  parameter  C_ENABLE_RLOCS                 = 0;
  parameter  C_FAMILY                       = "virtex2"; //Not allowed in Verilog model
  parameter  C_HAS_ALMOST_EMPTY             = 0;
  parameter  C_HAS_ALMOST_FULL              = 0;
  parameter  C_HAS_BACKUP                   = 0;
  parameter  C_HAS_DATA_COUNT               = 0;
  parameter  C_HAS_MEMINIT_FILE             = 0;
  parameter  C_HAS_OVERFLOW                 = 0;
  parameter  C_HAS_RD_DATA_COUNT            = 0;
  parameter  C_HAS_RD_RST                   = 0;
  parameter  C_HAS_RST                      = 0;
  parameter  C_HAS_UNDERFLOW                = 0;
  parameter  C_HAS_VALID                    = 0;
  parameter  C_HAS_WR_ACK                   = 0;
  parameter  C_HAS_WR_DATA_COUNT            = 0;
  parameter  C_HAS_WR_RST                   = 0;
  parameter  C_IMPLEMENTATION_TYPE          = 0;
  parameter  C_INIT_WR_PNTR_VAL             = 0;
  parameter  C_MEMORY_TYPE                  = 1;
  parameter  C_MIF_FILE_NAME                = "";
  parameter  C_OPTIMIZATION_MODE            = 0;
  parameter  C_OVERFLOW_LOW                 = 0;
  parameter  C_PRELOAD_LATENCY              = 1;
  parameter  C_PRELOAD_REGS                 = 0;
  parameter  C_PROG_EMPTY_THRESH_ASSERT_VAL = 0;
  parameter  C_PROG_EMPTY_THRESH_NEGATE_VAL = 0;
  parameter  C_PROG_EMPTY_TYPE              = 0;
  parameter  C_PROG_FULL_THRESH_ASSERT_VAL  = 0;
  parameter  C_PROG_FULL_THRESH_NEGATE_VAL  = 0;
  parameter  C_PROG_FULL_TYPE               = 0;
  parameter  C_RD_DATA_COUNT_WIDTH          = 2;
  parameter  C_RD_DEPTH                     = 256;
  parameter  C_RD_PNTR_WIDTH                = 8;
  parameter  C_UNDERFLOW_LOW                = 0;
  parameter  C_VALID_LOW                    = 0;
  parameter  C_WR_ACK_LOW                   = 0;
  parameter  C_WR_DATA_COUNT_WIDTH          = 2;
  parameter  C_WR_DEPTH                     = 256;
  parameter  C_WR_PNTR_WIDTH                = 8;
  parameter  C_WR_RESPONSE_LATENCY          = 1;
  parameter  C_FULL_FLAGS_RST_VAL           = 1;
  parameter  C_USE_FWFT_DATA_COUNT          = 0;
  parameter  C_USE_EMBEDDED_REG             = 0;

  /*****************************************************************************
  * Declare Input and Output Ports
  *****************************************************************************/
  input [C_DIN_WIDTH-1:0]            DIN;
  input [C_RD_PNTR_WIDTH-1:0]        PROG_EMPTY_THRESH;
  input [C_RD_PNTR_WIDTH-1:0]        PROG_EMPTY_THRESH_ASSERT;
  input [C_RD_PNTR_WIDTH-1:0]        PROG_EMPTY_THRESH_NEGATE;
  input [C_WR_PNTR_WIDTH-1:0]        PROG_FULL_THRESH;
  input [C_WR_PNTR_WIDTH-1:0]        PROG_FULL_THRESH_ASSERT;
  input [C_WR_PNTR_WIDTH-1:0]        PROG_FULL_THRESH_NEGATE;
  input                              RD_CLK;
  input                              RD_EN;
  input                              RST;
  input                              WR_CLK;
  input                              WR_EN;
  output                             ALMOST_EMPTY;
  output                             ALMOST_FULL;
  output [C_DOUT_WIDTH-1:0]          DOUT;
  output                             EMPTY;
  output                             FULL;
  output                             OVERFLOW;
  output                             PROG_EMPTY;
  output                             PROG_FULL;
  output                             VALID;
  output [C_RD_DATA_COUNT_WIDTH-1:0] RD_DATA_COUNT;
  output                             UNDERFLOW;
  output                             WR_ACK;
  output [C_WR_DATA_COUNT_WIDTH-1:0] WR_DATA_COUNT;

  /*******************************************************************************
  * Input and output register declarations
  ******************************************************************************/
  /*******************************************************************************
  * Parameters used as constants
  ******************************************************************************/
  //When RST is present, set FULL reset value to '1'.
  //If core has no RST, make sure FULL powers-on as '0'.
  parameter                      C_DEPTH_RATIO_WR =  
    (C_WR_DEPTH>C_RD_DEPTH) ? (C_WR_DEPTH/C_RD_DEPTH) : 1;
  parameter                      C_DEPTH_RATIO_RD =  
    (C_RD_DEPTH>C_WR_DEPTH) ? (C_RD_DEPTH/C_WR_DEPTH) : 1;
  parameter                      C_FIFO_WR_DEPTH = 
    (C_COMMON_CLOCK) ? 
    C_WR_DEPTH : C_WR_DEPTH - 1;
  parameter                      C_FIFO_RD_DEPTH = 
    (C_COMMON_CLOCK) ? 
    C_RD_DEPTH : C_RD_DEPTH - 1;

  
  // EXTRA_WORDS = 2 * C_DEPTH_RATIO_WR / C_DEPTH_RATIO_RD
  // WR_DEPTH : RD_DEPTH = 1:2 => EXTRA_WORDS = 1
  // WR_DEPTH : RD_DEPTH = 1:4 => EXTRA_WORDS = 1 (rounded to ceiling)
  // WR_DEPTH : RD_DEPTH = 2:1 => EXTRA_WORDS = 4
  // WR_DEPTH : RD_DEPTH = 4:1 => EXTRA_WORDS = 8
  parameter EXTRA_WORDS = (C_DEPTH_RATIO_RD > 1)? 1:(2 * C_DEPTH_RATIO_WR);
  // extra_words_dc = 2 * C_DEPTH_RATIO_WR / C_DEPTH_RATIO_RD
  //  C_DEPTH_RATIO_WR | C_DEPTH_RATIO_RD | C_PNTR_WIDTH    | EXTRA_WORDS_DC
  //  -----------------|------------------|-----------------|---------------
  //  1                | 8                | C_RD_PNTR_WIDTH | 2
  //  1                | 4                | C_RD_PNTR_WIDTH | 2
  //  1                | 2                | C_RD_PNTR_WIDTH | 2
  //  1                | 1                | C_WR_PNTR_WIDTH | 2
  //  2                | 1                | C_WR_PNTR_WIDTH | 4
  //  4                | 1                | C_WR_PNTR_WIDTH | 8
  //  8                | 1                | C_WR_PNTR_WIDTH | 16
  parameter EXTRA_WORDS_DC = (C_DEPTH_RATIO_WR)?
                             2:(2 * C_DEPTH_RATIO_WR/C_DEPTH_RATIO_RD);  


  //Memory which will be used to simulate a FIFO
  reg [C_DIN_WIDTH-1:0]          memory[C_WR_DEPTH-1:0];
  reg [31:0]                     num_wr_bits;
  reg [31:0]                     num_rd_bits;
  reg [31:0]                     next_num_wr_bits;
  reg [31:0]                     next_num_rd_bits;
  reg [31:0]                     wr_ptr;
  reg [31:0]                     rd_ptr;
  reg [31:0]                     wr_ptr_rdclk;
  reg [31:0]                     wr_ptr_rdclk_next;
  reg [31:0]                     rd_ptr_wrclk;
  reg [31:0]                     rd_ptr_wrclk_next;
  wire [31:0]                    num_read_words  = num_rd_bits/C_DOUT_WIDTH;
  wire [31:0]                    num_read_words_dc_i; 
  wire [31:0]                    num_read_words_dc = num_rd_bits/C_DOUT_WIDTH;
  wire [31:0]                    num_read_words_fwft_dc = (num_rd_bits/C_DOUT_WIDTH+2);
  wire [31:0]                    num_read_words_pe = 
    num_rd_bits/(C_DOUT_WIDTH/C_DEPTH_RATIO_WR);
  wire [C_RD_DATA_COUNT_WIDTH-1:0] num_read_words_sized_i; 
  wire [C_RD_DATA_COUNT_WIDTH-1:0] num_read_words_sized 
    = num_read_words_dc_i[C_RD_PNTR_WIDTH-1 : C_RD_PNTR_WIDTH-C_RD_DATA_COUNT_WIDTH]; 
  wire [C_RD_DATA_COUNT_WIDTH-1:0] num_read_words_sized_fwft 
    = num_read_words_dc_i[C_RD_PNTR_WIDTH : C_RD_PNTR_WIDTH-C_RD_DATA_COUNT_WIDTH+1]; 
  wire [31:0]                    num_write_words = num_wr_bits/C_DIN_WIDTH;
  wire [31:0]                    num_write_words_dc_i;
  wire [31:0]                    num_write_words_dc = 1+(num_wr_bits-1)/C_DIN_WIDTH;//roof of num_wr_bits/C_DIN_WIDTH
  wire [31:0]                    num_write_words_fwft_dc = 
    (num_wr_bits/C_DIN_WIDTH*C_DEPTH_RATIO_RD+2*C_DEPTH_RATIO_WR)/C_DEPTH_RATIO_RD;
  wire [31:0]                    num_write_words_pf = 
    num_wr_bits/(C_DIN_WIDTH/C_DEPTH_RATIO_RD);
  wire [C_WR_DATA_COUNT_WIDTH-1:0] num_write_words_sized_i; 
  wire [C_WR_DATA_COUNT_WIDTH-1:0] num_write_words_sized 
    = num_write_words_dc_i[C_WR_PNTR_WIDTH-1 : C_WR_PNTR_WIDTH-C_WR_DATA_COUNT_WIDTH]; 
  wire [C_WR_DATA_COUNT_WIDTH-1:0] num_write_words_sized_fwft 
    = num_write_words_dc_i[C_WR_PNTR_WIDTH : C_WR_PNTR_WIDTH-C_WR_DATA_COUNT_WIDTH+1]; 
  wire [31:0]                    reads_per_write = C_DIN_WIDTH/C_DOUT_WIDTH;
  wire [31:0]                    log2_reads_per_write = log2_val(reads_per_write);
  wire [31:0]                    writes_per_read = C_DOUT_WIDTH/C_DIN_WIDTH;
  wire [31:0]                    log2_writes_per_read = log2_val(writes_per_read);

  /*******************************************************************************
   * Internal Registers and wires
   ******************************************************************************/
  wire                           wr_ack_i;
  wire                           overflow_i;
  wire                           underflow_i;
  wire                           valid_i;
  wire                           valid_out;
  reg                            valid_d1;
  /*******************************************************************************
   * Internal registers and wires for internal reset logics 
   ******************************************************************************/
  reg                            rd_rst_asreg    =0;
  reg                            rd_rst_asreg_d1 =0;
  reg                            rd_rst_asreg_d2 =0;
  reg                            rd_rst_reg      =0;
  reg                            rd_rst_d1       =0;
  reg                            wr_rst_asreg    =0;
  reg                            wr_rst_asreg_d1 =0;
  reg                            wr_rst_asreg_d2 =0;
  reg                            wr_rst_reg      =0;
  reg                            wr_rst_d1       =0;
  wire                           rd_rst_comb;
  wire                           rd_rst_i;
  wire                           wr_rst_comb;
  wire                           wr_rst_i;



  //Special ideal FIFO signals
  reg [C_DOUT_WIDTH-1:0]         ideal_dout;
  wire [C_DOUT_WIDTH-1:0]        ideal_dout_out;
  reg [C_DOUT_WIDTH-1:0]         ideal_dout_d1;
  reg                            ideal_wr_ack;
  reg                            ideal_valid;
  reg                            ideal_overflow;
  reg                            ideal_underflow;
  reg                            ideal_full;
  reg                            ideal_empty;
  reg                            ideal_almost_full;
  reg                            ideal_almost_empty;
  reg                            ideal_prog_full;
  reg                            ideal_prog_empty;

  //MSBs of the counts
  reg [C_WR_DATA_COUNT_WIDTH-1 : 0] ideal_wr_count;
  reg [C_RD_DATA_COUNT_WIDTH-1 : 0] ideal_rd_count;

  //user specified value for reseting the size of the fifo
  reg [C_DOUT_WIDTH-1:0]            dout_reset_val;

  //temporary registers for WR_RESPONSE_LATENCY feature

  integer                           tmp_wr_listsize;
  integer                           tmp_rd_listsize;

  //Signal for registered version of prog full and empty
  reg                               prog_full_d;
  reg                               prog_empty_d;

  //Threshold values for Programmable Flags
  integer                           prog_empty_actual_thresh_assert;
  integer                           prog_empty_actual_thresh_negate;
  integer                           prog_full_actual_thresh_assert;
  integer                           prog_full_actual_thresh_negate;


  /****************************************************************************
   * Function Declarations
   ***************************************************************************/

  task write_fifo;
    begin
      memory[wr_ptr]     <= DIN;
      if (wr_ptr == 0) begin
        wr_ptr          <= C_WR_DEPTH - 1;
      end else begin
        wr_ptr          <= wr_ptr - 1;
      end
    end
  endtask // write_fifo

  task read_fifo;
    integer i;
    reg [C_DOUT_WIDTH-1:0] tmp_dout;
    reg [C_DIN_WIDTH-1:0]  memory_read;
    reg [31:0]             tmp_rd_ptr;
    reg [31:0]             rd_ptr_high;
    reg [31:0]             rd_ptr_low;
    begin
      // output is wider than input
      if (reads_per_write == 0) begin
        tmp_dout = 0;
        tmp_rd_ptr = (rd_ptr << log2_writes_per_read)+(writes_per_read-1);
        for (i = writes_per_read - 1; i >= 0; i = i - 1) begin
          tmp_dout = tmp_dout << C_DIN_WIDTH;
          tmp_dout = tmp_dout | memory[tmp_rd_ptr];
          if (tmp_rd_ptr == 0) begin
            tmp_rd_ptr = C_WR_DEPTH - 1;
          end else begin
            tmp_rd_ptr = tmp_rd_ptr - 1;
          end
        end

      // output is symmetric
      end else if (reads_per_write == 1) begin
        tmp_dout = memory[rd_ptr];

      // input is wider than output
      end else begin
        rd_ptr_high = rd_ptr >> log2_reads_per_write;
        rd_ptr_low  = rd_ptr & (reads_per_write - 1);
        memory_read = memory[rd_ptr_high];
        tmp_dout    = memory_read >> (rd_ptr_low*C_DOUT_WIDTH);
      end
      ideal_dout <= tmp_dout;
      if (rd_ptr == 0) begin
        rd_ptr <= C_RD_DEPTH - 1;
      end else begin
        rd_ptr <= rd_ptr - 1;
      end
    end
  endtask

  /****************************************************************************
  * log2_val
  *   Returns the 'log2' value for the input value for the supported ratios
  ***************************************************************************/
  function [31:0] log2_val;
    input [31:0] binary_val;

    begin
      if (binary_val == 8) begin
        log2_val = 3;
      end else if (binary_val == 4) begin
        log2_val = 2;
      end else begin
        log2_val = 1;
      end
    end
  endfunction

  /*************************************************************************
  * hexstr_conv
  *   Converts a string of type hex to a binary value (for C_DOUT_RST_VAL)
  ***********************************************************************/
  function [C_DOUT_WIDTH-1:0] hexstr_conv;
    input [(C_DOUT_WIDTH*8)-1:0] def_data;

    integer index,i,j;
    reg [3:0] bin;

    begin
      index = 0;
      hexstr_conv = 'b0;
      for( i=C_DOUT_WIDTH-1; i>=0; i=i-1 )
      begin
        case (def_data[7:0])
          8'b00000000 :
          begin
            bin = 4'b0000;
            i = -1;
          end
          8'b00110000 : bin = 4'b0000;
          8'b00110001 : bin = 4'b0001;
          8'b00110010 : bin = 4'b0010;
          8'b00110011 : bin = 4'b0011;
          8'b00110100 : bin = 4'b0100;
          8'b00110101 : bin = 4'b0101;
          8'b00110110 : bin = 4'b0110;
          8'b00110111 : bin = 4'b0111;
          8'b00111000 : bin = 4'b1000;
          8'b00111001 : bin = 4'b1001;
          8'b01000001 : bin = 4'b1010;
          8'b01000010 : bin = 4'b1011;
          8'b01000011 : bin = 4'b1100;
          8'b01000100 : bin = 4'b1101;
          8'b01000101 : bin = 4'b1110;
          8'b01000110 : bin = 4'b1111;
          8'b01100001 : bin = 4'b1010;
          8'b01100010 : bin = 4'b1011;
          8'b01100011 : bin = 4'b1100;
          8'b01100100 : bin = 4'b1101;
          8'b01100101 : bin = 4'b1110;
          8'b01100110 : bin = 4'b1111;
          default :
          begin
            bin = 4'bx;
          end
        endcase
        for( j=0; j<4; j=j+1)
        begin
          if ((index*4)+j < C_DOUT_WIDTH)
          begin
            hexstr_conv[(index*4)+j] = bin[j];
          end
        end
        index = index + 1;
        def_data = def_data >> 8;
      end
    end
  endfunction

  /*************************************************************************
  * Initialize Signals for clean power-on simulation
  *************************************************************************/
  initial
    begin
      num_wr_bits        = 0;
      num_rd_bits        = 0;
      next_num_wr_bits   = 0;
      next_num_rd_bits   = 0;
      rd_ptr             = C_RD_DEPTH - 1;
      wr_ptr             = C_WR_DEPTH - 1;
      rd_ptr_wrclk       = rd_ptr;
      wr_ptr_rdclk       = wr_ptr;
      dout_reset_val     = hexstr_conv(C_DOUT_RST_VAL);
      ideal_dout         = dout_reset_val;
      ideal_wr_ack       = 1'b0;
      ideal_valid        = 1'b0;
      ideal_overflow     = 1'b0;
      ideal_underflow    = 1'b0;
      //Modified the start-up value of FULL to '0' in v3.2 (IP2_Im)
      //ideal_full         = C_FULL_RESET_VAL; //was in v3.1
      ideal_full         = 1'b0; //v3.2 
      ideal_empty        = 1'b1;
      //Modified the start-up value of ALMOST_FULL to '0' in v3.2 (IP2_Im)
      //ideal_almost_full  = C_ALMOST_FULL_RESET_VAL; //was in v3.1
      ideal_almost_full  = 1'b0; //v3.2
      ideal_almost_empty = 1'b1;
      ideal_wr_count     = 0;
      ideal_rd_count     = 0;
      //Modified the start-up value of PROG_FULL to '0' in v3.2 (IP2_Im)
      //ideal_prog_full    = C_PROG_FULL_RESET_VAL; //was in v3.1
      ideal_prog_full    = 1'b0; //v3.2
      ideal_prog_empty   = 1'b1;
      //Modified the start-up value of PROG_FULL to '0' in v3.2 (IP2_Im)
      //Therefore, prog_full_d has to start-up at '0' too
      //prog_full_d        = C_PROG_FULL_RESET_VAL; //was in v3.1
      prog_full_d        = 1'b0; //v3.2
      prog_empty_d       = 1'b1;
    end



  /*************************************************************************
   * Assign Internal ideal signals to output ports
   *************************************************************************/
  assign ideal_dout_out= (C_PRELOAD_LATENCY==2 &&
                          (C_MEMORY_TYPE==0 || C_MEMORY_TYPE==1))?
                         ideal_dout_d1: ideal_dout;   
  assign DOUT          = ideal_dout_out;
  assign FULL          = ideal_full;
  assign EMPTY         = ideal_empty;
  assign ALMOST_FULL   = ideal_almost_full;
  assign ALMOST_EMPTY  = ideal_almost_empty;
  assign num_write_words_sized_i=C_USE_FWFT_DATA_COUNT?
                                num_write_words_sized_fwft:num_write_words_sized;
  assign num_read_words_sized_i=C_USE_FWFT_DATA_COUNT?
                                num_read_words_sized_fwft:num_read_words_sized;
  assign num_write_words_dc_i = C_USE_FWFT_DATA_COUNT?
                               num_write_words_fwft_dc: num_write_words_dc;
  assign num_read_words_dc_i = C_USE_FWFT_DATA_COUNT?
                               num_read_words_fwft_dc: num_read_words_dc;
  assign WR_DATA_COUNT = ideal_wr_count;
  assign RD_DATA_COUNT = ideal_rd_count;
  assign PROG_FULL     = ideal_prog_full;
  assign PROG_EMPTY    = ideal_prog_empty;

  //Handshaking signals can be active low, depending on _LOW parameters
  assign valid_i       = (C_PRELOAD_LATENCY==0) ? (RD_EN & ~EMPTY) : ideal_valid;
  assign VALID         = valid_out ? !C_VALID_LOW : C_VALID_LOW;
  assign valid_out     = (C_PRELOAD_LATENCY==2 &&
                          (C_MEMORY_TYPE==0 || C_MEMORY_TYPE==1))?
                         valid_d1: valid_i;  
  assign underflow_i   = (C_PRELOAD_LATENCY==0) ? (RD_EN & EMPTY) : ideal_underflow;
  assign UNDERFLOW     = underflow_i ? !C_UNDERFLOW_LOW : C_UNDERFLOW_LOW;

  assign WR_ACK        = wr_ack_i ? !C_WR_ACK_LOW : C_WR_ACK_LOW;
  assign wr_ack_i      = (C_WR_RESPONSE_LATENCY==1) ? ideal_wr_ack :
                            (WR_EN & !FULL);
  assign OVERFLOW      = overflow_i ? !C_OVERFLOW_LOW : C_OVERFLOW_LOW;
  assign overflow_i    = (C_WR_RESPONSE_LATENCY==1) ? ideal_overflow :
                             (WR_EN & FULL);

  /*******************************************************************************
  * Internal reset logics 
  ******************************************************************************/
  assign wr_rst_comb      = !wr_rst_asreg_d2 && wr_rst_asreg;
  assign rd_rst_comb      = !rd_rst_asreg_d2 && rd_rst_asreg;
  assign wr_rst_i         = C_HAS_RST ? wr_rst_reg : 0;
  assign rd_rst_i         = C_HAS_RST ? rd_rst_reg : 0;


  always @(posedge RD_CLK or posedge rd_rst_i) begin
    if (rd_rst_i == 1'b1) begin
      valid_d1 <= 1'b0;
    end else begin
      valid_d1 <= valid_i;
    end    
  end   
  always @(posedge RD_CLK or posedge rd_rst_i) begin
    if (rd_rst_i == 1'b1) begin
      ideal_dout_d1 <= dout_reset_val;
    end else begin
      ideal_dout_d1 <= ideal_dout;
    end    
  end   
  
  always @(posedge WR_CLK or posedge RST) begin
    if (RST == 1'b1) begin
      wr_rst_asreg <= 1'b1;
    end else begin
      if (wr_rst_asreg_d1 == 1'b1) begin
        wr_rst_asreg <= 1'b0;
      end else begin
        wr_rst_asreg <= wr_rst_asreg;
      end
    end    
  end   

  always @(posedge WR_CLK) begin
    wr_rst_asreg_d1 <= wr_rst_asreg;
    wr_rst_asreg_d2 <= wr_rst_asreg_d1;
  end
  
  always @(posedge WR_CLK or posedge wr_rst_comb) begin
    if (wr_rst_comb == 1'b1) begin
      wr_rst_reg <= 1'b1;
    end else begin
      wr_rst_reg <= 1'b0;
    end    
  end   

  always @(posedge WR_CLK or posedge wr_rst_i) begin
    if (wr_rst_i == 1'b1) begin
      wr_rst_d1 <= 1'b1;
    end else begin
      wr_rst_d1 <= wr_rst_i;
    end    
  end   
  always @(posedge RD_CLK or posedge RST) begin
    if (RST == 1'b1) begin
      rd_rst_asreg <= 1'b1;
    end else begin
      if (rd_rst_asreg_d1 == 1'b1) begin
        rd_rst_asreg <= 1'b0;
      end else begin
        rd_rst_asreg <= rd_rst_asreg;
      end
    end    
  end   

  always @(posedge RD_CLK) begin
    rd_rst_asreg_d1 <= rd_rst_asreg;
    rd_rst_asreg_d2 <= rd_rst_asreg_d1;
  end
  
  always @(posedge RD_CLK or posedge rd_rst_comb) begin
    if (rd_rst_comb == 1'b1) begin
      rd_rst_reg <= 1'b1;
    end else begin
      rd_rst_reg <= 1'b0;
    end    
  end   
   
   //Generate overflow and underflow flags seperately
   //because they don't support async rst
   always @(posedge WR_CLK) begin
     ideal_overflow    <= WR_EN & ideal_full;
   end
   always @(posedge RD_CLK) begin
     ideal_underflow    <= ideal_empty & RD_EN;
   end

   /*******************************************************************************
   * Write and Read Logics 
   ******************************************************************************/
   always @(posedge WR_CLK or posedge wr_rst_i) begin : gen_fifo_w

     /****** Reset fifo (case 1)***************************************/
     if (wr_rst_i == 1'b1) begin
       num_wr_bits       <= 0;
       next_num_wr_bits  <= 0;
       wr_ptr            <= C_WR_DEPTH - 1;
       rd_ptr_wrclk      <= C_RD_DEPTH - 1;
       ideal_wr_ack      <= 0;
       ideal_full        <= C_FULL_FLAGS_RST_VAL;
       ideal_almost_full <= C_FULL_FLAGS_RST_VAL;
       ideal_wr_count    <= 0;

       ideal_prog_full   <= C_FULL_FLAGS_RST_VAL;
       prog_full_d       <= C_FULL_FLAGS_RST_VAL;

     end else begin //wr_rst_i==0

       //Determine the current number of words in the FIFO
       tmp_wr_listsize = (C_DEPTH_RATIO_RD > 1) ? num_wr_bits/C_DOUT_WIDTH :
                         num_wr_bits/C_DIN_WIDTH;
       rd_ptr_wrclk_next = rd_ptr;
       if (rd_ptr_wrclk < rd_ptr_wrclk_next) begin
         next_num_wr_bits = num_wr_bits -
                            C_DOUT_WIDTH*(rd_ptr_wrclk + C_RD_DEPTH
                                          - rd_ptr_wrclk_next);
       end else begin
         next_num_wr_bits = num_wr_bits -
                            C_DOUT_WIDTH*(rd_ptr_wrclk - rd_ptr_wrclk_next);
       end

       //If this is a write, handle the write by adding the value
       // to the linked list, and updating all outputs appropriately
       if (WR_EN == 1'b1) begin
         if (ideal_full == 1'b1) begin

           //If the FIFO is full, do NOT perform the write,
           // update flags accordingly
           if ((tmp_wr_listsize + C_DEPTH_RATIO_RD - 1)/C_DEPTH_RATIO_RD 
             >= C_FIFO_WR_DEPTH) begin
             //write unsuccessful - do not change contents

             //Do not acknowledge the write
             ideal_wr_ack      <= 0;
             //Reminder that FIFO is still full
             ideal_full        <= 1'b1;
             ideal_almost_full <= 1'b1;

             ideal_wr_count    <= num_write_words_sized_i;

           //If the FIFO is one from full, but reporting full
           end else if ((tmp_wr_listsize + C_DEPTH_RATIO_RD - 1)/C_DEPTH_RATIO_RD ==
                        C_FIFO_WR_DEPTH-1) begin
             //No change to FIFO

             //Write not successful
             ideal_wr_ack      <= 0;
             //With DEPTH-1 words in the FIFO, it is almost_full
             ideal_full        <= 1'b0;
             ideal_almost_full <= 1'b1;

             ideal_wr_count    <= num_write_words_sized_i;


           //If the FIFO is completely empty, but it is
           // reporting FULL for some reason (like reset)
           end else if ((tmp_wr_listsize + C_DEPTH_RATIO_RD - 1)/C_DEPTH_RATIO_RD <=
                        C_FIFO_WR_DEPTH-2) begin
             //No change to FIFO

             //Write not successful
             ideal_wr_ack      <= 0;
             //FIFO is really not close to full, so change flag status.
             ideal_full        <= 1'b0;
             ideal_almost_full <= 1'b0;

             ideal_wr_count    <= num_write_words_sized_i;
           end //(tmp_wr_listsize == 0)

         end else begin

           //If the FIFO is full, do NOT perform the write,
           // update flags accordingly
           if ((tmp_wr_listsize + C_DEPTH_RATIO_RD - 1)/C_DEPTH_RATIO_RD >=
              C_FIFO_WR_DEPTH) begin
             //write unsuccessful - do not change contents

             //Do not acknowledge the write
             ideal_wr_ack       <= 0;
             //Reminder that FIFO is still full
             ideal_full         <= 1'b1;
             ideal_almost_full  <= 1'b1;

             ideal_wr_count     <= num_write_words_sized_i;

           //If the FIFO is one from full
           end else if ((tmp_wr_listsize + C_DEPTH_RATIO_RD - 1)/C_DEPTH_RATIO_RD ==
                       C_FIFO_WR_DEPTH-1) begin
             //Add value on DIN port to FIFO
             write_fifo;
             next_num_wr_bits = next_num_wr_bits + C_DIN_WIDTH;

             //Write successful, so issue acknowledge
             // and no error
             ideal_wr_ack      <= 1;
             //This write is CAUSING the FIFO to go full
             ideal_full        <= 1'b1;
             ideal_almost_full <= 1'b1;

             ideal_wr_count    <= num_write_words_sized_i;

           //If the FIFO is 2 from full
           end else if ((tmp_wr_listsize + C_DEPTH_RATIO_RD - 1)/C_DEPTH_RATIO_RD ==
                        C_FIFO_WR_DEPTH-2) begin
             //Add value on DIN port to FIFO
             write_fifo;
             next_num_wr_bits =  next_num_wr_bits + C_DIN_WIDTH;
             //Write successful, so issue acknowledge
             // and no error
             ideal_wr_ack      <= 1;
             //Still 2 from full
             ideal_full        <= 1'b0;
             //2 from full, and writing, so set almost_full
             ideal_almost_full <= 1'b1;

             ideal_wr_count    <= num_write_words_sized_i;

           //If the FIFO is not close to being full
           end else if ((tmp_wr_listsize + C_DEPTH_RATIO_RD - 1)/C_DEPTH_RATIO_RD <
                       C_FIFO_WR_DEPTH-2) begin
             //Add value on DIN port to FIFO
             write_fifo;
             next_num_wr_bits  = next_num_wr_bits + C_DIN_WIDTH;
             //Write successful, so issue acknowledge
             // and no error
             ideal_wr_ack      <= 1;
             //Not even close to full.
             ideal_full        <= 1'b0;
             ideal_almost_full <= 1'b0;

             ideal_wr_count    <= num_write_words_sized_i;

           end

         end

       end else begin //(WR_EN == 1'b1)

         //If user did not attempt a write, then do not
         // give ack or err
         ideal_wr_ack   <= 0;

         //Implied statements:
         //ideal_empty <= ideal_empty;
         //ideal_almost_empty <= ideal_almost_empty;

         //Check for full
         if ((tmp_wr_listsize + C_DEPTH_RATIO_RD - 1)/C_DEPTH_RATIO_RD >= C_FIFO_WR_DEPTH)
           ideal_full <= 1'b1;
         else
           ideal_full <= 1'b0;

         //Check for almost_full
         if ((tmp_wr_listsize + C_DEPTH_RATIO_RD - 1)/C_DEPTH_RATIO_RD >= C_FIFO_WR_DEPTH-1)
           ideal_almost_full  <= 1'b1;
         else
           ideal_almost_full  <= 1'b0;

         ideal_wr_count <= num_write_words_sized_i;
       end

       /*********************************************************
        * Programmable FULL flags
        *********************************************************/
       //Single Programmable Full Constant Threshold
       if (C_PROG_FULL_TYPE==1) begin
         if (C_PRELOAD_REGS==1 && C_PRELOAD_LATENCY==0) begin
           prog_full_actual_thresh_assert = C_PROG_FULL_THRESH_ASSERT_VAL-EXTRA_WORDS;
           prog_full_actual_thresh_negate = C_PROG_FULL_THRESH_ASSERT_VAL-EXTRA_WORDS;
         end else begin
           prog_full_actual_thresh_assert = C_PROG_FULL_THRESH_ASSERT_VAL;
           prog_full_actual_thresh_negate = C_PROG_FULL_THRESH_ASSERT_VAL;
         end

       //Two Programmable Full Constant Thresholds
       end else if (C_PROG_FULL_TYPE==2) begin
         if (C_PRELOAD_REGS==1 && C_PRELOAD_LATENCY==0) begin
           prog_full_actual_thresh_assert = C_PROG_FULL_THRESH_ASSERT_VAL-EXTRA_WORDS;
           prog_full_actual_thresh_negate = C_PROG_FULL_THRESH_NEGATE_VAL-EXTRA_WORDS;
         end else begin
           prog_full_actual_thresh_assert = C_PROG_FULL_THRESH_ASSERT_VAL;
           prog_full_actual_thresh_negate = C_PROG_FULL_THRESH_NEGATE_VAL;
         end

       //Single Programmable Full Threshold Input
       end else if (C_PROG_FULL_TYPE==3) begin
         if (C_PRELOAD_REGS==1 && C_PRELOAD_LATENCY==0) begin
           prog_full_actual_thresh_assert = PROG_FULL_THRESH-EXTRA_WORDS;
           prog_full_actual_thresh_negate = PROG_FULL_THRESH-EXTRA_WORDS;
         end else begin
           prog_full_actual_thresh_assert = PROG_FULL_THRESH;
           prog_full_actual_thresh_negate = PROG_FULL_THRESH;
         end

       //Two Programmable Full Threshold Inputs
       end else if (C_PROG_FULL_TYPE==4) begin
         if (C_PRELOAD_REGS==1 && C_PRELOAD_LATENCY==0) begin
           prog_full_actual_thresh_assert = PROG_FULL_THRESH_ASSERT-EXTRA_WORDS;
           prog_full_actual_thresh_negate = PROG_FULL_THRESH_NEGATE-EXTRA_WORDS;
         end else begin
           prog_full_actual_thresh_assert = PROG_FULL_THRESH_ASSERT;
           prog_full_actual_thresh_negate = PROG_FULL_THRESH_NEGATE;
         end
       end //C_PROG_FULL_TYPE
       
       if (num_write_words_pf==0) begin
          prog_full_d <= 1'b0;
       end else begin
         if (((1+(num_write_words_pf-1)/C_DEPTH_RATIO_RD) 
              == prog_full_actual_thresh_assert-1) && WR_EN) begin
            prog_full_d <= 1'b1;
         end else if ((1+(num_write_words_pf-1)/C_DEPTH_RATIO_RD) 
                      >= prog_full_actual_thresh_assert) begin
            prog_full_d <= 1'b1;
         end else if ((1+(num_write_words_pf-1)/C_DEPTH_RATIO_RD) 
                      < prog_full_actual_thresh_negate) begin
            prog_full_d <= 1'b0;
         end
       end  

       if (wr_rst_d1==1 && wr_rst_i==0) begin
         ideal_prog_full   <= 0;
       end else begin
         ideal_prog_full   <= prog_full_d;
       end
       num_wr_bits       <= next_num_wr_bits;
       rd_ptr_wrclk      <= rd_ptr;

     end //wr_rst_i==0
   end // write always

   always @(posedge RD_CLK or posedge rd_rst_i) begin : gen_fifo_r

     /****** Reset fifo (case 1)***************************************/
     if (rd_rst_i) begin
       num_rd_bits        <= 0;
       next_num_rd_bits   <= 0;
       rd_ptr             <= C_RD_DEPTH -1;
       wr_ptr_rdclk       <= C_WR_DEPTH -1;
       ideal_dout         <= dout_reset_val;
       ideal_valid        <= 1'b0;
       ideal_empty        <= 1'b1;
       ideal_almost_empty <= 1'b1;
       ideal_rd_count     <= 0;

       ideal_prog_empty   <= 1'b1;
       prog_empty_d       <= 1;


     end else begin //rd_rst_i==0

       //Determine the current number of words in the FIFO
       tmp_rd_listsize = (C_DEPTH_RATIO_WR > 1) ? num_rd_bits/C_DIN_WIDTH :
                         num_rd_bits/C_DOUT_WIDTH;
       wr_ptr_rdclk_next = wr_ptr;

       if (wr_ptr_rdclk < wr_ptr_rdclk_next) begin
         next_num_rd_bits = num_rd_bits +
                            C_DIN_WIDTH*(wr_ptr_rdclk +C_WR_DEPTH
                                         - wr_ptr_rdclk_next);
       end else begin
         next_num_rd_bits = num_rd_bits +
                             C_DIN_WIDTH*(wr_ptr_rdclk - wr_ptr_rdclk_next);
       end

       /*****************************************************************/
       // Read Operation - Read Latency 1
       /*****************************************************************/
       if (C_PRELOAD_LATENCY==1 || C_PRELOAD_LATENCY==2) begin

         if (RD_EN == 1'b1) begin

           if (ideal_empty == 1'b1) begin

             //If the FIFO is completely empty, and is reporting empty
             if (tmp_rd_listsize/C_DEPTH_RATIO_WR <= 0)
               begin
                 //Do not change the contents of the FIFO

                 //Do not acknowledge the read from empty FIFO
                 ideal_valid        <= 1'b0;
                 //Reminder that FIFO is still empty
                 ideal_empty        <= 1'b1;
                 ideal_almost_empty <= 1'b1;

                 ideal_rd_count     <= num_read_words_sized_i;
               end // if (tmp_rd_listsize <= 0)

             //If the FIFO is one from empty, but it is reporting empty
             else if (tmp_rd_listsize/C_DEPTH_RATIO_WR == 1)
               begin
                 //Do not change the contents of the FIFO

                 //Do not acknowledge the read from empty FIFO
                 ideal_valid        <= 1'b0;
                 //Note that FIFO is no longer empty, but is almost empty (has one word left)
                 ideal_empty        <= 1'b0;
                 ideal_almost_empty <= 1'b1;

                 ideal_rd_count     <= num_read_words_sized_i;

               end // if (tmp_rd_listsize == 1)

             //If the FIFO is two from empty, and is reporting empty
             else if (tmp_rd_listsize/C_DEPTH_RATIO_WR == 2)
               begin
                 //Do not change the contents of the FIFO

                 //Do not acknowledge the read from empty FIFO
                 ideal_valid        <= 1'b0;
                 //Fifo has two words, so is neither empty or almost empty
                 ideal_empty        <= 1'b0;
                 ideal_almost_empty <= 1'b0;

                 ideal_rd_count     <= num_read_words_sized_i;

               end // if (tmp_rd_listsize == 2)

             //If the FIFO is not close to empty, but is reporting that it is
             // Treat the FIFO as empty this time, but unset EMPTY flags.
             if ((tmp_rd_listsize/C_DEPTH_RATIO_WR > 2) && (tmp_rd_listsize/C_DEPTH_RATIO_WR<C_FIFO_RD_DEPTH))
               begin
                 //Do not change the contents of the FIFO

                 //Do not acknowledge the read from empty FIFO
                 ideal_valid <= 1'b0;
                 //Note that the FIFO is No Longer Empty or Almost Empty
                 ideal_empty  <= 1'b0;
                 ideal_almost_empty <= 1'b0;

                 ideal_rd_count <= num_read_words_sized_i;

               end // if ((tmp_rd_listsize > 2) && (tmp_rd_listsize<=C_FIFO_RD_DEPTH-1))
             end // else: if(ideal_empty == 1'b1)

           else //if (ideal_empty == 1'b0)
             begin

               //If the FIFO is completely full, and we are successfully reading from it
               if (tmp_rd_listsize/C_DEPTH_RATIO_WR >= C_FIFO_RD_DEPTH)
                 begin
                   //Read the value from the FIFO
                   read_fifo;
                   next_num_rd_bits = next_num_rd_bits - C_DOUT_WIDTH;

                   //Acknowledge the read from the FIFO, no error
                   ideal_valid        <= 1'b1;
                   //Not close to empty
                   ideal_empty        <= 1'b0;
                   ideal_almost_empty <= 1'b0;

                   ideal_rd_count     <= num_read_words_sized_i;

                 end // if (tmp_rd_listsize == C_FIFO_RD_DEPTH)

               //If the FIFO is not close to being empty
               else if ((tmp_rd_listsize/C_DEPTH_RATIO_WR > 2) && (tmp_rd_listsize/C_DEPTH_RATIO_WR<=C_FIFO_RD_DEPTH))
                 begin
                   //Read the value from the FIFO
                   read_fifo;
                   next_num_rd_bits = next_num_rd_bits - C_DOUT_WIDTH;

                   //Acknowledge the read from the FIFO, no error
                   ideal_valid        <= 1'b1;
                   //Not close to empty
                   ideal_empty        <= 1'b0;
                   ideal_almost_empty <= 1'b0;

                   ideal_rd_count     <= num_read_words_sized_i;

                 end // if ((tmp_rd_listsize > 2) && (tmp_rd_listsize<=C_FIFO_RD_DEPTH-1))

               //If the FIFO is two from empty
               else if (tmp_rd_listsize/C_DEPTH_RATIO_WR == 2)
                 begin
                   //Read the value from the FIFO
                   read_fifo;
                   next_num_rd_bits = next_num_rd_bits - C_DOUT_WIDTH;

                   //Acknowledge the read from the FIFO, no error
                   ideal_valid        <= 1'b1;
                   //Fifo is not yet empty. It is going almost_empty
                   ideal_empty        <= 1'b0;
                   ideal_almost_empty <= 1'b1;

                   ideal_rd_count     <= num_read_words_sized_i;

                 end // if (tmp_rd_listsize == 2)

               //If the FIFO is one from empty
               else if ((tmp_rd_listsize/C_DEPTH_RATIO_WR == 1))
                 begin
                   //Read the value from the FIFO
                   read_fifo;
                   next_num_rd_bits = next_num_rd_bits - C_DOUT_WIDTH;

                   //Acknowledge the read from the FIFO, no error
                   ideal_valid        <= 1'b1;
                   //Note that FIFO is GOING empty
                   ideal_empty        <= 1'b1;
                   ideal_almost_empty <= 1'b1;

                   ideal_rd_count     <= num_read_words_sized_i;

                 end // if (tmp_rd_listsize == 1)


               //If the FIFO is completely empty
               else if (tmp_rd_listsize/C_DEPTH_RATIO_WR <= 0)
                 begin
                   //Do not change the contents of the FIFO

                   //Do not acknowledge the read from empty FIFO
                   ideal_valid        <= 1'b0;
                   //Reminder that FIFO is still empty
                   ideal_empty        <= 1'b1;
                   ideal_almost_empty <= 1'b1;

                   ideal_rd_count     <= num_read_words_sized_i;

                 end // if (tmp_rd_listsize <= 0)

             end // if (ideal_empty == 1'b0)

           end //(RD_EN == 1'b1)

         else //if (RD_EN == 1'b0)
           begin
             //If user did not attempt a read, do not give an ack or err
             ideal_valid          <= 1'b0;

             //Check for empty
             if (tmp_rd_listsize/C_DEPTH_RATIO_WR <= 0)
               ideal_empty        <= 1'b1;
             else
               ideal_empty        <= 1'b0;

             //Check for almost_empty
             if (tmp_rd_listsize/C_DEPTH_RATIO_WR <= 1)
               ideal_almost_empty <= 1'b1;
             else
               ideal_almost_empty <= 1'b0;

             ideal_rd_count       <= num_read_words_sized_i;

           end // else: !if(RD_EN == 1'b1)

       /*****************************************************************/
       // Read Operation - Read Latency 0
       /*****************************************************************/
       end else if (C_PRELOAD_REGS==1 && C_PRELOAD_LATENCY==0) begin
         if (RD_EN == 1'b1) begin

           if (ideal_empty == 1'b1) begin

             //If the FIFO is completely empty, and is reporting empty
             if (tmp_rd_listsize/C_DEPTH_RATIO_WR <= 0) begin
               //Do not change the contents of the FIFO

               //Do not acknowledge the read from empty FIFO
               ideal_valid        <= 1'b0;
               //Reminder that FIFO is still empty
               ideal_empty        <= 1'b1;
               ideal_almost_empty <= 1'b1;

               ideal_rd_count     <= num_read_words_sized_i;

             //If the FIFO is one from empty, but it is reporting empty
             end else if (tmp_rd_listsize/C_DEPTH_RATIO_WR == 1) begin
               //Do not change the contents of the FIFO

               //Do not acknowledge the read from empty FIFO
               ideal_valid        <= 1'b0;
               //Note that FIFO is no longer empty, but is almost empty (has one word left)
               ideal_empty        <= 1'b0;
               ideal_almost_empty <= 1'b1;

               ideal_rd_count     <= num_read_words_sized_i;

             //If the FIFO is two from empty, and is reporting empty
             end else if (tmp_rd_listsize/C_DEPTH_RATIO_WR == 2) begin
               //Do not change the contents of the FIFO

               //Do not acknowledge the read from empty FIFO
               ideal_valid        <= 1'b0;
               //Fifo has two words, so is neither empty or almost empty
               ideal_empty        <= 1'b0;
               ideal_almost_empty <= 1'b0;

               ideal_rd_count     <= num_read_words_sized_i;

               //If the FIFO is not close to empty, but is reporting that it is
             // Treat the FIFO as empty this time, but unset EMPTY flags.
             end else if ((tmp_rd_listsize/C_DEPTH_RATIO_WR > 2) &&
                         (tmp_rd_listsize/C_DEPTH_RATIO_WR<C_FIFO_RD_DEPTH)) begin
               //Do not change the contents of the FIFO

               //Do not acknowledge the read from empty FIFO
               ideal_valid        <= 1'b0;
               //Note that the FIFO is No Longer Empty or Almost Empty
               ideal_empty        <= 1'b0;
               ideal_almost_empty <= 1'b0;

               ideal_rd_count     <= num_read_words_sized_i;

             end // if ((tmp_rd_listsize > 2) && (tmp_rd_listsize<=C_FIFO_RD_DEPTH-1))

           end else begin

             //If the FIFO is completely full, and we are successfully reading from it
             if (tmp_rd_listsize/C_DEPTH_RATIO_WR >= C_FIFO_RD_DEPTH) begin
               //Read the value from the FIFO
               read_fifo;
               next_num_rd_bits = next_num_rd_bits - C_DOUT_WIDTH;

               //Acknowledge the read from the FIFO, no error
               ideal_valid        <= 1'b1;
               //Not close to empty
               ideal_empty        <= 1'b0;
               ideal_almost_empty <= 1'b0;

               ideal_rd_count     <= num_read_words_sized_i;

             //If the FIFO is not close to being empty
             end else if ((tmp_rd_listsize/C_DEPTH_RATIO_WR > 2) &&
                          (tmp_rd_listsize/C_DEPTH_RATIO_WR<=C_FIFO_RD_DEPTH)) begin
               //Read the value from the FIFO
               read_fifo;
               next_num_rd_bits = next_num_rd_bits - C_DOUT_WIDTH;

               //Acknowledge the read from the FIFO, no error
               ideal_valid        <= 1'b1;
               //Not close to empty
               ideal_empty        <= 1'b0;
               ideal_almost_empty <= 1'b0;

               ideal_rd_count     <= num_read_words_sized_i;

             //If the FIFO is two from empty
             end else if (tmp_rd_listsize/C_DEPTH_RATIO_WR == 2) begin
               //Read the value from the FIFO
               read_fifo;
               next_num_rd_bits = next_num_rd_bits - C_DOUT_WIDTH;

               //Acknowledge the read from the FIFO, no error
               ideal_valid        <= 1'b1;
               //Fifo is not yet empty. It is going almost_empty
               ideal_empty        <= 1'b0;
               ideal_almost_empty <= 1'b1;

               ideal_rd_count     <= num_read_words_sized_i;

             //If the FIFO is one from empty
             end else if (tmp_rd_listsize/C_DEPTH_RATIO_WR == 1) begin
               //Read the value from the FIFO
               read_fifo;
               next_num_rd_bits = next_num_rd_bits - C_DOUT_WIDTH;

               //Acknowledge the read from the FIFO, no error
               ideal_valid        <= 1'b1;
               //Note that FIFO is GOING empty
               ideal_empty        <= 1'b1;
               ideal_almost_empty <= 1'b1;

               ideal_rd_count     <= num_read_words_sized_i;

             //If the FIFO is completely empty
             end else if (tmp_rd_listsize/C_DEPTH_RATIO_WR <= 0) begin
               //Do not change the contents of the FIFO

               //Do not acknowledge the read from empty FIFO
               ideal_valid        <= 1'b0;
               //Reminder that FIFO is still empty
               ideal_empty        <= 1'b1;
               ideal_almost_empty <= 1'b1;

               ideal_rd_count     <= num_read_words_sized_i;

             end // if (tmp_rd_listsize <= 0)

           end // if (ideal_empty == 1'b0)

         end else begin//(RD_EN == 1'b0)

         
           //If user did not attempt a read, do not give an ack or err
           ideal_valid           <= 1'b0;

           //Check for empty
           if (tmp_rd_listsize/C_DEPTH_RATIO_WR <= 0)
             ideal_empty         <= 1'b1;
           else
             ideal_empty         <= 1'b0;

           //Check for almost_empty
           if (tmp_rd_listsize/C_DEPTH_RATIO_WR <= 1)
             ideal_almost_empty  <= 1'b1;
           else
             ideal_almost_empty  <= 1'b0;

           ideal_rd_count        <= num_read_words_sized_i;

         end // else: !if(RD_EN == 1'b1)
       end //if (C_PRELOAD_REGS==1 && C_PRELOAD_LATENCY==0)


       /*********************************************************
        * Programmable EMPTY flags
        *********************************************************/
       //Determine the Assert and Negate thresholds for Programmable Empty
       //  (Subtract 2 read-sized words when using Preload 0)

       //Single Programmable Empty Constant Threshold
       if (C_PROG_EMPTY_TYPE==1) begin
         if (C_PRELOAD_REGS==1 && C_PRELOAD_LATENCY==0) begin
           prog_empty_actual_thresh_assert = C_PROG_EMPTY_THRESH_ASSERT_VAL-2;
           prog_empty_actual_thresh_negate = C_PROG_EMPTY_THRESH_ASSERT_VAL-2;
         end
         else begin
           prog_empty_actual_thresh_assert = C_PROG_EMPTY_THRESH_ASSERT_VAL;
           prog_empty_actual_thresh_negate = C_PROG_EMPTY_THRESH_ASSERT_VAL;
         end

       //Two Programmable Empty Constant Thresholds
       end else if (C_PROG_EMPTY_TYPE==2) begin
         if (C_PRELOAD_REGS==1 && C_PRELOAD_LATENCY==0) begin
           prog_empty_actual_thresh_assert = C_PROG_EMPTY_THRESH_ASSERT_VAL-2;
           prog_empty_actual_thresh_negate = C_PROG_EMPTY_THRESH_NEGATE_VAL-2;
         end
         else begin
           prog_empty_actual_thresh_assert = C_PROG_EMPTY_THRESH_ASSERT_VAL;
           prog_empty_actual_thresh_negate = C_PROG_EMPTY_THRESH_NEGATE_VAL;
         end

       //Single Programmable Empty Constant Threshold
       end else if (C_PROG_EMPTY_TYPE==3) begin
         if (C_PRELOAD_REGS==1 && C_PRELOAD_LATENCY==0) begin
           prog_empty_actual_thresh_assert = PROG_EMPTY_THRESH-2;
           prog_empty_actual_thresh_negate = PROG_EMPTY_THRESH-2;
         end
         else begin
           prog_empty_actual_thresh_assert = PROG_EMPTY_THRESH;
           prog_empty_actual_thresh_negate = PROG_EMPTY_THRESH;

         end
       //Two Programmable Empty Constant Thresholds
       end else if (C_PROG_EMPTY_TYPE==4) begin
         if (C_PRELOAD_REGS==1 && C_PRELOAD_LATENCY==0) begin
           prog_empty_actual_thresh_assert = PROG_EMPTY_THRESH_ASSERT-2;
           prog_empty_actual_thresh_negate = PROG_EMPTY_THRESH_NEGATE-2;
         end
         else begin
           prog_empty_actual_thresh_assert = PROG_EMPTY_THRESH_ASSERT;
           prog_empty_actual_thresh_negate = PROG_EMPTY_THRESH_NEGATE;
         end
       end

       if ((num_read_words_pe/C_DEPTH_RATIO_WR == prog_empty_actual_thresh_assert+1) 
           && RD_EN) begin
         prog_empty_d <= 1'b1;
       end else if (num_read_words_pe/C_DEPTH_RATIO_WR 
                    <= prog_empty_actual_thresh_assert) begin
         prog_empty_d <= 1'b1;
       end else if (num_read_words_pe/C_DEPTH_RATIO_WR 
                    > prog_empty_actual_thresh_negate) begin
         prog_empty_d <= 1'b0;
       end


       ideal_prog_empty <= prog_empty_d;
       num_rd_bits      <= next_num_rd_bits;
       wr_ptr_rdclk     <= wr_ptr;
     end //rd_rst_i==0
   end //always

endmodule // fifo_generator_v4_3_bhv_ver_as


/*******************************************************************************
 * Declaration of top-level module
 ******************************************************************************/
module fifo_generator_v4_3_bhv_ver_ss
  (
    CLK, RST, SRST, DIN, WR_EN, RD_EN,
    PROG_FULL_THRESH, PROG_FULL_THRESH_ASSERT, PROG_FULL_THRESH_NEGATE,
    PROG_EMPTY_THRESH, PROG_EMPTY_THRESH_ASSERT, PROG_EMPTY_THRESH_NEGATE,
    DOUT, FULL, ALMOST_FULL, WR_ACK, OVERFLOW, EMPTY,
    ALMOST_EMPTY, VALID, UNDERFLOW, DATA_COUNT,
    PROG_FULL, PROG_EMPTY
   );

/******************************************************************************
 * Declare user parameters and their defaults
 *****************************************************************************/
 parameter  C_COMMON_CLOCK                 = 0;
 parameter  C_COUNT_TYPE                   = 0;
 parameter  C_DATA_COUNT_WIDTH             = 2;
 parameter  C_DEFAULT_VALUE                = "";
 parameter  C_DIN_WIDTH                    = 8;
 parameter  C_DOUT_RST_VAL                 = "";
 parameter  C_DOUT_WIDTH                   = 8;
 parameter  C_ENABLE_RLOCS                 = 0;
 parameter  C_FAMILY                       = "virtex2"; //Not allowed in Verilog model
 parameter  C_HAS_ALMOST_EMPTY             = 0;
 parameter  C_HAS_ALMOST_FULL              = 0;
 parameter  C_HAS_BACKUP                   = 0;
 parameter  C_HAS_DATA_COUNT               = 0;
 parameter  C_HAS_MEMINIT_FILE             = 0;
 parameter  C_HAS_OVERFLOW                 = 0;
 parameter  C_HAS_RD_DATA_COUNT            = 0;
 parameter  C_HAS_RD_RST                   = 0;
 parameter  C_HAS_RST                      = 0;
 parameter  C_HAS_SRST                     = 0;
 parameter  C_HAS_UNDERFLOW                = 0;
 parameter  C_HAS_VALID                    = 0;
 parameter  C_HAS_WR_ACK                   = 0;
 parameter  C_HAS_WR_DATA_COUNT            = 0;
 parameter  C_HAS_WR_RST                   = 0;
 parameter  C_IMPLEMENTATION_TYPE          = 0;
 parameter  C_INIT_WR_PNTR_VAL             = 0;
 parameter  C_MEMORY_TYPE                  = 1;
 parameter  C_MIF_FILE_NAME                = "";
 parameter  C_OPTIMIZATION_MODE            = 0;
 parameter  C_OVERFLOW_LOW                 = 0;
 parameter  C_PRELOAD_LATENCY              = 1;
 parameter  C_PRELOAD_REGS                 = 0;
 parameter  C_PROG_EMPTY_THRESH_ASSERT_VAL = 0;
 parameter  C_PROG_EMPTY_THRESH_NEGATE_VAL = 0;
 parameter  C_PROG_EMPTY_TYPE              = 0;
 parameter  C_PROG_FULL_THRESH_ASSERT_VAL  = 0;
 parameter  C_PROG_FULL_THRESH_NEGATE_VAL  = 0;
 parameter  C_PROG_FULL_TYPE               = 0;
 parameter  C_RD_DATA_COUNT_WIDTH          = 2;
 parameter  C_RD_DEPTH                     = 256;
 parameter  C_RD_PNTR_WIDTH                = 8;
 parameter  C_UNDERFLOW_LOW                = 0;
 parameter  C_VALID_LOW                    = 0;
 parameter  C_WR_ACK_LOW                   = 0;
 parameter  C_WR_DATA_COUNT_WIDTH          = 2;
 parameter  C_WR_DEPTH                     = 256;
 parameter  C_WR_PNTR_WIDTH                = 8;
 parameter  C_WR_RESPONSE_LATENCY          = 1;
 parameter  C_FULL_FLAGS_RST_VAL           = 1;
 parameter  C_USE_EMBEDDED_REG             = 0;


/******************************************************************************
 * Declare Input and Output Ports
 *****************************************************************************/
 input                            CLK;
 input [C_DIN_WIDTH-1:0]          DIN;
 input [C_RD_PNTR_WIDTH-1:0]      PROG_EMPTY_THRESH;
 input [C_RD_PNTR_WIDTH-1:0]      PROG_EMPTY_THRESH_ASSERT;
 input [C_RD_PNTR_WIDTH-1:0]      PROG_EMPTY_THRESH_NEGATE;
 input [C_WR_PNTR_WIDTH-1:0]      PROG_FULL_THRESH;
 input [C_WR_PNTR_WIDTH-1:0]      PROG_FULL_THRESH_ASSERT;
 input [C_WR_PNTR_WIDTH-1:0]      PROG_FULL_THRESH_NEGATE;
 input                            RD_EN;
 input                            RST;
 input                            SRST;
 input                            WR_EN;
 output                           ALMOST_EMPTY;
 output                           ALMOST_FULL;
 output [C_DATA_COUNT_WIDTH-1:0]  DATA_COUNT;
 output [C_DOUT_WIDTH-1:0]        DOUT;
 output                           EMPTY;
 output                           FULL;
 output                           OVERFLOW;
 output                           PROG_EMPTY;
 output                           PROG_FULL;
 output                           VALID;
 output                           UNDERFLOW;
 output                           WR_ACK;

/*******************************************************************************
 * Input and output register declarations
 ******************************************************************************/
/*******************************************************************************
 * Parameters used as constants
 ******************************************************************************/
   //When RST is present, set FULL reset value to '1'.
   //If core has no RST, make sure FULL powers-on as '0'.
   //The reset value assignments for FULL, ALMOST_FULL, and PROG_FULL are not 
   //changed for v3.2(IP2_Im). When the core has Sync Reset, C_HAS_SRST=1 and C_HAS_RST=0.
   // Therefore, during SRST, all the FULL flags reset to 0.
   parameter                      C_HAS_FAST_FIFO = 0;
   parameter                      C_FIFO_WR_DEPTH = (C_COMMON_CLOCK) ? C_WR_DEPTH : C_WR_DEPTH - 1;
   parameter                      C_FIFO_RD_DEPTH = (C_COMMON_CLOCK) ? C_RD_DEPTH : C_RD_DEPTH - 1;

   /****************************************************************************
    * Internal Registers and wires
    ***************************************************************************/
   wire                           wr_ack_i;
   wire                           overflow_i;
   wire                           underflow_i;
   wire                           valid_i;
   wire                           valid_out;
   reg                            valid_d1;
   wire                           srst_i;
   /*******************************************************************************
    * Internal registers and wires for internal reset logics 
    ******************************************************************************/
   reg                            rst_asreg    =0;
   reg                            rst_asreg_d1 =0;
   reg                            rst_asreg_d2 =0;
   reg                            rst_reg      =0;
   reg                            rst_d1       =0;
   wire                           rst_comb;
   wire                           rst_i;


   //Memory which will be used to simulate a FIFO
   reg [C_DIN_WIDTH-1:0]          memory[C_WR_DEPTH-1:0];
   reg [31:0]                     num_bits;
   reg [31:0]                     wr_ptr;
   reg [31:0]                     rd_ptr;
   wire [31:0]                    num_read_words  = num_bits/C_DOUT_WIDTH;
   reg [31:0]                     num_read_words_q;
   wire [31:0]                    num_write_words = num_bits/C_DIN_WIDTH;
   reg [31:0]                     num_write_words_q;
   //Removed power_on_timer in v3.2 (IP2_Im). For all reset types (Async, Sync, or no reset), the power-on values of the flags in the core are modified so that the core is ready to use from the very first clock cycle.
  //reg [3:0]                    power_on_timer;

   //Special ideal FIFO signals
   reg [C_DOUT_WIDTH-1:0]         ideal_dout;
   reg [C_DOUT_WIDTH-1:0]         ideal_dout_d1;
   wire [C_DOUT_WIDTH-1:0]        ideal_dout_out;
   reg                            ideal_wr_ack;
   reg                            ideal_valid;
   reg                            ideal_overflow;
   reg                            ideal_underflow;
   reg                            ideal_full;
   reg                            ideal_empty;
   reg                            ideal_almost_full;
   reg                            ideal_almost_empty;
   reg                            ideal_prog_full;
   reg                            ideal_prog_empty;


   //MSBs of the counts
   wire [C_DATA_COUNT_WIDTH-1:0]  ideal_d_count;

   //user specified value for reseting the size of the fifo
   reg [C_DOUT_WIDTH-1:0]          dout_reset_val;


   //temporary registers for WR_RESPONSE_LATENCY feature
   reg                             ideal_wr_ack_q;
   reg                             ideal_overflow_q;

   reg                             prog_full_d;
   reg                             prog_empty_d;

   //Delayed version of RST
   reg                             rst_q;
   reg                             rst_qq;

   /****************************************************************************
    * Function Declarations
    ***************************************************************************/
   task write_fifo;
      begin
         memory[wr_ptr]     <= DIN;
         if (wr_ptr == 0) begin
            wr_ptr          <= C_WR_DEPTH - 1;
         end else begin
            wr_ptr          <= wr_ptr - 1;
         end
      end
   endtask // write_fifo

   task  read_fifo;
      begin
         ideal_dout <= memory[rd_ptr];
         if (rd_ptr == 0) begin
            rd_ptr  <= C_RD_DEPTH - 1;
         end else begin
            rd_ptr  <= rd_ptr - 1;
         end
      end
   endtask

   /****************************************************************************
    * log2_val
    *   Returns the 'log2' value for the input value for the supported ratios
    ***************************************************************************/
    function [31:0] log2_val;
    input [31:0] binary_val;

    begin
       if (binary_val == 8) begin
          log2_val = 3;
       end else if (binary_val == 4) begin
          log2_val = 2;
       end else begin
          log2_val = 1;
       end
    end
    endfunction

   /****************************************************************************
    * hexstr_conv
    *   Converts a string of type hex to a binary value (for C_DOUT_RST_VAL)
    ***************************************************************************/
    function [C_DOUT_WIDTH-1:0] hexstr_conv;
    input [(C_DOUT_WIDTH*8)-1:0] def_data;

    integer index,i,j;
    reg [3:0] bin;

    begin
      index = 0;
      hexstr_conv = 'b0;
      for( i=C_DOUT_WIDTH-1; i>=0; i=i-1 )
      begin
        case (def_data[7:0])
          8'b00000000 :
          begin
            bin = 4'b0000;
            i = -1;
          end
          8'b00110000 : bin = 4'b0000;
          8'b00110001 : bin = 4'b0001;
          8'b00110010 : bin = 4'b0010;
          8'b00110011 : bin = 4'b0011;
          8'b00110100 : bin = 4'b0100;
          8'b00110101 : bin = 4'b0101;
          8'b00110110 : bin = 4'b0110;
          8'b00110111 : bin = 4'b0111;
          8'b00111000 : bin = 4'b1000;
          8'b00111001 : bin = 4'b1001;
          8'b01000001 : bin = 4'b1010;
          8'b01000010 : bin = 4'b1011;
          8'b01000011 : bin = 4'b1100;
          8'b01000100 : bin = 4'b1101;
          8'b01000101 : bin = 4'b1110;
          8'b01000110 : bin = 4'b1111;
          8'b01100001 : bin = 4'b1010;
          8'b01100010 : bin = 4'b1011;
          8'b01100011 : bin = 4'b1100;
          8'b01100100 : bin = 4'b1101;
          8'b01100101 : bin = 4'b1110;
          8'b01100110 : bin = 4'b1111;
          default :
          begin
            bin = 4'bx;
          end
        endcase
        for( j=0; j<4; j=j+1)
        begin
          if ((index*4)+j < C_DOUT_WIDTH)
          begin
            hexstr_conv[(index*4)+j] = bin[j];
          end
        end
        index = index + 1;
        def_data = def_data >> 8;
      end
    end
  endfunction

  /*****************************************************************************
   * Initialize Signals
   ****************************************************************************/
   initial begin
      num_bits           = 0;
      num_read_words_q   = 0;
      num_write_words_q  = 0;
      rd_ptr             = C_RD_DEPTH -1;
      wr_ptr             = C_WR_DEPTH -1;
      dout_reset_val     = hexstr_conv(C_DOUT_RST_VAL);
      ideal_dout       = dout_reset_val;
      ideal_wr_ack       = 1'b0;
      ideal_valid        = 1'b0;
      valid_d1           = 1'b0;
      ideal_overflow     = 1'b0;
      ideal_underflow    = 1'b0;
      //Modified the start-up value of FULL to '0' in v3.2 (IP2_Im)
      //ideal_full         = C_FULL_RESET_VAL; //was in v3.1
      ideal_full         = 1'b0; //v3.2
      ideal_empty        = 1'b1;
      //Modified the start-up value of ALMOST_FULL to '0' in v3.2 (IP2_Im)
      //ideal_almost_full  = C_ALMOST_FULL_RESET_VAL; //was in v3.1
      ideal_almost_full  = 1'b0;
      ideal_almost_empty = 1'b1;
      //Modified the start-up value of PROG_FULL to '0' in v3.2 (IP2_Im)
      //ideal_prog_full    = C_PROG_FULL_RESET_VAL; //was in v3.1
      ideal_prog_full    = 1'b0; //v3.2
      ideal_prog_empty   = 1'b1;

      //Modified the start-up value of PROG_FULL to '0' in v3.2 (IP2_Im)
      //Therefore, prog_full_d is also changed
      //prog_full_d        = C_PROG_FULL_RESET_VAL; //was in v3.1
      prog_full_d        = 1'b0; //v3.2
      prog_empty_d       = 1'b1;

      //Removed in v3.2
      //power_on_timer     = C_HAS_RST ? 4'h3 : 4'h0;
  
      //Added these initial values in v3.2 to make it consistent with the synchronization flop stages in the core.
      rst_q              = 1'b0;
      rst_qq             = 1'b0;    
   end


  /*****************************************************************************
   * Assign Internal ideal signals to output ports
   ****************************************************************************/
  assign ideal_dout_out= (C_USE_EMBEDDED_REG==1 &&
                          (C_MEMORY_TYPE==0 || C_MEMORY_TYPE==1))?
                         ideal_dout_d1: ideal_dout; 
  assign DOUT          = ideal_dout_out;
  //was in v3.1
  //assign FULL          = (power_on_timer) ? C_FULL_RESET_VAL : ideal_full;
  //v3.2
  assign FULL          = ideal_full;

  assign EMPTY         = ideal_empty;
  //was in v3.1
  //assign ALMOST_FULL   = (power_on_timer) ? C_ALMOST_FULL_RESET_VAL : ideal_almost_full;
  //v3.2
  assign ALMOST_FULL   = ideal_almost_full;

  assign ALMOST_EMPTY  = ideal_almost_empty;

  assign ideal_d_count = num_read_words[C_RD_PNTR_WIDTH-1:C_RD_PNTR_WIDTH-C_DATA_COUNT_WIDTH];
  assign DATA_COUNT    = ideal_d_count;

  //was in v3.1
  //assign PROG_FULL     = (power_on_timer) ? C_PROG_FULL_RESET_VAL : ideal_prog_full;
  //v3.2
  assign PROG_FULL     = ideal_prog_full;

  assign PROG_EMPTY    = ideal_prog_empty;

  //Handshaking signals can be active low, depending on _LOW parameters
  assign valid_i       = (C_PRELOAD_LATENCY==0) ? (RD_EN & ~EMPTY) : ideal_valid;
  assign VALID         = valid_out ? !C_VALID_LOW : C_VALID_LOW;
  assign valid_out     = (C_PRELOAD_LATENCY==2 &&
                          (C_MEMORY_TYPE==0 || C_MEMORY_TYPE==1))?
                         valid_d1: valid_i; 
  assign underflow_i   = (C_PRELOAD_LATENCY==0) ? (RD_EN & EMPTY) : ideal_underflow;
  assign UNDERFLOW     = underflow_i ? !C_UNDERFLOW_LOW : C_UNDERFLOW_LOW;

  assign WR_ACK        = wr_ack_i ? !C_WR_ACK_LOW : C_WR_ACK_LOW;
  assign wr_ack_i      = (C_WR_RESPONSE_LATENCY==2) ? ideal_wr_ack_q :
                         (C_WR_RESPONSE_LATENCY==1) ? ideal_wr_ack :
                         (WR_EN & !FULL);
  assign OVERFLOW      = overflow_i ? !C_OVERFLOW_LOW : C_OVERFLOW_LOW;
  assign overflow_i    = (C_WR_RESPONSE_LATENCY==2) ? ideal_overflow_q :
                         (C_WR_RESPONSE_LATENCY==1) ? ideal_overflow :
                         (WR_EN & FULL);

  assign srst_i         = C_HAS_SRST ? SRST : 0;

  /*******************************************************************************
   * Internal reset logics 
   ******************************************************************************/
  assign rst_comb      = !rst_asreg_d2 && rst_asreg;
  assign rst_i         = C_HAS_RST ? rst_reg : 0;

  always @(posedge CLK or posedge rst_i) begin
    if (rst_i == 1'b1) begin
      valid_d1 <= 1'b0;
    end else begin
      if (srst_i) begin
        valid_d1 <= 1'b0;
      end else begin
        valid_d1 <= valid_i;
      end
    end    
  end   
  always @(posedge CLK or posedge rst_i) begin
    if (rst_i == 1'b1) begin
      ideal_dout_d1 <= dout_reset_val;
    end else begin
      if (srst_i) begin
        ideal_dout_d1 <= dout_reset_val;
      end else begin
        ideal_dout_d1 <= ideal_dout;
      end
    end    
  end   

  always @(posedge CLK or posedge RST) begin
     if (RST == 1'b1) begin
       rst_asreg <= 1'b1;
     end else begin
       if (rst_asreg_d1 == 1'b1) begin
         rst_asreg <= 1'b0;
       end else begin
         rst_asreg <= rst_asreg;
       end
     end    
  end   

  always @(posedge CLK) begin
     rst_asreg_d1 <= rst_asreg;
     rst_asreg_d2 <= rst_asreg_d1;
  end
  
  always @(posedge CLK or posedge rst_comb) begin
     if (rst_comb == 1'b1) begin
       rst_reg <= 1'b1;
     end else begin
       rst_reg <= 1'b0;
     end    
  end   

  
  /*******************************************************************************
   * Write and Read Logics 
   ******************************************************************************/

  always @(posedge CLK or posedge rst_i)
   begin : gen_wr_ack_resp

       //Register reset
       rst_q   <= rst_i;
       rst_qq  <= rst_q;

       //Register output signals to achieve desired WR_RESPONSE latency
       if (C_WR_RESPONSE_LATENCY == 2) begin
          if (rst_i == 1) begin
            ideal_wr_ack_q   <= 0;
            ideal_overflow_q <= 0;
          end else begin
            ideal_wr_ack_q   <= ideal_wr_ack;
            ideal_overflow_q <= ideal_overflow;
          end
       end
  
      //Removed in v3.2 
      /*
      if (rst_i == 1) begin
         power_on_timer <= 0;
      end else if (power_on_timer > 0) begin
         power_on_timer <= power_on_timer -1;
      end else begin
         power_on_timer <= 0;
      end
      */
    end // block: gen_wr_ack_resp

   // block memory has a synchronous reset
   always @(posedge CLK) begin : gen_fifo_blkmemdout
      //Changed the latency of during async reset to '1' instead of '2' to make it consistent with the core.
      //if (rst_i || rst_q || rst_qq) begin //was in v3.1
      if (rst_i || rst_q || srst_i) begin //v3.2
         /******Initialize Read Domain Signals************************************/
         if (C_MEMORY_TYPE == 1) begin
            ideal_dout <= dout_reset_val;
         end
     //v3.2
     //end else begin
         //if (C_MEMORY_TYPE == 1 && power_on_timer >= 2) begin //was in v3.1
     //    if (C_MEMORY_TYPE == 1) begin //v3.2
     //       ideal_dout <= dout_reset_val;
     //    end
     end
   end

   always @(posedge CLK or posedge rst_i) begin : gen_fifo

      /****** Reset fifo - Asynchronous Reset*************************************/
      //Changed the latency of during async reset to '1' instead of '2' to make it consistent with the core.
      //if (rst_i || rst_q || rst_qq) begin //was in v3.1
      //if (rst_i || rst_q) begin //v3.2
      if (rst_i ) begin //v3.2
         /******Initialize Generic FIFO constructs********************************/
         num_bits           <= 0;
         wr_ptr             <= C_WR_DEPTH - 1;
         rd_ptr             <= C_RD_DEPTH - 1;
         num_read_words_q   <= 0;
         num_write_words_q  <= 0;


         /******Initialize Write Domain Signals***********************************/
         ideal_wr_ack       <= 0;
         ideal_full         <= C_FULL_FLAGS_RST_VAL;
         ideal_almost_full  <= C_FULL_FLAGS_RST_VAL;

         /******Initialize Read Domain Signals************************************/
         if (C_MEMORY_TYPE != 1) begin
            ideal_dout      <= dout_reset_val;
         end
         ideal_valid        <= 1'b0;
         ideal_empty        <= 1'b1;
         ideal_almost_empty <= 1'b1;

      end else begin   
         if (srst_i) begin
            // SRST is available only for Sync BRAM and Sync DRAM. Not for SSHFT.
            if (C_MEMORY_TYPE == 1 || C_MEMORY_TYPE == 2) begin
 	       /******Initialize Generic FIFO constructs********************************/
               num_bits           <= 0;
               wr_ptr             <= C_WR_DEPTH - 1;
               rd_ptr             <= C_RD_DEPTH - 1;
               num_read_words_q   <= 0;
               num_write_words_q  <= 0;

               /******Initialize Write Domain Signals***********************************/
               ideal_wr_ack       <= 0;
               ideal_full         <= 0; //'0'
               ideal_almost_full  <= 0; //'0'

               /******Initialize Read Domain Signals************************************/
               //Reset DOUT of Sync DRAM. Sync BRAM DOUT was reset in the above always block.
	       if (C_MEMORY_TYPE == 2) begin  
                  ideal_dout      <= dout_reset_val;
               end
               ideal_valid        <= 1'b0;
               ideal_empty        <= 1'b1;
               ideal_almost_empty <= 1'b1;
            end

         end else begin  //normal operating conditions
         /**********************************************************************/
         // Synchronous FIFO Condition #1 : Writing and not reading
         /**********************************************************************/
         if (WR_EN & ~RD_EN) begin

            /*********************************/
            //If the FIFO is full, do NOT perform the write,
            // update flags accordingly
            /*********************************/
            if (num_write_words >= C_FIFO_WR_DEPTH) begin
               ideal_wr_ack       <= 0;

               //still full
               ideal_full         <= 1'b1;
               ideal_almost_full  <= 1'b1;

               //write unsuccessful - do not change contents

               // no read attempted
               ideal_valid        <= 1'b0;

               //Not near empty
               ideal_empty        <= 1'b0;
               ideal_almost_empty <= 1'b0;


            /*********************************/
            //If the FIFO is reporting FULL
            // (Startup condition)
            /*********************************/
            end else if ((num_write_words < C_FIFO_WR_DEPTH) && (ideal_full == 1'b1)) begin
               ideal_wr_ack       <= 0;

               //still full
               ideal_full         <= 1'b0;
               ideal_almost_full  <= 1'b0;

               //write unsuccessful - do not change contents

               // no read attempted
               ideal_valid        <= 1'b0;

               //FIFO EMPTY in this state can not be determined
               //ideal_empty        <= 1'b0;
               //ideal_almost_empty <= 1'b0;


               /*********************************/
               //If the FIFO is one from full
               /*********************************/
            end else if (num_write_words == C_FIFO_WR_DEPTH-1) begin
               //good write
               ideal_wr_ack       <= 1;

               //FIFO is one from FULL and going FULL
               ideal_full         <= 1'b1;
               ideal_almost_full  <= 1'b1;

               //Add input data
               write_fifo;

               // no read attempted
               ideal_valid        <= 1'b0;

               //Not near empty
               ideal_empty        <= 1'b0;
               ideal_almost_empty <= 1'b0;

               num_bits           <= num_bits + C_DIN_WIDTH;

               /*********************************/
               //If the FIFO is 2 from full
               /*********************************/
            end else if (num_write_words == C_FIFO_WR_DEPTH-2) begin
               //good write
               ideal_wr_ack       <= 1;

               //2 from full, and writing, so set almost_full
               ideal_full         <= 1'b0;
               ideal_almost_full  <= 1'b1;

               //Add input data
               write_fifo;

               //no read attempted
               ideal_valid        <= 1'b0;

               //Not near empty
               ideal_empty        <= 1'b0;
               ideal_almost_empty <= 1'b0;

               num_bits           <= num_bits + C_DIN_WIDTH;

               /*********************************/
               //If the FIFO is ALMOST EMPTY
               /*********************************/
            end else if (num_read_words == 1) begin
               //good write
               ideal_wr_ack       <= 1;

               //Not near FULL
               ideal_full         <= 1'b0;
               ideal_almost_full  <= 1'b0;

               //Add input data
               write_fifo;

               // no read attempted
               ideal_valid        <= 1'b0;

               //Leaving ALMOST_EMPTY
               ideal_empty        <= 1'b0;
               ideal_almost_empty <= 1'b0;

               num_bits           <= num_bits + C_DIN_WIDTH;

               /*********************************/
               //If the FIFO is EMPTY
               /*********************************/
            end else if (num_read_words == 0) begin
               // good write
               ideal_wr_ack       <= 1;

               //Not near FULL
               ideal_full         <= 1'b0;
               ideal_almost_full  <= 1'b0;

               //Add input data
               write_fifo;

               // no read attempted
               ideal_valid        <= 1'b0;

               //Leaving EMPTY (still ALMOST_EMPTY)
               ideal_empty        <= 1'b0;
               ideal_almost_empty <= 1'b1;

               num_bits           <= num_bits + C_DIN_WIDTH;

               /*********************************/
               //If the FIFO is not near EMPTY or FULL
               /*********************************/
            end else begin
               // good write
               ideal_wr_ack       <= 1;

               //Not near FULL
               ideal_full         <= 1'b0;
               ideal_almost_full  <= 1'b0;

               //Add input data
               write_fifo;

               // no read attempted
               ideal_valid        <= 1'b0;

               //Not near EMPTY
               ideal_empty        <= 1'b0;
               ideal_almost_empty <= 1'b0;

               num_bits           <= num_bits + C_DIN_WIDTH;

            end // average case


            /**********************************************************************/
            // Synchronous FIFO Condition #2 : Reading and not writing
            /**********************************************************************/
         end else if (~WR_EN & RD_EN) begin

            /*********************************/
            //If the FIFO is EMPTY
            /*********************************/
            if ((num_read_words == 0) || (ideal_empty == 1'b1)) begin
               //no write attemped
               ideal_wr_ack       <= 0;

               //FIFO is not near FULL
               ideal_full         <= 1'b0;
               ideal_almost_full  <= 1'b0;

               //Read will fail
               ideal_valid        <= 1'b0;

               //FIFO is still empty
               ideal_empty        <= 1'b1;
               ideal_almost_empty <= 1'b1;

               //No read

               /*********************************/
               //If the FIFO is ALMOST EMPTY
               /*********************************/
            end else if (num_read_words == 1) begin
               //no write attempted
               ideal_wr_ack       <= 0;

               //FIFO is not near FULL
               ideal_full         <= 1'b0;
               ideal_almost_full  <= 1'b0;

               //Read successful
               ideal_valid        <= 1'b1;

               //This read will make FIFO go empty
               ideal_empty        <= 1'b1;
               ideal_almost_empty <= 1'b1;

               //Get the data from the FIFO
               read_fifo;
               num_bits           <= num_bits - C_DIN_WIDTH;


               /*********************************/
               //If the FIFO is 2 from EMPTY
               /*********************************/
            end else if (num_read_words == 2) begin

               //no write attempted
               ideal_wr_ack       <= 0;

               //FIFO is not near FULL
               ideal_full         <= 1'b0;
               ideal_almost_full  <= 1'b0;

               //Read successful
               ideal_valid        <= 1'b1;

               //FIFO is going ALMOST_EMPTY
               ideal_empty        <= 1'b0;
               ideal_almost_empty <= 1'b1;

               //Get the data from the FIFO
               read_fifo;
               num_bits           <= num_bits - C_DOUT_WIDTH;



               /*********************************/
               //If the FIFO is one from full
               /*********************************/
            end else if (num_write_words == C_FIFO_WR_DEPTH-1) begin

               //no write attempted
               ideal_wr_ack       <= 0;

               //FIFO is leaving ALMOST FULL
               ideal_full         <= 1'b0;
               ideal_almost_full  <= 1'b0;

               //Read successful
               ideal_valid        <= 1'b1;

               //Not near empty
               ideal_empty        <= 1'b0;
               ideal_almost_empty <= 1'b0;

               //Read from the FIFO
               read_fifo;
               num_bits           <= num_bits - C_DOUT_WIDTH;


               /*********************************/
               // FIFO is FULL
               /*********************************/
            end else if (num_write_words >= C_FIFO_WR_DEPTH)
              begin
                 //no write attempted
                 ideal_wr_ack       <= 0;

                 //FIFO is leaving FULL, but is still ALMOST_FULL
                 ideal_full         <= 1'b0;
                 ideal_almost_full  <= 1'b1;

                 //Read successful
                 ideal_valid        <= 1'b1;

                 //Not near empty
                 ideal_empty        <= 1'b0;
                 ideal_almost_empty <= 1'b0;

                 //Read from the FIFO
                 read_fifo;
                 num_bits           <= num_bits - C_DOUT_WIDTH;

                 /*********************************/
                 //If the FIFO is not near EMPTY or FULL
                 /*********************************/
              end else begin
                 //no write attemped
                 ideal_wr_ack       <= 0;

                 //Not near empty
                 ideal_full         <= 1'b0;
                 ideal_almost_full  <= 1'b0;

                 //Read successful
                 ideal_valid        <= 1'b1;

                 //Not near empty
                 ideal_empty        <= 1'b0;
                 ideal_almost_empty <= 1'b0;

                 //Read from the FIFO
                 read_fifo;
                 num_bits           <= num_bits - C_DOUT_WIDTH;


              end // average read


            /**********************************************************************/
            // Synchronous FIFO Condition #3 : Reading and writing
            /**********************************************************************/
         end else if (WR_EN & RD_EN) begin

            /*********************************/
            // FIFO is FULL
            /*********************************/
            if (num_write_words >= C_FIFO_WR_DEPTH) begin

               ideal_wr_ack       <= 0;

               //Read will be successful, so FIFO will leave FULL
               ideal_full         <= 1'b0;
               ideal_almost_full  <= 1'b1;

               //Read successful
               ideal_valid        <= 1'b1;

               //Not near empty
               ideal_empty        <= 1'b0;
               ideal_almost_empty <= 1'b0;

               //Read from the FIFO
               read_fifo;
               num_bits           <= num_bits - C_DOUT_WIDTH;


            /*********************************/
            // FIFO is reporting FULL, but it is empty
            // (This is a special case, when coming out of RST
            /*********************************/
            end else if ((num_write_words == 0) && (ideal_full == 1'b1)) begin

               ideal_wr_ack       <= 0;

               //Read will be successful, so FIFO will leave FULL
               ideal_full         <= 1'b0;
               ideal_almost_full  <= 1'b0;

               //Read unsuccessful
               ideal_valid        <= 1'b0;

               //Report empty condition
               ideal_empty        <= 1'b1;
               ideal_almost_empty <= 1'b1;

               //Do not read from empty FIFO
               // Read from the FIFO


               /*********************************/
               //If the FIFO is one from full
               /*********************************/
            end else if (num_write_words == C_FIFO_WR_DEPTH-1) begin

               //Write successful
               ideal_wr_ack       <= 1;

               //FIFO will remain ALMOST_FULL
               ideal_full         <= 1'b0;
               ideal_almost_full  <= 1'b1;

               // put the data into the FIFO
               write_fifo;

               //Read successful
               ideal_valid        <= 1'b1;

               //Not near empty
               ideal_empty        <= 1'b0;
               ideal_almost_empty <= 1'b0;

               //Read from the FIFO
               read_fifo;
               num_bits           <= num_bits + C_DIN_WIDTH - C_DOUT_WIDTH;

               /*********************************/
               //If the FIFO is ALMOST EMPTY
               /*********************************/
            end else if (num_read_words == 1) begin

               //Write successful
               ideal_wr_ack       <= 1;

               // Not near FULL
               ideal_full         <= 1'b0;
               ideal_almost_full  <= 1'b0;

               // put the data into the FIFO
               write_fifo;

               //Read successful
               ideal_valid        <= 1'b1;

               //FIFO will stay ALMOST_EMPTY
               ideal_empty        <= 1'b0;
               ideal_almost_empty <= 1'b1;

               //Read from the FIFO
               read_fifo;
               num_bits           <= num_bits + C_DIN_WIDTH - C_DOUT_WIDTH;


               /*********************************/
               //If the FIFO is EMPTY
               /*********************************/
            end else if (num_read_words == 0) begin

               //Write successful
               ideal_wr_ack       <= 1;

               // Not near FULL
               ideal_full         <= 1'b0;
               ideal_almost_full  <= 1'b0;

               // put the data into the FIFO
               write_fifo;

               //Read will fail
               ideal_valid        <= 1'b0;

               //FIFO will leave EMPTY
               ideal_empty        <= 1'b0;
               ideal_almost_empty <= 1'b1;

               // No read
               num_bits           <= num_bits + C_DIN_WIDTH;


               /*********************************/
               //If the FIFO is not near EMPTY or FULL
               /*********************************/
            end else begin

               //Write successful
               ideal_wr_ack       <= 1;

               // Not near FULL
               ideal_full         <= 1'b0;
               ideal_almost_full  <= 1'b0;

               // put the data into the FIFO
               write_fifo;

               //Read successful
               ideal_valid        <= 1'b1;

               // Not near EMPTY
               ideal_empty        <= 1'b0;
               ideal_almost_empty <= 1'b0;

               //Read from the FIFO
               read_fifo;
               num_bits           <= num_bits + C_DIN_WIDTH - C_DOUT_WIDTH;

            end // average case

            /**********************************************************************/
            // Synchronous FIFO Condition #4 : Not reading or writing
            /***
             *******************************************************************/
         end else begin

            /*********************************/
            // FIFO is FULL
            /*********************************/
            if (num_write_words >= C_FIFO_WR_DEPTH) begin

               //No write
               ideal_wr_ack       <= 0;
               ideal_full         <= 1'b1;
               ideal_almost_full  <= 1'b1;

               //No read
               ideal_valid        <= 1'b0;
               ideal_empty        <= 1'b0;
               ideal_almost_empty <= 1'b0;

               //No change to memory

               /*********************************/
               //If the FIFO is one from full
               /*********************************/
            end else if (num_write_words == C_FIFO_WR_DEPTH-1) begin

               //No write
               ideal_wr_ack       <= 0;
               ideal_full         <= 1'b0;
               ideal_almost_full  <= 1'b1;

               //No read
               ideal_valid        <= 1'b0;
               ideal_empty        <= 1'b0;
               ideal_almost_empty <= 1'b0;

               //No change to memory

               /*********************************/
               //If the FIFO is ALMOST EMPTY
               /*********************************/
            end else if (num_read_words == 1) begin
               //No write
               ideal_wr_ack       <= 0;
               ideal_full         <= 1'b0;
               ideal_almost_full  <= 1'b0;

               //No read
               ideal_valid        <= 1'b0;
               ideal_empty        <= 1'b0;
               ideal_almost_empty <= 1'b1;

               //No change to memory

            end // almost empty


            /*********************************/
            //If the FIFO is EMPTY
            /*********************************/
            else if (num_read_words == 0)
              begin
                 //No write
                 ideal_wr_ack       <= 0;
                 ideal_full         <= 1'b0;
                 ideal_almost_full  <= 1'b0;

                 //No read
                 ideal_valid        <= 1'b0;
                 ideal_empty        <= 1'b1;
                 ideal_almost_empty <= 1'b1;

                 //No change to memory

                 /*********************************/
                 //If the FIFO is not near EMPTY or FULL
                 /*********************************/
              end else begin

                 //No write
                 ideal_wr_ack       <= 0;
                 ideal_full         <= 1'b0;
                 ideal_almost_full  <= 1'b0;

                 //No read
                 ideal_valid        <= 1'b0;
                 ideal_empty        <= 1'b0;
                 ideal_almost_empty <= 1'b0;

                 //No change to memory

              end //    average case

         end // neither reading or writing

         num_read_words_q  <= num_read_words;
         num_write_words_q <= num_write_words;
        
         end //normal operating conditions
      end 

   end // block: gen_fifo

   //Generate overflow and underflow flags seperately
   //because they don't support async rst
   always @(posedge CLK) begin
     ideal_overflow    <= WR_EN & ideal_full;
     ideal_underflow    <= ideal_empty & RD_EN;
   end
   
   always @(posedge CLK or posedge rst_i) begin : gen_fifo_p

      /****** Reset fifo - Async Reset****************************************/
      //The latency of de-assertion of the flags is reduced by 1 to be consistent with the core.
      //if (rst_i || rst_q) begin //was in v3.1 
      if (rst_i) begin //v3.2 
         ideal_prog_full   <= C_FULL_FLAGS_RST_VAL;
         ideal_prog_empty  <= 1'b1;
         prog_full_d       <= C_FULL_FLAGS_RST_VAL;
         prog_empty_d      <= 1'b1;

      end else begin
         if (srst_i) begin
            //SRST is available only for Sync BRAM and Sync DRAM. Not for SSHFT.
            if (C_MEMORY_TYPE == 1 || C_MEMORY_TYPE == 2) begin
               ideal_prog_full   <= 1'b0;
               ideal_prog_empty  <= 1'b1;
               prog_full_d       <= 1'b0;
               prog_empty_d      <= 1'b1;
            end
         end else begin 

	        /*****************************************************************
            * Programmable FULL flags
            ****************************************************************/
            //Single constant threshold
            if (C_PROG_FULL_TYPE == 1) begin
               if ((num_write_words >= C_PROG_FULL_THRESH_ASSERT_VAL-1)
                             && WR_EN && !RD_EN) begin
                  prog_full_d <= 1'b1;
               end else if (((num_write_words == C_PROG_FULL_THRESH_ASSERT_VAL)
                    && RD_EN && !WR_EN) || (rst_q && !rst_i)) begin //v3.2
	              prog_full_d <= 1'b0;
               end

            //Dual constant thresholds
            end else if (C_PROG_FULL_TYPE == 2) begin
               if ((num_write_words == C_PROG_FULL_THRESH_ASSERT_VAL-1)
                   && WR_EN && !RD_EN) begin
                  prog_full_d <= 1'b1;
               end else if ((num_write_words == C_PROG_FULL_THRESH_NEGATE_VAL)
                            && RD_EN && !WR_EN) begin
                  prog_full_d <= 1'b0;
               end

            //Single input threshold
            end else if (C_PROG_FULL_TYPE == 3) begin
              if ((num_write_words == PROG_FULL_THRESH-1)
                    && WR_EN && !RD_EN) begin
                  prog_full_d <= 1'b1;
              end else if ((num_write_words == PROG_FULL_THRESH)
                    && !WR_EN && RD_EN) begin
                   prog_full_d <= 1'b0;
              end else if (num_write_words >= PROG_FULL_THRESH) begin
                  prog_full_d <= 1'b1;
              end else if (num_write_words < PROG_FULL_THRESH) begin
                   prog_full_d <= 1'b0;
              end

            //Dual input thresholds
            end else begin
              if ((num_write_words == PROG_FULL_THRESH_ASSERT-1)
                    && WR_EN && !RD_EN) begin
                  prog_full_d <= 1'b1;
              end else if ((num_write_words == PROG_FULL_THRESH_NEGATE)
                    && !WR_EN && RD_EN)begin
                  prog_full_d <= 1'b0;
              end else if (num_write_words >= PROG_FULL_THRESH_ASSERT) begin
                  prog_full_d <= 1'b1;
              end else if (num_write_words < PROG_FULL_THRESH_NEGATE) begin 
                  prog_full_d <= 1'b0;
               end
            end

            /*****************************************************************
             * Programmable EMPTY flags
             ****************************************************************/
            //Single constant threshold
            if (C_PROG_EMPTY_TYPE == 1) begin
               if ((num_read_words == C_PROG_EMPTY_THRESH_ASSERT_VAL+1)
                   && RD_EN && !WR_EN) begin
                  prog_empty_d <= 1'b1;
               end else if ((num_read_words == C_PROG_EMPTY_THRESH_ASSERT_VAL)
                   && WR_EN && !RD_EN) begin
                  prog_empty_d <= 1'b0;
               end
            //Dual constant thresholds
            end else if (C_PROG_EMPTY_TYPE == 2) begin
               if ((num_read_words == C_PROG_EMPTY_THRESH_ASSERT_VAL+1)
                   && RD_EN && !WR_EN) begin
                  prog_empty_d <= 1'b1;
               end else if ((num_read_words == C_PROG_EMPTY_THRESH_NEGATE_VAL)
                            && !RD_EN && WR_EN) begin
                  prog_empty_d <= 1'b0;
               end

            //Single input threshold
            end else if (C_PROG_EMPTY_TYPE == 3) begin
               if ((num_read_words == PROG_EMPTY_THRESH+1)
                             && RD_EN && !WR_EN) begin
                  prog_empty_d <= 1'b1;
               end else if ((num_read_words == PROG_EMPTY_THRESH)
                             && !RD_EN && WR_EN) begin
                  prog_empty_d <= 1'b0;
               end else if (num_read_words <= PROG_EMPTY_THRESH) begin
                  prog_empty_d <= 1'b1;
               end else if (num_read_words > PROG_EMPTY_THRESH)begin
                  prog_empty_d <= 1'b0;
               end

            //Dual input thresholds
            end else begin
               if (num_read_words <= PROG_EMPTY_THRESH_ASSERT) begin 
                  prog_empty_d <= 1'b1;
               end else if ((num_read_words == PROG_EMPTY_THRESH_ASSERT+1)
                   && RD_EN && !WR_EN) begin
                  prog_empty_d <= 1'b1;
               end else if (num_read_words > PROG_EMPTY_THRESH_NEGATE)begin
                  prog_empty_d <= 1'b0;
               end else if ((num_read_words == PROG_EMPTY_THRESH_NEGATE)
                   && !RD_EN && WR_EN) begin
                  prog_empty_d <= 1'b0;
               end
            end

            ideal_prog_empty  <= prog_empty_d;
            if (rst_q && !rst_i) begin
              ideal_prog_full   <= 1'b0;
            end else begin
              ideal_prog_full   <= prog_full_d;
            end

         end //if (srst_i) begin
      end //if (rst_i) begin
   end //always @(posedge CLK or posedge rst_i) begin : gen_fifo_p
endmodule // fifo_generator_v4_3_bhv_ver_ss


module fifo_generator_v4_3_bhv_ver_preload0
 (
  RD_CLK,
  RD_RST,
  RD_EN,
  FIFOEMPTY,
  FIFODATA,
  USERDATA,
  USERVALID,
  USERUNDERFLOW,
  USEREMPTY,
  USERALMOSTEMPTY,
  RAMVALID,
  FIFORDEN
  );


 parameter  C_DOUT_RST_VAL            = "";
 parameter  C_DOUT_WIDTH              = 8;
 parameter  C_HAS_RST                 = 0;
 parameter  C_USERVALID_LOW           = 0;
 parameter  C_USERUNDERFLOW_LOW       = 0;


 input                     RD_CLK;
 input                     RD_RST;
 input                     RD_EN;
 input                     FIFOEMPTY;
 input  [C_DOUT_WIDTH-1:0] FIFODATA;
 output [C_DOUT_WIDTH-1:0] USERDATA;
 output                    USERVALID;
 output                    USERUNDERFLOW;
 output                    USEREMPTY;
 output                    USERALMOSTEMPTY;
 output                    RAMVALID;
 output                    FIFORDEN;

 wire                      RD_CLK;
 wire                      RD_RST;
 wire                      RD_EN;
 wire                      FIFOEMPTY;
 wire [C_DOUT_WIDTH-1:0]   FIFODATA;
 reg [C_DOUT_WIDTH-1:0]    USERDATA;
 wire                      USERVALID;
 wire                      USERUNDERFLOW;
 wire                      USEREMPTY;
 wire                      USERALMOSTEMPTY;
 wire                      RAMVALID;
 wire                      FIFORDEN;

 wire                      preloadstage1;
 wire                      preloadstage2;
 reg                       ram_valid_i;
 reg                       read_data_valid_i;
 wire                      ram_regout_en;
 wire                      ram_rd_en;
 reg                       empty_i        = 1'b1;
 reg                       empty_q        = 1'b1;
 reg                       rd_en_q        = 1'b0; //Fix for CR:236270 in v3.2 //prasanna
 reg                       almost_empty_i = 1'b1;
 reg                       almost_empty_q = 1'b1;
 wire rd_rst_i;


/*************************************************************************
* FUNCTIONS
*************************************************************************/

   /*************************************************************************
    * hexstr_conv
    *   Converts a string of type hex to a binary value (for C_DOUT_RST_VAL)
    ***********************************************************************/
    function [C_DOUT_WIDTH-1:0] hexstr_conv;
    input [(C_DOUT_WIDTH*8)-1:0] def_data;

    integer index,i,j;
    reg [3:0] bin;

    begin
      index = 0;
      hexstr_conv = 'b0;
      for( i=C_DOUT_WIDTH-1; i>=0; i=i-1 )
      begin
        case (def_data[7:0])
          8'b00000000 :
          begin
            bin = 4'b0000;
            i = -1;
          end
          8'b00110000 : bin = 4'b0000;
          8'b00110001 : bin = 4'b0001;
          8'b00110010 : bin = 4'b0010;
          8'b00110011 : bin = 4'b0011;
          8'b00110100 : bin = 4'b0100;
          8'b00110101 : bin = 4'b0101;
          8'b00110110 : bin = 4'b0110;
          8'b00110111 : bin = 4'b0111;
          8'b00111000 : bin = 4'b1000;
          8'b00111001 : bin = 4'b1001;
          8'b01000001 : bin = 4'b1010;
          8'b01000010 : bin = 4'b1011;
          8'b01000011 : bin = 4'b1100;
          8'b01000100 : bin = 4'b1101;
          8'b01000101 : bin = 4'b1110;
          8'b01000110 : bin = 4'b1111;
          8'b01100001 : bin = 4'b1010;
          8'b01100010 : bin = 4'b1011;
          8'b01100011 : bin = 4'b1100;
          8'b01100100 : bin = 4'b1101;
          8'b01100101 : bin = 4'b1110;
          8'b01100110 : bin = 4'b1111;
          default :
          begin
            bin = 4'bx;
          end
        endcase
        for( j=0; j<4; j=j+1)
        begin
          if ((index*4)+j < C_DOUT_WIDTH)
          begin
            hexstr_conv[(index*4)+j] = bin[j];
          end
        end
        index = index + 1;
        def_data = def_data >> 8;
      end
    end
  endfunction

initial
 begin
    ram_valid_i       = 1'b0;
    read_data_valid_i = 1'b0;
    USERDATA          = hexstr_conv(C_DOUT_RST_VAL);
  end


 //******************************************************************************
 //  connect up optional reset
 //******************************************************************************
 assign rd_rst_i = C_HAS_RST ? RD_RST : 0;


 //******************************************************************************
 //  preloadstage2 indicates that stage2 needs to be updated. This is true
 //  whenever read_data_valid is false, and RAM_valid is true.
 //******************************************************************************
 assign preloadstage2 = ram_valid_i & (~read_data_valid_i | RD_EN);

 //******************************************************************************
 //  preloadstage1 indicates that stage1 needs to be updated. This is true
 //  whenever the RAM has data (RAM_EMPTY is false), and either RAM_Valid is
 //  false (indicating that Stage1 needs updating), or preloadstage2 is active
 //  (indicating that Stage2 is going to update, so Stage1, therefore, must
 //  also be updated to keep it valid.
 //******************************************************************************
 assign preloadstage1 = ((~ram_valid_i | preloadstage2) & ~FIFOEMPTY);

 //******************************************************************************
 // Calculate RAM_REGOUT_EN
 //  The output registers are controlled by the ram_regout_en signal.
 //  These registers should be updated either when the output in Stage2 is
 //  invalid (preloadstage2), OR when the user is reading, in which case the
 //  Stage2 value will go invalid unless it is replenished.
 //******************************************************************************
 assign ram_regout_en = preloadstage2;

 //******************************************************************************
 // Calculate RAM_RD_EN
 //   RAM_RD_EN will be asserted whenever the RAM needs to be read in order to
 //  update the value in Stage1.
 //   One case when this happens is when preloadstage1=true, which indicates
 //  that the data in Stage1 or Stage2 is invalid, and needs to automatically
 //  be updated.
 //   The other case is when the user is reading from the FIFO, which guarantees
 //  that Stage1 or Stage2 will be invalid on the next clock cycle, unless it is
 //  replinished by data from the memory. So, as long as the RAM has data in it,
 //  a read of the RAM should occur.
 //******************************************************************************
 assign ram_rd_en = (RD_EN & ~FIFOEMPTY) | preloadstage1;

 //******************************************************************************
 // Calculate RAMVALID_P0_OUT
 //   RAMVALID_P0_OUT indicates that the data in Stage1 is valid.
 //
 //   If the RAM is being read from on this clock cycle (ram_rd_en=1), then
 //   RAMVALID_P0_OUT is certainly going to be true.
 //   If the RAM is not being read from, but the output registers are being
 //   updated to fill Stage2 (ram_regout_en=1), then Stage1 will be emptying,
 //   therefore causing RAMVALID_P0_OUT to be false.
 //   Otherwise, RAMVALID_P0_OUT will remain unchanged.
 //******************************************************************************
 always @ (posedge RD_CLK or posedge rd_rst_i)
  begin  // PROCESS regout_valid
    if (rd_rst_i)                // asynchronous reset (active high)
      ram_valid_i     <= 1'b0;
    else
    begin
      if (ram_rd_en == 1'b1)
        ram_valid_i   <= 1'b1;
      else
        if (ram_regout_en == 1'b1)
          ram_valid_i <= 1'b0;
        else
          ram_valid_i <= ram_valid_i;
    end //rd_rst_i
  end //always

 //******************************************************************************
 // Calculate READ_DATA_VALID
 //  READ_DATA_VALID indicates whether the value in Stage2 is valid or not.
 //  Stage2 has valid data whenever Stage1 had valid data and ram_regout_en_i=1,
 //  such that the data in Stage1 is propogated into Stage2.
 //******************************************************************************
 always @ (posedge RD_CLK or posedge rd_rst_i)
   begin
    if (rd_rst_i)
     read_data_valid_i <= 1'b0;
    else
     read_data_valid_i <= ram_valid_i | (read_data_valid_i & ~RD_EN);
   end //always


 //*****************************************************************************
 // Calculate EMPTY
 //  Defined as the inverse of READ_DATA_VALID
 //
 // Description:
 //
 //  If read_data_valid_i indicates that the output is not valid,
 // and there is no valid data on the output of the ram to preload it
 // with, then we will report empty.
 //
 //  If there is no valid data on the output of the ram and we are
 // reading, then the FIFO will go empty.
 //
 //*****************************************************************************
 always @ (posedge RD_CLK or posedge rd_rst_i)
  begin
   if (rd_rst_i)                // asynchronous reset (active high)
   begin
    empty_i <= 1'b1;
    empty_q <= 1'b1;
   end
   else // rising clock edge
    begin
     empty_i <= (~ram_valid_i & ~read_data_valid_i) | (~ram_valid_i & RD_EN);
     empty_q <= empty_i;
    end
  end //always

  //Fix for CR:236270 //prasanna
  //Register RD_EN from user to calculate USERUNDERFLOW.
  always @ (posedge RD_CLK or posedge rd_rst_i)
  begin
   if (rd_rst_i)                // asynchronous reset (active high)
   begin
    rd_en_q <= 1'b0;
   end
   else // rising clock edge
    begin
     rd_en_q <= RD_EN;
    end
  end //always


 //*****************************************************************************
 // Calculate user_almost_empty
 //  user_almost_empty is defined such that, unless more words are written
 //  to the FIFO, the next read will cause the FIFO to go EMPTY.
 //
 //  In most cases, whenever the output registers are updated (due to a user
 // read or a preload condition), then user_almost_empty will update to
 // whatever RAM_EMPTY is.
 //
 //  The exception is when the output is valid, the user is not reading, and
 // Stage1 is not empty. In this condition, Stage1 will be preloaded from the
 // memory, so we need to make sure user_almost_empty deasserts properly under
 // this condition.
 //*****************************************************************************
 always @ (posedge RD_CLK or posedge rd_rst_i)
  begin
   if (rd_rst_i)                // asynchronous reset (active high)
   begin
    almost_empty_i <= 1'b1;
    almost_empty_q <= 1'b1;
   end
   else // rising clock edge
    begin
     if ((ram_regout_en) | (~FIFOEMPTY & read_data_valid_i & ~RD_EN))
      begin
       almost_empty_i <= FIFOEMPTY;
      end
     almost_empty_q   <= empty_i;
    end
  end //always


 assign USEREMPTY       = empty_i;
 assign USERALMOSTEMPTY = almost_empty_i;
 assign FIFORDEN        = ram_rd_en;
 assign RAMVALID        = ram_valid_i;
 assign USERVALID       = C_USERVALID_LOW ? ~read_data_valid_i : read_data_valid_i;
 //assign USERUNDERFLOW   = C_USERUNDERFLOW_LOW ? ~(empty_q & RD_EN) : empty_q & RD_EN; //Bug in v3.1 (CR:236270) 
 assign USERUNDERFLOW   = C_USERUNDERFLOW_LOW ? ~(empty_q & rd_en_q) : empty_q & rd_en_q; //Fix for CR:236270 in v3.2 //prasanna 
 
 always @ (posedge RD_CLK or posedge rd_rst_i)
  begin
    if (rd_rst_i)             // asynchronous reset (active high)
      USERDATA   <= hexstr_conv(C_DOUT_RST_VAL);
    else  // rising clock edge
      if (ram_regout_en)
        USERDATA <= FIFODATA;
  end //always





endmodule
