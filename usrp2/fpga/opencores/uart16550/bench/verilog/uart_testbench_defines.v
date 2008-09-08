//////////////////////////////////////////////////////////////////////
////                                                              ////
////  uart_testbench_defines.v                                    ////
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
// $Log: uart_testbench_defines.v,v $
// Revision 1.1  2004/03/27 03:55:17  tadejm
// Testbench with complete selfchecking. BUG is that THRE status is set at the end of last sent bit when TX FIFO is empty instead when only TX FIFO gets empty. This causes testcases not to finish.
//
//
//


`ifdef VERBOSE
  // Displaying messages to CRT and providing to "testcase"
  `define SEVERE_ERROR(TEXT)   $display("Time: %t    (%m)", $time); $display("*E, Object %m reporting severe error:"); $display("    %0s", TEXT); $sformat(testcase.tim, "Time: %t    (%m)", $time); testcase.severe_err_msg = TEXT; -> testcase.severe_err_event; @(testcase.testbench_log_written)
  `define UTILS_WARNING(TEXT)  $display("Time: %t    (%m)", $time); $display("*W, %0s", TEXT); $sformat(testcase.tim, "Time: %t    (%m)", $time); testcase.msg = TEXT; -> testcase.wrn_event; @(testcase.testbench_log_written)
  `define UTILS_ERROR(TEXT)    $display("Time: %t    (%m)", $time); $display("*E, %0s", TEXT); $sformat(testcase.tim, "Time: %t    (%m)", $time); testcase.msg = TEXT; -> testcase.err_event; @(testcase.testbench_log_written)
  `define BENCH_WARNING(TEXT)  $display("Time: %t    (%m)", $time); $display("*W, %0s", TEXT); $sformat(testcase.tim, "Time: %t    (%m)", $time); testcase.msg = TEXT; -> testcase.wrn_event; @(testcase.testbench_log_written)
  `define BENCH_ERROR(TEXT)    $display("Time: %t    (%m)", $time); $display("*E, %0s", TEXT); $sformat(testcase.tim, "Time: %t    (%m)", $time); testcase.msg = TEXT; -> testcase.err_event; @(testcase.testbench_log_written)
  `define TC_ERROR(TEXT)       $display("Time: %t    (%m)", $time); $display("*E, %0s", TEXT); $sformat(testcase.tim, "Time: %t    (%m)", $time); testcase.msg = TEXT; -> testcase.err_event; @(testcase.testbench_log_written)
  `define DEVICE_WARNING(TEXT) $display("Time: %t    (%m)", $time); $display("*W, %0s", TEXT); $sformat(testcase.tim, "Time: %t    (%m)", $time); testcase.msg = TEXT; -> testcase.wrn_event; @(testcase.testbench_log_written)
  `define DEVICE_ERROR(TEXT)   $display("Time: %t    (%m)", $time); $display("*E, %0s", TEXT); $sformat(testcase.tim, "Time: %t    (%m)", $time); testcase.msg = TEXT; -> testcase.err_event; @(testcase.testbench_log_written)
  // Displaying messages to CRT
  `define UTILS_MSG(TEXT)       $display("Time %t    (%m)", $time); $display("*N, %0s", TEXT);           $sformat(testcase.tim, "Time: %t    (%m)", $time); testcase.msg = TEXT;                     -> testcase.msg_event; @(testcase.testbench_log_written)
  `define UTILS_VAL1(TEXT, VAL) $display("Time %t    (%m)", $time); $display("*N, %0s %0h.", TEXT, VAL); $sformat(testcase.tim, "Time: %t    (%m)", $time); testcase.msg = TEXT; testcase.val = VAL; -> testcase.val_event; @(testcase.testbench_log_written)
  `define BENCH_MSG(TEXT)       $display("Time %t    (%m)", $time); $display("*N, %0s", TEXT);           $sformat(testcase.tim, "Time: %t    (%m)", $time); testcase.msg = TEXT;                     -> testcase.msg_event; @(testcase.testbench_log_written)
  `define BENCH_VAL1(TEXT, VAL) $display("Time %t    (%m)", $time); $display("*N, %0s %0h.", TEXT, VAL); $sformat(testcase.tim, "Time: %t    (%m)", $time); testcase.msg = TEXT; testcase.val = VAL; -> testcase.val_event; @(testcase.testbench_log_written)
  `define TC_MSG(TEXT)          $display("Time %t    (%m)", $time); $display("*N, %0s", TEXT);           $sformat(testcase.tim, "Time: %t    (%m)", $time); testcase.msg = TEXT;                     -> testcase.msg_event; @(testcase.testbench_log_written)
  `define TC_VAL1(TEXT, VAL)    $display("Time %t    (%m)", $time); $display("*N, %0s %0h.", TEXT, VAL); $sformat(testcase.tim, "Time: %t    (%m)", $time); testcase.msg = TEXT; testcase.val = VAL; -> testcase.val_event; @(testcase.testbench_log_written)
`else
  // Displaying messages to CRT and providing to "testcase"
  `define SEVERE_ERROR(TEXT)   $display("*E, Object %m reporting severe error:"); $display("    %0s", TEXT); $sformat(testcase.tim, "Time: %t    (%m)", $time); testcase.severe_err_msg = TEXT; -> testcase.severe_err_event; @(testcase.testbench_log_written)
  `define UTILS_WARNING(TEXT)  $display("*W, %0s", TEXT); $sformat(testcase.tim, "Time: %t    (%m)", $time); testcase.msg = TEXT; -> testcase.wrn_event; @(testcase.testbench_log_written)
  `define UTILS_ERROR(TEXT)    $display("*E, %0s", TEXT); $sformat(testcase.tim, "Time: %t    (%m)", $time); testcase.msg = TEXT; -> testcase.err_event; @(testcase.testbench_log_written)
  `define BENCH_WARNING(TEXT)  $display("*W, %0s", TEXT); $sformat(testcase.tim, "Time: %t    (%m)", $time); testcase.msg = TEXT; -> testcase.wrn_event; @(testcase.testbench_log_written)
  `define BENCH_ERROR(TEXT)    $display("*E, %0s", TEXT); $sformat(testcase.tim, "Time: %t    (%m)", $time); testcase.msg = TEXT; -> testcase.err_event; @(testcase.testbench_log_written)
  `define TC_ERROR(TEXT)       $display("*E, %0s", TEXT); $sformat(testcase.tim, "Time: %t    (%m)", $time); testcase.msg = TEXT; -> testcase.err_event; @(testcase.testbench_log_written)
  `define DEVICE_WARNING(TEXT) $display("*W, %0s", TEXT); $sformat(testcase.tim, "Time: %t    (%m)", $time); testcase.msg = TEXT; -> testcase.wrn_event; @(testcase.testbench_log_written)
  `define DEVICE_ERROR(TEXT)   $display("*E, %0s", TEXT); $sformat(testcase.tim, "Time: %t    (%m)", $time); testcase.msg = TEXT; -> testcase.err_event; @(testcase.testbench_log_written)
  // Displaying messages to CRT
  `define UTILS_MSG(TEXT)       $display("*N, %0s", TEXT);           $sformat(testcase.tim, "Time: %t    (%m)", $time); testcase.msg = TEXT;                     -> testcase.msg_event; @(testcase.testbench_log_written)
  `define UTILS_VAL1(TEXT, VAL) $display("*N, %0s %0h.", TEXT, VAL); $sformat(testcase.tim, "Time: %t    (%m)", $time); testcase.msg = TEXT; testcase.val = VAL; -> testcase.val_event; @(testcase.testbench_log_written)
  `define BENCH_MSG(TEXT)       $display("*N, %0s", TEXT);           $sformat(testcase.tim, "Time: %t    (%m)", $time); testcase.msg = TEXT;                     -> testcase.msg_event; @(testcase.testbench_log_written)
  `define BENCH_VAL1(TEXT, VAL) $display("*N, %0s %0h.", TEXT, VAL); $sformat(testcase.tim, "Time: %t    (%m)", $time); testcase.msg = TEXT; testcase.val = VAL; -> testcase.val_event; @(testcase.testbench_log_written)
  `define TC_MSG(TEXT)          $display("*N, %0s", TEXT);           $sformat(testcase.tim, "Time: %t    (%m)", $time); testcase.msg = TEXT;                     -> testcase.msg_event; @(testcase.testbench_log_written)
  `define TC_VAL1(TEXT, VAL)    $display("*N, %0s %0h.", TEXT, VAL); $sformat(testcase.tim, "Time: %t    (%m)", $time); testcase.msg = TEXT; testcase.val = VAL; -> testcase.val_event; @(testcase.testbench_log_written)
`endif

// Testcase end
`define PROMPT #1000000; log.end_log; $finish
