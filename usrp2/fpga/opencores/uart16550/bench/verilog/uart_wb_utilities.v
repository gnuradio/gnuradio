//////////////////////////////////////////////////////////////////////
////                                                              ////
////  uart_wb_utilities.v                                         ////
////                                                              ////
////  This file is part of the "uart16550" project                ////
////  http://www.opencores.org/projects/uart16550/                ////
////                                                              ////
////  Author(s):                                                  ////
////      - tadej@opencores.org (Tadej Markovic)                  ////
////                                                              ////
////  All additional information is avaliable in the README.txt   ////
////  file.                                                       ////
////                                                              ////
////                                                              ////
//////////////////////////////////////////////////////////////////////
////                                                              ////
//// Copyright (C) 2000 - 2004 authors                            ////
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
// $Log: uart_wb_utilities.v,v $
// Revision 1.1  2004/03/27 03:55:17  tadejm
// Testbench with complete selfchecking. BUG is that THRE status is set at the end of last sent bit when TX FIFO is empty instead when only TX FIFO gets empty. This causes testcases not to finish.
//
//
//


`include "uart_defines.v"
`include "uart_testbench_defines.v"
`include "wb_model_defines.v"
`include "timescale.v"

module uart_wb_utilities;

// Single read/write TASKs
//########################

  // SINGLE_READ
  task single_read;
    input  [`UART_ADDR_WIDTH-1:0]  read_adr_i;
    reg                     [3:0]  read_sel_i;
    reg    `WBM_MODEL_READ_IN_TYPE read_stim;
    reg    `WBM_MODEL_READ_OUT_TYPE read_result;
    integer     master_waits;
    integer     slave_waits;
    integer     num_of_reads;
    reg         fast_b2b;
  begin
    read_sel_i = 4'hF;
    testbench.i_wb_master_model.next_read_adr = read_adr_i;
    testbench.i_wb_master_model.next_read_sel = read_sel_i;
    testbench.i_wb_master_model.next_read_cti = 3'b000;  // Clasic WB
    testbench.i_wb_master_model.next_read_bte = $random; // Don't care hwen Clasic WB
    master_waits = {$random} % 13;
    slave_waits = 4;
    num_of_reads = 1;
    fast_b2b = 1'b0;
    read_stim`WBM_MODEL_READ_WAITS               = master_waits;
    read_stim`WBM_MODEL_READ_ALLOWED_SLAVE_WAITS = slave_waits;
    read_stim`WBM_MODEL_READ_LAST                = (num_of_reads == 1);
    read_stim`WBM_MODEL_READ_FAST_B2B            = fast_b2b;
    // Start read
    testbench.i_wb_master_model.start_read(read_stim, read_result);
    // ACK response
    if (read_result`WBM_MODEL_READ_SLAVE_ACK !== 1'b1)
    begin
        `TC_ERROR("Wishbone master model did not receive expected transfer termination from the design.");
    end
    // 
    if (read_result`WBM_MODEL_READ_STIM_ERR !== 1'b0)
    begin
        `TC_ERROR("No reads done since design's wishbone slave interface responded with an error.");
    end
    // 
    if (read_result`WBM_MODEL_READ_DESIGN_ERR !== 1'b0)
    begin
        `TC_ERROR("Wishbone master model detected a design response error during single read access.");
    end
  end
  endtask // single_read

  // SINGLE_WRITE
  task single_write;
    input  [`UART_ADDR_WIDTH-1:0]  write_adr_i;
    input  [`UART_DATA_WIDTH-1:0]  write_dat_i;
    reg                     [3:0]  write_sel_i;
    reg    `WBM_MODEL_WRITE_IN_TYPE write_stim;
    reg    `WBM_MODEL_WRITE_OUT_TYPE write_result;
    integer     master_waits;
    integer     slave_waits;
    integer     num_of_writes;
    reg         fast_b2b;
  begin
    write_sel_i = 4'hF;
    testbench.i_wb_master_model.next_write_adr = write_adr_i;
    testbench.i_wb_master_model.next_write_sel = write_sel_i;
    testbench.i_wb_master_model.next_write_dat = write_dat_i;
    testbench.i_wb_master_model.next_write_cti = 3'b000;  // Clasic WB
    testbench.i_wb_master_model.next_write_bte = $random; // Don't care hwen Clasic WB
    master_waits = {$random} % 13;
    slave_waits = 4;
    num_of_writes = 1;
    fast_b2b = 1'b0;
    write_stim`WBM_MODEL_WRITE_WAITS               = master_waits;
    write_stim`WBM_MODEL_WRITE_ALLOWED_SLAVE_WAITS = slave_waits;
    write_stim`WBM_MODEL_WRITE_LAST                = (num_of_writes == 1);
    write_stim`WBM_MODEL_WRITE_FAST_B2B            = fast_b2b;
    // Start write
    testbench.i_wb_master_model.start_write(write_stim, write_result);
    // ACK response
    if (write_result`WBM_MODEL_WRITE_SLAVE_ACK !== 1'b1)
    begin
        `TC_ERROR("Wishbone master model did not receive expected transfer termination from the design.");
    end
    // 
    if (write_result`WBM_MODEL_WRITE_STIM_ERR !== 1'b0)
    begin
        `TC_ERROR("No writes done since wishbone master model reported an error.");
    end
    // 
    if (write_result`WBM_MODEL_WRITE_DESIGN_ERR !== 1'b0)
    begin
        `TC_ERROR("Wishbone master model detected a design response error during single write access.");
    end
  end
  endtask // single_write

// Char read/write TASKs
//######################

  // READ_CHAR
  task read_char;
  begin
    if (testbench.lcr_reg[7] === 1'b1) // dlab == 1
    begin
      `UTILS_ERROR("READING of CHAR from RB Register NOT possible, since DLAB in LC Register is set.");
    end
    else
    begin
      `UTILS_MSG("READING of CHAR from UART's RB Register.");
      single_read(`UART_REG_RB);
      `UTILS_VAL1("Read RBR =", testbench.i_wb_master_model.read_dat);
    end
  end
  endtask // read_char

  // WRITE_CHAR
  task write_char;
    input  [7:0]  char_i;
  begin
    if (testbench.lcr_reg[7] === 1'b1) // dlab == 1
    begin
      `UTILS_ERROR("WRITING CHAR to TR Register NOT possible, since DLAB in LC Register is set.");
    end
    else
    begin
      `UTILS_MSG("WRITING CHAR to UART's TR Register.");
      single_write(`UART_REG_TR, char_i);
      `UTILS_VAL1("Write TRR =", testbench.i_wb_master_model.write_dat);
    end
  end
  endtask // write_char

// Register read/write TASKs
//##########################

  // READ_IER - adr 1
  task read_ier;
  begin
    if (testbench.lcr_reg[7] === 1'b1) // dlab == 1
    begin
      `UTILS_ERROR("READING of IE Register NOT possible, since DLAB in LC Register is set.");
    end
    else
    begin
      `UTILS_MSG("READING UART's IE Register.");
      single_read(`UART_REG_IE);
      `UTILS_VAL1("Read IER =", testbench.i_wb_master_model.read_dat);
    end
  end
  endtask // read_ier

  // WRITE_IER - adr 1
  task write_ier;
    input  [7:0]  data_i;
  begin
    if (testbench.lcr_reg[7] === 1'b1) // dlab == 1
    begin
      `UTILS_ERROR("WRITING to IE Register NOT possible, since DLAB in LC Register is set.");
    end
    else
    begin
      `UTILS_MSG("WRITING UART's IE Register.");
      single_write(`UART_REG_IE, data_i);
      `UTILS_VAL1("Write IER =", testbench.i_wb_master_model.write_dat);
    end
  end
  endtask // write_ier

  // READ_IIR - adr 2
  task read_iir;
  begin
    `UTILS_MSG("READING UART's II Register.");
    single_read(`UART_REG_II);
    `UTILS_VAL1("Read IIR =", testbench.i_wb_master_model.read_dat);
  end
  endtask // read_iir

  // WRITE_FCR - adr 2
  task write_fcr;
    input  [7:0]  data_i;
  begin
    `UTILS_MSG("WRITING UART's FC Register.");
    single_write(`UART_REG_FC, data_i);
    `UTILS_VAL1("Write FCR =", testbench.i_wb_master_model.write_dat);
  end
  endtask // write_fcr

  // READ_LCR - adr 3
  task read_lcr;
  begin
    `UTILS_MSG("READING UART's LC Register.");
    single_read(`UART_REG_LC);
    `UTILS_VAL1("Read LCR =", testbench.i_wb_master_model.read_dat);
  end
  endtask // read_lcr

  // WRITE_LCR - adr 3
  task write_lcr;
    input  [7:0]  data_i;
  begin
    `UTILS_MSG("WRITING UART's LC Register.");
    single_write(`UART_REG_LC, data_i);
    `UTILS_VAL1("Write LCR =", testbench.i_wb_master_model.write_dat);
  end
  endtask // write_lcr

  // WRITE_MCR - adr 4
  task write_mcr;
    input  [7:0]  data_i;
  begin
    `UTILS_MSG("WRITING UART's MC Register.");
    single_write(`UART_REG_MC, data_i);
    `UTILS_VAL1("Write MCR =", testbench.i_wb_master_model.write_dat);
  end
  endtask // write_mcr

  // READ_LSR - adr 5
  task read_lsr;
  begin
    `UTILS_MSG("READING UART's LS Register.");
    single_read(`UART_REG_LS);
    `UTILS_VAL1("Read LSR =", testbench.i_wb_master_model.read_dat);
  end
  endtask // read_lsr

  // READ_MSR - adr 6
  task read_msr;
  begin
    `UTILS_MSG("READING UART's MS Register.");
    single_read(`UART_REG_MS);
    `UTILS_VAL1("Read MSR =", testbench.i_wb_master_model.read_dat);
  end
  endtask // read_msr

  // READ_DLR - adr 0, 1
  task read_dlr;
  begin
    if (testbench.lcr_reg[7] === 1'b0) // dlab == 0
    begin
      // Setting DLAB
      `UTILS_MSG("DLAB in LC Register is going to be 1.");
      `UTILS_VAL1("Current LCR =", testbench.lcr_reg);
      write_lcr(testbench.lcr_reg | 8'h80);
      // Reading DL Register
      `UTILS_MSG("READING UART's DL Register [15:8].");
      single_read(`UART_REG_DL2);
      `UTILS_VAL1("Read DLR [15:8] =", testbench.i_wb_master_model.read_dat);
      `UTILS_MSG("READING UART's DL Register [ 7:0].");
      single_read(`UART_REG_DL1);
      `UTILS_VAL1("Read DLR [ 7:0] =", testbench.i_wb_master_model.read_dat);
      // Resetting DLAB
      `UTILS_MSG("DLAB in LC Register is going to be 0.");
      write_lcr(testbench.lcr_reg & 8'h7F);
    end
    else
    begin
      `UTILS_MSG("DLAB in LC Register is already 1.");
      `UTILS_VAL1("Current LCR =", testbench.lcr_reg);
      // Reading DL Register
      `UTILS_MSG("READING UART's DL Register [15:8].");
      single_read(`UART_REG_DL2);
      `UTILS_VAL1("Read DLR [15:8] =", testbench.i_wb_master_model.read_dat);
      `UTILS_MSG("READING UART's DL Register [ 7:0].");
      single_read(`UART_REG_DL1);
      `UTILS_VAL1("Read DLR [ 7:0] =", testbench.i_wb_master_model.read_dat);
    end
  end
  endtask // read_dlr

  // WRITE_DLR - adr 0, 1
  task write_dlr;
    input  [15:0]  data_i;
  begin
    if (testbench.lcr_reg[7] === 1'b0) // dlab == 0
    begin
      // Setting DLAB
      `UTILS_MSG("DLAB in LC Register is going to be 1.");
      `UTILS_VAL1("Current LCR =", testbench.lcr_reg);
      write_lcr(testbench.lcr_reg | 8'h80);
      // Writing DL Register
      `UTILS_MSG("WRITING UART's DL Register [15:8].");
      single_write(`UART_REG_DL2, data_i[15:8]);
      `UTILS_VAL1("Write DLR [15:8] =", testbench.i_wb_master_model.write_dat);
      `UTILS_MSG("WRITING UART's DL Register [ 7:0].");
      single_write(`UART_REG_DL1, data_i[ 7:0]);
      `UTILS_VAL1("Write DLR [ 7:0] =", testbench.i_wb_master_model.write_dat);
      // Resetting DLAB
      `UTILS_MSG("DLAB in LC Register is going to be 0.");
      write_lcr(testbench.lcr_reg & 8'h7F);
    end
    else
    begin
      `UTILS_MSG("DLAB in LC Register is already 1.");
      `UTILS_VAL1("Current LCR =", testbench.lcr_reg);
      // Writing DL Register
      `UTILS_MSG("WRITING UART's DL Register [15:8].");
      single_write(`UART_REG_DL2, data_i[15:8]);
      `UTILS_VAL1("Write DLR [15:8] =", testbench.i_wb_master_model.write_dat);
      `UTILS_MSG("WRITING UART's DL Register [ 7:0].");
      single_write(`UART_REG_DL1, data_i[ 7:0]);
      `UTILS_VAL1("Write DLR [ 7:0] =", testbench.i_wb_master_model.write_dat);
    end
  end
  endtask // write_dlr


endmodule