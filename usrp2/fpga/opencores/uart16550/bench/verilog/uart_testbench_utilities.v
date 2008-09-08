//////////////////////////////////////////////////////////////////////
////                                                              ////
////  uart_testbench_utilities.v                                  ////
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
// $Log: uart_testbench_utilities.v,v $
// Revision 1.1  2004/03/27 03:55:17  tadejm
// Testbench with complete selfchecking. BUG is that THRE status is set at the end of last sent bit when TX FIFO is empty instead when only TX FIFO gets empty. This causes testcases not to finish.
//
//
//


`include "uart_defines.v"
`include "uart_testbench_defines.v"
`include "wb_model_defines.v"
`include "timescale.v"

module testbench_utilities;

// Basic system TASKs
//###################

  // DO_RESET
  task do_reset;
  begin
    testbench.wb_reset = 1'b1;
    #1;
    `UTILS_MSG("RESET signal asynchronously set.");
  end
  endtask // do_reset

  // RELEASE_RESET
  task release_reset;
  begin
    @(posedge testbench.wb_clk);
    #1;
    testbench.wb_reset = 1'b0;
    `UTILS_MSG("RESET signal released synchronously to WB clk.");
  end
  endtask // release_reset

  // DISABLE_CLK_GENERATORS
  task disable_clk_generators;
    input         wb_clk_disable;
    input         rx_clk_disable;
    input         tx_clk_disable;
    input         tx_clk_divided_disable;
  begin
    `UTILS_MSG("Following clocks are DISABLED:");
    if (wb_clk_disable)
    begin
      testbench.wb_clk_en = 1'b0;
      `UTILS_MSG("- WB_clk");
    end
    if (rx_clk_disable)
    begin
      testbench.i_uart_device.rx_clk_en = 1'b0;
      `UTILS_MSG("- RX_clk");
    end
    if (tx_clk_disable)
    begin
      testbench.i_uart_device.tx_clk_en = 1'b0;
      `UTILS_MSG("- TX_clk");
    end
    if (tx_clk_divided_disable)
    begin
      testbench.i_uart_device.tx_clk_divided_en = 1'b0;
      `UTILS_MSG("- TX_clk_divided");
    end
    if (~wb_clk_disable && ~rx_clk_disable && ~tx_clk_disable && ~tx_clk_divided_disable)
    begin
      `UTILS_MSG("- NO clocks DISABLED");
    end
  end
  endtask // disable_clk_generators

  // ENABLE_CLK_GENERATORS
  task enable_clk_generators;
    input         wb_clk_enable;
    input         rx_clk_enable;
    input         tx_clk_enable;
    input         tx_clk_divided_enable;
  begin
    `UTILS_MSG("Following clocks are ENABLED:");
    if (wb_clk_enable)
    begin
      testbench.wb_clk_en = 1'b1;
      `UTILS_MSG("- WB_clk");
    end
    if (rx_clk_enable)
    begin
      testbench.i_uart_device.rx_clk_en = 1'b1;
      `UTILS_MSG("- RX_clk");
    end
    if (tx_clk_enable)
    begin
      testbench.i_uart_device.tx_clk_en = 1'b1;
      `UTILS_MSG("- TX_clk");
    end
    if (tx_clk_divided_enable)
    begin
      testbench.i_uart_device.tx_clk_divided_en = 1'b1;
      `UTILS_MSG("- TX_clk_divided");
    end
    if (~wb_clk_enable && ~rx_clk_enable && ~tx_clk_enable && ~tx_clk_divided_enable)
    begin
      `UTILS_MSG("- NO clocks ENABLED");
    end
  end
  endtask // enable_clk_generators

  // SET_DEVICE_TX_RX_CLK_PERIOD
  task set_device_tx_rx_clk_period;
    input [31:0]  clk_period;
  begin
    testbench.i_uart_device.T_clk_period = clk_period;
    `UTILS_VAL1("UART DEVICE TX/RX clock period:", clk_period);
  end
  endtask // set_device_tx_rx_clk_period

  // SET_DEVICE_TX_CLK_DELAY
  task set_device_tx_clk_delay;
    input [31:0]  tx_clk_delay;
  begin
    testbench.i_uart_device.T_clk_delay = tx_clk_delay;
    `UTILS_VAL1("UART DEVICE TX clock delay:", tx_clk_delay);
  end
  endtask // set_device_tx_clk_delay

  // SET_DEVICE_TX_RX_CLK_DIVISOR
  task set_device_tx_rx_clk_divisor;
    input [31:0]  clk_divisor;
  begin
    testbench.i_uart_device.T_divisor = clk_divisor;
    `UTILS_VAL1("UART DEVICE TX/RX clock divisor:", clk_divisor);
  end
  endtask // set_device_tx_rx_clk_divisor

  // SET_WB_CLK_PERIOD
  task set_wb_clock_period;
    input [31:0]  clk_period;
  begin
    testbench.T_wb_clk_period = clk_period;
    testbench.i_uart_device.T_clk_period = clk_period;
    `UTILS_VAL1("WB & UART DEVICE TX/RX clock period:", clk_period);
  end
  endtask // set_wb_clock_period

  // WB_CLK_FOLLOWS_DEVICE_RX_CLK
  task wb_clk_follows_device_rx_clk;
    input [31:0]  time_delay_i;
    integer       time_delay;
  begin
    time_delay = time_delay_i;
    @(posedge testbench.wb_clk);
    testbench.wb_clk_en = 1'b0;
    @(posedge testbench.i_uart_device.rx_clk);
    #time_delay testbench.wb_clk = 1'b1;
    testbench.wb_clk_en = 1'b1;
    `UTILS_VAL1("WB followed UART DEVICE rising edge RX clock for time delay:", time_delay);
  end
  endtask // wb_clk_follows_device_rx_clk

  // DEVICE_RX_CLK_FOLLOWS_WB_CLK
  task device_rx_clk_follows_wb_clk;
    input [31:0]  time_delay_i;
    integer       time_delay;
  begin
    time_delay = time_delay_i;
    @(posedge testbench.i_uart_device.rx_clk);
    testbench.i_uart_device.rx_clk_en = 1'b0;
    @(posedge testbench.wb_clk);
    #time_delay testbench.i_uart_device.rx_clk = 1'b1;
    testbench.i_uart_device.rx_clk_en = 1'b1;
    `UTILS_VAL1("UART DEVICE RX followed WB rising edge clock for time delay:", time_delay);
  end
  endtask // device_rx_clk_follows_wb_clk
  
