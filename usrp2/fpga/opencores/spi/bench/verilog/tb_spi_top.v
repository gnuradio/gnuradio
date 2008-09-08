//////////////////////////////////////////////////////////////////////
////                                                              ////
////  tb_spi_top.v                                                ////
////                                                              ////
////  This file is part of the SPI IP core project                ////
////  http://www.opencores.org/projects/spi/                      ////
////                                                              ////
////  Author(s):                                                  ////
////      - Simon Srot (simons@opencores.org)                     ////
////                                                              ////
////  Based on:                                                   ////
////      - i2c/bench/verilog/tst_bench_top.v                     ////
////        Copyright (C) 2001 Richard Herveille                  ////
////                                                              ////
////  All additional information is avaliable in the Readme.txt   ////
////  file.                                                       ////
////                                                              ////
//////////////////////////////////////////////////////////////////////
////                                                              ////
//// Copyright (C) 2002 Authors                                   ////
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

`include "timescale.v"

module tb_spi_top();

  reg         clk;
  reg         rst;
  wire [31:0] adr;
  wire [31:0] dat_i, dat_o;
  wire        we;
  wire  [3:0] sel;
  wire        stb;
  wire        cyc;
  wire        ack;
  wire        err;
  wire        int;

  wire  [7:0] ss;
  wire        sclk;
  wire        mosi;
  wire        miso;

  reg  [31:0] q;
  reg  [31:0] q1;
  reg  [31:0] q2;
  reg  [31:0] q3;
  reg  [31:0] result;

  parameter SPI_RX_0   = 5'h0;
  parameter SPI_RX_1   = 5'h4;
  parameter SPI_RX_2   = 5'h8;
  parameter SPI_RX_3   = 5'hc;
  parameter SPI_TX_0   = 5'h0;
  parameter SPI_TX_1   = 5'h4;
  parameter SPI_TX_2   = 5'h8;
  parameter SPI_TX_3   = 5'hc;
  parameter SPI_CTRL   = 5'h10;
  parameter SPI_DIVIDE = 5'h14;
  parameter SPI_SS     = 5'h18;

  // Generate clock
  always #5 clk = ~clk;

  // Wishbone master model
  wb_master_model #(32, 32) i_wb_master (
    .clk(clk), .rst(rst),
    .adr(adr), .din(dat_i), .dout(dat_o),
    .cyc(cyc), .stb(stb), .we(we), .sel(sel), .ack(ack), .err(err), .rty(1'b0)
  );

  // SPI master core
  spi_top i_spi_top (
    .wb_clk_i(clk), .wb_rst_i(rst), 
    .wb_adr_i(adr[4:0]), .wb_dat_i(dat_o), .wb_dat_o(dat_i), 
    .wb_sel_i(sel), .wb_we_i(we), .wb_stb_i(stb), 
    .wb_cyc_i(cyc), .wb_ack_o(ack), .wb_err_o(err), .wb_int_o(int),
    .ss_pad_o(ss), .sclk_pad_o(sclk), .mosi_pad_o(mosi), .miso_pad_i(miso) 
  );

  // SPI slave model
  spi_slave_model i_spi_slave (
    .rst(rst), .ss(ss[0]), .sclk(sclk), .mosi(mosi), .miso(miso)
  );

  initial
    begin
      $display("\nstatus: %t Testbench started\n\n", $time);

      $dumpfile("bench.vcd");
      $dumpvars(1, tb_spi_top);
      $dumpvars(1, tb_spi_top.i_spi_slave);

      // Initial values
      clk = 0;

      i_spi_slave.rx_negedge = 1'b0;
      i_spi_slave.tx_negedge = 1'b0;

      result = 32'h0;

      // Reset system
      rst = 1'b0; // negate reset
      #2;
      rst = 1'b1; // assert reset
      repeat(20) @(posedge clk);
      rst = 1'b0; // negate reset

      $display("status: %t done reset", $time);
      
      @(posedge clk);

      // Program core
      i_wb_master.wb_write(0, SPI_DIVIDE, 32'h00); // set devider register
      i_wb_master.wb_write(0, SPI_TX_0, 32'h5a);   // set tx register to 0x5a
      i_wb_master.wb_write(0, SPI_CTRL, 32'h208);   // set 8 bit transfer
      i_wb_master.wb_write(0, SPI_SS, 32'h01);     // set ss 0

      $display("status: %t programmed registers", $time);

      i_wb_master.wb_cmp(0, SPI_DIVIDE, 32'h00);   // verify devider register
      i_wb_master.wb_cmp(0, SPI_TX_0, 32'h5a);     // verify tx register
      i_wb_master.wb_cmp(0, SPI_CTRL, 32'h208);     // verify tx register
      i_wb_master.wb_cmp(0, SPI_SS, 32'h01);       // verify ss register

      $display("status: %t verified registers", $time);

      i_spi_slave.rx_negedge = 1'b1;
      i_spi_slave.tx_negedge = 1'b0;
      i_spi_slave.data[31:0] = 32'ha5967e5a;
      i_wb_master.wb_write(0, SPI_CTRL, 32'h308);   // set 8 bit transfer, start transfer

      $display("status: %t generate transfer:  8 bit, msb first, tx posedge, rx negedge", $time);

      // Check bsy bit
      i_wb_master.wb_read(0, SPI_CTRL, q);
      while (q[8])
        i_wb_master.wb_read(1, SPI_CTRL, q);

      i_wb_master.wb_read(1, SPI_RX_0, q);
      result = result + q;

      if (i_spi_slave.data[7:0] == 8'h5a && q == 32'h000000a5)
        $display("status: %t transfer completed: ok", $time);
      else
        $display("status: %t transfer completed: nok", $time);

      i_spi_slave.rx_negedge = 1'b0;
      i_spi_slave.tx_negedge = 1'b1;
      i_wb_master.wb_write(0, SPI_TX_0, 32'ha5);
      i_wb_master.wb_write(0, SPI_CTRL, 32'h408);   // set 8 bit transfer, tx negedge
      i_wb_master.wb_write(0, SPI_CTRL, 32'h508);   // set 8 bit transfer, tx negedge, start transfer

      $display("status: %t generate transfer:  8 bit, msb first, tx negedge, rx posedge", $time);

      // Check bsy bit
      i_wb_master.wb_read(0, SPI_CTRL, q);
      while (q[8])
        i_wb_master.wb_read(1, SPI_CTRL, q);

      i_wb_master.wb_read(1, SPI_RX_0, q);
      result = result + q;

      if (i_spi_slave.data[7:0] == 8'ha5 && q == 32'h00000096)
        $display("status: %t transfer completed: ok", $time);
      else
        $display("status: %t transfer completed: nok", $time);

      i_spi_slave.rx_negedge = 1'b0;
      i_spi_slave.tx_negedge = 1'b1;
      i_wb_master.wb_write(0, SPI_TX_0, 32'h5aa5);
      i_wb_master.wb_write(0, SPI_CTRL, 32'hc10);   // set 16 bit transfer, tx negedge, lsb
      i_wb_master.wb_write(0, SPI_CTRL, 32'hd10);   // set 16 bit transfer, tx negedge, start transfer

      $display("status: %t generate transfer: 16 bit, lsb first, tx negedge, rx posedge", $time);

      // Check bsy bit
      i_wb_master.wb_read(0, SPI_CTRL, q);
      while (q[8])
        i_wb_master.wb_read(1, SPI_CTRL, q);

      i_wb_master.wb_read(1, SPI_RX_0, q);
      result = result + q;

      if (i_spi_slave.data[15:0] == 16'ha55a && q == 32'h00005a7e)
        $display("status: %t transfer completed: ok", $time);
      else
        $display("status: %t transfer completed: nok", $time);

      i_spi_slave.rx_negedge = 1'b1;
      i_spi_slave.tx_negedge = 1'b0;
      i_wb_master.wb_write(0, SPI_TX_0, 32'h76543210);
      i_wb_master.wb_write(0, SPI_TX_1, 32'hfedcba98);
      i_wb_master.wb_write(0, SPI_CTRL, 32'h1a40);   // set 64 bit transfer, rx negedge, lsb
      i_wb_master.wb_write(0, SPI_CTRL, 32'h1b40);   // set 64 bit transfer, rx negedge, start transfer

      $display("status: %t generate transfer: 64 bit, lsb first, tx posedge, rx negedge", $time);

      // Check bsy bit
      i_wb_master.wb_read(0, SPI_CTRL, q);
      while (q[8])
        i_wb_master.wb_read(1, SPI_CTRL, q);

      i_wb_master.wb_read(1, SPI_RX_0, q);
      result = result + q;
      i_wb_master.wb_read(1, SPI_RX_1, q1);
      result = result + q1;

      if (i_spi_slave.data == 32'h195d3b7f && q == 32'h5aa5a55a && q1 == 32'h76543210)
        $display("status: %t transfer completed: ok", $time);
      else
        $display("status: %t transfer completed: nok", $time);

      i_spi_slave.rx_negedge = 1'b0;
      i_spi_slave.tx_negedge = 1'b1;
      i_wb_master.wb_write(0, SPI_TX_0, 32'hccddeeff);
      i_wb_master.wb_write(0, SPI_TX_1, 32'h8899aabb);
      i_wb_master.wb_write(0, SPI_TX_2, 32'h44556677);
      i_wb_master.wb_write(0, SPI_TX_3, 32'h00112233);
      i_wb_master.wb_write(0, SPI_CTRL, 32'h400);
      i_wb_master.wb_write(0, SPI_CTRL, 32'h500);

      $display("status: %t generate transfer: 128 bit, msb first, tx posedge, rx negedge", $time);

      // Check bsy bit
      i_wb_master.wb_read(0, SPI_CTRL, q);
      while (q[8])
        i_wb_master.wb_read(1, SPI_CTRL, q);

      i_wb_master.wb_read(1, SPI_RX_0, q);
      result = result + q;
      i_wb_master.wb_read(1, SPI_RX_1, q1);
      result = result + q1;
      i_wb_master.wb_read(1, SPI_RX_2, q2);
      result = result + q2;
      i_wb_master.wb_read(1, SPI_RX_3, q3);
      result = result + q3;

      if (i_spi_slave.data == 32'hccddeeff && q == 32'h8899aabb && q1 == 32'h44556677 && q2 == 32'h00112233 && q3 == 32'h195d3b7f)
        $display("status: %t transfer completed: ok", $time);
      else
        $display("status: %t transfer completed: nok", $time);

      i_spi_slave.rx_negedge = 1'b0;
      i_spi_slave.tx_negedge = 1'b1;
      i_wb_master.wb_write(0, SPI_TX_0, 32'haa55a5a5);
      i_wb_master.wb_write(0, SPI_CTRL, 32'h1420);
      i_wb_master.wb_write(0, SPI_CTRL, 32'h1520);

      $display("status: %t generate transfer: 32 bit, msb first, tx negedge, rx posedge, ie", $time);

      // Check interrupt signal
      while (!int)
        @(posedge clk);

      i_wb_master.wb_read(1, SPI_RX_0, q);
      result = result + q;
    
      @(posedge clk);
      if (!int && i_spi_slave.data == 32'haa55a5a5 && q == 32'hccddeeff)
        $display("status: %t transfer completed: ok", $time);
      else
        $display("status: %t transfer completed: nok", $time);

      i_spi_slave.rx_negedge = 1'b1;
      i_spi_slave.tx_negedge = 1'b0;
      i_wb_master.wb_write(0, SPI_TX_0, 32'h01248421);
      i_wb_master.wb_write(0, SPI_CTRL, 32'h3220);
      i_wb_master.wb_write(0, SPI_CTRL, 32'h3320);

      $display("status: %t generate transfer: 32 bit, msb first, tx posedge, rx negedge, ie, ass", $time);

      while (!int)
        @(posedge clk);

      i_wb_master.wb_read(1, SPI_RX_0, q);
      result = result + q;

      @(posedge clk);
      if (!int && i_spi_slave.data == 32'h01248421 && q == 32'haa55a5a5)
        $display("status: %t transfer completed: ok", $time);
      else
        $display("status: %t transfer completed: nok", $time);

      i_spi_slave.rx_negedge = 1'b1;
      i_spi_slave.tx_negedge = 1'b0;
      i_wb_master.wb_write(0, SPI_TX_0, 32'h1);
      i_wb_master.wb_write(0, SPI_CTRL, 32'h3201);
      i_wb_master.wb_write(0, SPI_CTRL, 32'h3301);

      $display("status: %t generate transfer: 1 bit, msb first, tx posedge, rx negedge, ie, ass", $time);

      while (!int)
        @(posedge clk);

      i_wb_master.wb_read(1, SPI_RX_0, q);
      result = result + q;

      @(posedge clk);
      if (!int && i_spi_slave.data == 32'h02490843 && q == 32'h0)
        $display("status: %t transfer completed: ok", $time);
      else
        $display("status: %t transfer completed: nok", $time);

      $display("\n\nstatus: %t Testbench done", $time);

      #25000; // wait 25us

      $display("report (%h)", (result ^ 32'h2e8b36ab) + 32'hdeaddead);
      $display("exit (%h)", result ^ 32'h2e8b36ab);

      $stop;
    end

endmodule


