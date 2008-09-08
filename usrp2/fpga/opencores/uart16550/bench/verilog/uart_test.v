//////////////////////////////////////////////////////////////////////
////                                                              ////
////  uart_test.v                                                 ////
////                                                              ////
////                                                              ////
////  This file is part of the "UART 16550 compatible" project    ////
////  http://www.opencores.org/cores/uart16550/                   ////
////                                                              ////
////  Documentation related to this project:                      ////
////  - http://www.opencores.org/cores/uart16550/                 ////
////                                                              ////
////  Projects compatibility:                                     ////
////  - WISHBONE                                                  ////
////  RS232 Protocol                                              ////
////  16550D uart (mostly supported)                              ////
////                                                              ////
////  Overview (main Features):                                   ////
////  UART core test bench                                        ////
////                                                              ////
////  Known problems (limits):                                    ////
////  A very simple test bench. Creates two UARTS and sends       ////
////  data on to the other.                                       ////
////                                                              ////
////  To Do:                                                      ////
////  More complete testing should be done!!!                     ////
////                                                              ////
////  Author(s):                                                  ////
////      - gorban@opencores.org                                  ////
////      - Jacob Gorban                                          ////
////                                                              ////
////  Created:        2001/05/12                                  ////
////  Last Updated:   2001/05/17                                  ////
////                  (See log for the revision history)          ////
////                                                              ////
////                                                              ////
//////////////////////////////////////////////////////////////////////
////                                                              ////
//// Copyright (C) 2000 Jacob Gorban, gorban@opencores.org        ////
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
// $Log: uart_test.v,v $
// Revision 1.3  2001/05/31 20:08:01  gorban
// FIFO changes and other corrections.
//
// Revision 1.2  2001/05/17 18:34:18  gorban
// First 'stable' release. Should be sythesizable now. Also added new header.
//
// Revision 1.0  2001-05-17 21:27:12+02  jacob
// Initial revision
//
//
//`define DATA_BUS_WIDTH_8
`include "timescale.v"
module uart_test ();

`include "uart_defines.v"

reg       clkr;
reg       wb_rst_ir;
wire  [`UART_ADDR_WIDTH-1:0]  wb_adr_i;
wire  [31:0]  wb_dat_i;
wire  [31:0]  wb_dat_o;
wire [3:0]    wb_sel_i;
wire        pad_stx_o;
reg       pad_srx_ir;

integer e;

uart_top  uart_snd(
  clk, 
  
  // Wishbone signals
            wb_rst_i, wb_adr_i, wb_dat_i, wb_dat_o, wb_we_i, wb_stb_i, wb_cyc_i, wb_ack_o,  wb_sel_i,
  int_o, // interrupt request

  // UART signals
  // serial input/output
  pad_stx_o, pad_srx_i,

  // modem signals
  rts_o, cts_i, dtr_o, dsr_i, ri_i, dcd_i
`ifdef UART_HAS_BAUDRATE_OUTPUT
  , baud1_o
`endif



  );


// All the signals and regs named with a 1 are receiver fifo signals

wire  [`UART_ADDR_WIDTH-1:0]  wb1_adr_i;
wire  [31:0]      wb1_dat_i;
wire  [31:0]      wb1_dat_o;
wire [3:0]    wb1_sel_i;
wire        int1_o;
wire        stx1_o;
reg       srx1_ir;

uart_top  uart_rcv(
  clk, 
  
  // Wishbone signals
  wb_rst_i, wb1_adr_i, wb1_dat_i, wb1_dat_o, wb1_we_i, wb1_stb_i, wb1_cyc_i, wb1_ack_o, wb1_sel_i,  
  int1_o, // interrupt request

  // UART signals
  // serial input/output
  stx1_o, srx1_i,

  // modem signals
  rts1_o, cts1_i, dtr1_o, dsr1_i, ri1_i, dcd1_i
`ifdef UART_HAS_BAUDRATE_OUTPUT
  , baud2_o
`endif

  );

assign clk = clkr;
assign wb_rst_i = wb_rst_ir;
assign pad_srx_i = pad_srx_ir;
assign cts_i = 1; //cts_ir;
assign dsr_i = 1; //dsr_ir;
assign ri_i = 1; //ri_ir;
assign dcd_i = 1; //dcd_ir;

assign srx1_i = srx1_ir;
assign cts1_i = 1; //cts1_ir;
assign dsr1_i = 1; //dsr1_ir;
assign ri1_i = 1; //ri1_ir;
assign dcd1_i = 1; //dcd1_ir;

reg [31:0] dat_o;
/////////// CONNECT THE UARTS
always @(pad_stx_o)
begin
  srx1_ir = pad_stx_o;  
end

initial
begin
  clkr = 0;
  #50000 $finish;
end