// Utility tasks
//##############

  // WAIT_FOR_NUM_OF_WB_CLK 
  task wait_for_num_of_wb_clk;
    input [31:0]  num_of_clk;
    integer       count;
  begin
    count = 0;
    `UTILS_VAL1("Waiting for following number of WB CLK periods:", num_of_clk);
    while (count < num_of_clk)
    begin
      @(testbench.wb_clk);
      count = count + 1'b1;
      #1;
    end
    `UTILS_MSG("Waiting expired.");
  end
  endtask // wait_for_num_of_wb_clk

  // WAIT_RX_FIFO_FULL_REGARDLESS_INT
  task wait_rx_fifo_full_regardless_int;
    integer count;
  begin
    count = 0;
    `UTILS_MSG("Waiting for RX FIFO to get full regardless of interrupt.");
    fork
    begin:fifo_full_loop
      while (testbench.i_uart_top.regs.receiver.fifo_rx.count <
             testbench.i_uart_top.regs.receiver.fifo_rx.fifo_depth) // While RX fifo not full
      begin
        @(testbench.wb_clk);
      end
      disable counter;
      `UTILS_MSG("RX FIFO got full.");
    end
    begin:counter
      while (count < testbench.max_wait_cnt)
      begin
        @(testbench.wb_clk);
        count = count + 1'b1;
        #1;
      end
      disable fifo_full_loop;
      `UTILS_ERROR("WAIT counter exceeded max value.");
    end
    join
  end
  endtask // wait_rx_fifo_full_regardless_int

  // WAIT_RX_FIFO_FULL_UNLESS_INT
  task wait_rx_fifo_full_unless_int;
    integer count;
  begin
    count = 0;
    `UTILS_MSG("Waiting for RX FIFO to get full unless interrupt occures before.");
    fork
    begin:fifo_full_loop
      while (testbench.i_uart_top.regs.receiver.fifo_rx.count <
             testbench.i_uart_top.regs.receiver.fifo_rx.fifo_depth) // While RX fifo not full
      begin
        @(testbench.wb_clk);
      end
      disable counter;
      disable int_loop;
      `UTILS_MSG("RX FIFO got full.");
    end
    begin:int_loop
      if (testbench.ier_reg[3:0] == 4'h0)
      begin
        `UTILS_MSG("All interrupts are disabled.");
      end
      else
      begin
        `UTILS_MSG("Interrupts are enabled in IE Register.");
        `UTILS_VAL1("IER:", testbench.ier_reg);
        @(testbench.int_aserted);
        `UTILS_MSG("Interrupt is asserted. The pending interrupt of highest priority is in II Register.");
        `UTILS_VAL1("IIR:", testbench.iir_reg);
        disable counter;
        disable fifo_full_loop;
      end
    end
    begin:counter
      while (count < testbench.max_wait_cnt)
      begin
        @(testbench.wb_clk);
        count = count + 1'b1;
        #1;
      end
      disable int_loop;
      disable fifo_full_loop;
      `UTILS_ERROR("WAIT counter exceeded max value.");
    end
    join
  end
  endtask // wait_rx_fifo_full_unless_int


// UART Initialize TASKs
//######################

  // POSSIBLE INITIALIZE TASKS - NOW FEW STEPS ARE MADE IN EACH testcase!!!


endmodule