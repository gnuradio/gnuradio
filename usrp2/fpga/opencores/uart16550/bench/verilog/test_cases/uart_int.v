//////////////////////////////////////////////////////////////////////
////                                                              ////
////  uart_int.v                                                  ////
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
// $Log: uart_int.v,v $
// Revision 1.1  2004/03/27 04:04:57  tadejm
// Testbench with complete selfchecking. BUG is that THRE status is set at the end of last sent bit when TX FIFO is empty instead when only TX FIFO gets empty. This causes testcases not to finish.
//
//
//


`include "uart_defines.v"
`include "uart_testbench_defines.v"
`include "timescale.v"

module testcase;


// Testcase INDEPENDENT code - common to all testcases
//#####################################################

  // Variables
    // Testbench reporting events & signals
    event             severe_err_event;
    event             err_event;
    event             wrn_event;
    event             msg_event;
    event             val_event;
    event             testbench_log_written;
    reg     [7999: 0] tim;
    reg     [7999: 0] severe_err_msg;
    reg     [7999: 0] msg;
    integer           val;
    // Testcase reporting events & signals
    event             testcase_log_written;
    event             test_end;
    reg     [1599: 0] test_name;
    reg               error_detected;

  // Module for writing to log files
  uart_log log ();
  defparam log.testcase_name = "uart_interrupts";

  // Log
  initial
  fork
  begin: init_log
    reg               ok;
    // Delay allows all other blocks in this fork - join block to execute
    #1;
    log.start_log(ok);
    if (ok !== 1'b1)
    begin
      `SEVERE_ERROR("Failed to open log file(s)!");
      disable init_log;
    end
    testcase_init;
    testcase_test;
    log.end_log;
    # 100;
    $finish;
  end
  begin
    forever
    begin
      @(test_name);
      error_detected = 1'b0;
      log.report_test_name(test_name);
      log.verbose_test_name(test_name);
      -> testcase_log_written;
    end
  end
  begin
    forever
    begin
      @(test_end);
      if (error_detected)
      begin
        log.tests_failed = log.tests_failed + 1'b1;
      end
      else
      begin
        log.tests_ok = log.tests_ok + 1'b1;
        log.report_test_ok;
      end
      -> testcase_log_written;
    end
  end
  begin
    @(severe_err_event);
    error_detected = 1'b1;
    -> test_end;
    @(testcase_log_written);
    log.report_test_failed(severe_err_msg);
    if (testcase.log.free == 0)
    begin
      wait (testcase.log.free);
      severe_err_msg = "Failed to write to log file(s)!";
    end
    log.verbose_severe_err(tim, severe_err_msg);
    -> testbench_log_written;
    # 100; $finish;
  end
  begin
    forever
    begin
      @(err_event);
      error_detected = 1'b1;
      -> test_end;
      @(testcase_log_written);
      log.report_test_failed(msg);
      if (testcase.log.free == 0)
      begin
        `SEVERE_ERROR("Failed to write to log file(s)!");
      end
      log.verbose_err(tim, msg);
      -> testbench_log_written;
      `PROMPT;
    end
  end
  begin
    forever
    begin
      @(wrn_event);
      if (testcase.log.free == 0)
      begin
        `SEVERE_ERROR("Failed to write to log file(s)!");
      end
      log.verbose_wrn(tim, msg);
      -> testbench_log_written;
    end
  end
  begin
    forever
    begin
      @(msg_event);
      if (testcase.log.free == 0)
      begin
        `SEVERE_ERROR("Failed to write to log file(s)!");
      end
      log.verbose_msg(tim, msg);
      -> testbench_log_written;
    end
  end
  begin
    forever
    begin
      @(val_event);
      if (testcase.log.free == 0)
      begin
        `SEVERE_ERROR("Failed to write to log file(s)!");
      end
      log.verbose_val(tim, msg, val);
      -> testbench_log_written;
    end
  end
  join
  

// Testcase (DEPENDENT) code
//###########################

  // Initialization
  task testcase_init;
  begin:init
    test_name = "Initialization of UART.";
    @(testcase_log_written);
    // 
    testbench_utilities.do_reset;
    testbench_utilities.disable_clk_generators(1, 1, 1, 1);
    testbench_utilities.set_device_tx_rx_clk_divisor(32'h1000);
    testbench_utilities.set_wb_clock_period(100);
    testbench_utilities.enable_clk_generators(1, 1, 1, 1);
    #100;
    testbench_utilities.release_reset;
    // 
    uart_wb_utilities.write_dlr(16'h1000);
    uart_wb_utilities.write_ier(8'h07);
    uart_wb_utilities.write_fcr(8'hC0);
    uart_wb_utilities.write_lcr(8'h03);
    // 
    uart_device_utilities.set_rx_length(8);
    uart_device_utilities.disable_rx_parity;
    uart_device_utilities.set_rx_second_stop_bit(0);
    // 
    uart_device_utilities.set_tx_length(8);
    uart_device_utilities.disable_tx_parity;
    uart_device_utilities.correct_tx_parity;
    uart_device_utilities.correct_tx_frame;
    uart_device_utilities.generate_tx_glitch(0);

    -> test_end;
    @(testcase_log_written);
  end
  endtask // testcase_init
  
  // Testcase
  task testcase_test;
  begin:test
    test_name = "Interrupt test.";
    @(testcase_log_written);
    fork
    begin: test
      uart_wb_utilities.write_char(8'hAA);
      @(testbench.int_aserted);
      `TC_MSG("INT ASSERTED!");
      uart_wb_utilities.write_char(8'hAA);
      @(testbench.int_released);
      `TC_MSG("INT RELEASED!");
      @(testbench.int_aserted);
      `TC_MSG("INT ASSERTED!");
      uart_wb_utilities.read_iir;
      @(testbench.int_released);
      `TC_MSG("INT RELEASED!");
    end
    begin: wait_end
      @(testbench.i_uart_device.device_received_packet);
      @(testbench.i_uart_device.device_received_packet);
      repeat(2) @(testbench.i_uart_device.rx_clk);
      disable test;
      disable wait_clk;
    end
    begin: wait_clk
      testbench_utilities.wait_for_num_of_wb_clk(32'h450000);
      disable test;
      disable wait_end;
    end
    join
    repeat (4) @(posedge testbench.wb_clk);
    # 100;
  
    -> test_end;
    @(testcase_log_written);
  end
  endtask // testcase_test


endmodule