wb_mast wbm(// Outputs
        .adr                 (wb_adr_i),
        .dout                (wb_dat_i),
        .cyc                 (wb_cyc_i),
        .stb                 (wb_stb_i),
        .sel                 (wb_sel_i),
        .we                (wb_we_i),
        // Inputs
        .clk                 (clk),
        .rst                 (wb_rst_i),
        .din                 (wb_dat_o),
        .ack                 (wb_ack_o),
        .err                 (1'b0),
        .rty                 (1'b0));

wb_mast wbm1(// Outputs
         .adr                (wb1_adr_i),
         .dout               (wb1_dat_i),
         .cyc                (wb1_cyc_i),
         .stb                (wb1_stb_i),
         .sel                (wb1_sel_i),
         .we                 (wb1_we_i),
         // Inputs
         .clk                (clk),
         .rst                (wb_rst_i),
         .din                (wb1_dat_o),
         .ack                (wb1_ack_o),
         .err                (1'b0),
         .rty                (1'b0));

// The test sequence
initial
begin
  #1 wb_rst_ir = 1;
  #10 wb_rst_ir = 0;
    
  //write to lcr. set bit 7
  //wb_cyc_ir = 1;
  wbm.wb_wr1(`UART_REG_LC, 4'b1000, {8'b10011011, 24'b0});
  // set dl to divide by 3
  wbm.wb_wr1(`UART_REG_DL1,4'b0001, 32'd2);
  @(posedge clk);
  @(posedge clk);
  // restore normal registers
  wbm.wb_wr1(`UART_REG_LC, 4'b1000, {8'b00011011, 24'b0}); //00011011 

  fork
  begin
    $display("%m : %t : sending : %h", $time, 8'b10000001);
    wbm.wb_wr1(0, 4'b1, 32'b10000001);
    @(posedge clk);
    @(posedge clk);
    $display("%m : %t : sending : %h", $time, 8'b01000010);
    wbm.wb_wr1(0, 4'b1, 32'b01000010);
    @(posedge clk);
    @(posedge clk);
    $display("%m : %t : sending : %h", $time, 8'b11000011);
    wbm.wb_wr1(0, 4'b1, 32'b11000011);
    @(posedge clk);
    @(posedge clk);
    $display("%m : %t : sending : %h", $time, 8'b00100100);
    wbm.wb_wr1(0, 4'b1, 32'b00100100);
    @(posedge clk);
    @(posedge clk);
    $display("%m : %t : sending : %h", $time, 8'b10100101);
    wbm.wb_wr1(0, 4'b1, 32'b10100101);
    @(posedge clk);
    @(posedge clk);
    $display("%m : %t : sending : %h", $time, 8'b01100110);
    wbm.wb_wr1(0, 4'b1, 32'b01100110);
    @(posedge clk);
    @(posedge clk);
    $display("%m : %t : sending : %h", $time, 8'b11100111);
    wbm.wb_wr1(0, 4'b1, 32'b11100111);
    @(posedge clk);
    @(posedge clk);
    $display("%m : %t : sending : %h", $time, 8'b00011000);
    wbm.wb_wr1(0, 4'b1, 32'b00011000);
    wait (uart_snd.regs.tstate==0 && uart_snd.regs.transmitter.tf_count==0);
//    disable check;
  end
//  begin: check
//  end
  join
end

always @(int1_o)
  if (int1_o)
    $display("INT_O high (%g)", $time);
  else
    $display("INT_O low (%g)", $time);
    
always @(int1_o)
begin
  if (int1_o) begin
    wbm1.wb_rd1(2,4'b0100, dat_o);
    $display("IIR : %h", dat_o);
    wbm1.wb_rd1(5,4'b0010, dat_o);
    $display("LSR : %h", dat_o);
    wbm1.wb_rd1(0, 4'b1, dat_o);
    $display("%m : %t : Data out: %h", $time, dat_o);
  end
end

// receiver side
initial
begin
  #11;
  //write to lcr. set bit 7
  //wb_cyc_ir = 1;
  wbm1.wb_wr1(`UART_REG_LC, 4'b1000, {8'b10011011, 24'b0});
  // set dl to divide by 3
  wbm1.wb_wr1(`UART_REG_DL1, 4'b1, 32'd2);
  @(posedge clk);
  @(posedge clk);
  // restore normal registers
  wbm1.wb_wr1(`UART_REG_LC, 4'b1000, {8'b00011011, 24'b0});
  wbm1.wb_wr1(`UART_REG_IE, 4'b0010, {16'b0, 8'b00001111, 8'b0});
  wait(uart_rcv.regs.receiver.rf_count == 2);
    e = 800;
  while (e > 0)
  begin
    @(posedge clk)
    if (uart_rcv.regs.enable) e = e - 1;
  end
  wbm1.wb_rd1(0, 4'b1, dat_o);
  $display("%m : %t : Data out: %h", $time, dat_o);
  @(posedge clk);
  wbm1.wb_rd1(0, 4'b1, dat_o);
  $display("%m : %t : Data out: %h", $time, dat_o);
  $display("%m : Finish");
  e = 800;
  while (e > 0)
  begin
    @(posedge clk)
    if (uart_rcv.regs.enable) e = e - 1;
  end
  e = 800;
  while (e > 0)
  begin
    @(posedge clk)
    if (uart_rcv.regs.enable) e = e - 1;
  end
  $finish;
end

//always @(uart_rcv.regs.rstate)
//begin
//  $display($time,": Receiver state changed to: ", uart_rcv.regs.rstate);
//end

initial
  begin
    `ifdef DATA_BUS_WIDTH_8
$display("DATA BUS IS 8");
`else
$display("DATA BUS IS 32");
`endif
    $display("%d %d", `UART_ADDR_WIDTH, `UART_DATA_WIDTH);

  end


always
begin
  #5 clkr = ~clk;
end

endmodule
