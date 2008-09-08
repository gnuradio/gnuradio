//////////////////////////////////////////////////////////////////////
////                                                              ////
////  wb_model_defines.v                                          ////
////                                                              ////
////  This file is part of the "uart16550" project                ////
////  http://www.opencores.org/projects/uart16550/                ////
////                                                              ////
////  Author(s):                                                  ////
////      - mihad@opencores.org (Miha Dolenc)                     ////
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
// $Log: wb_model_defines.v,v $
// Revision 1.1  2004/03/27 03:55:17  tadejm
// Testbench with complete selfchecking. BUG is that THRE status is set at the end of last sent bit when TX FIFO is empty instead when only TX FIFO gets empty. This causes testcases not to finish.
//
//
//


`define WB_MODEL_RST_ACTIVE 1'b1
`define WB_MODEL_ERR_MSG(TEXT) $display("Error detected at time %t!", $time) ; $display("%m reports: %0s.", TEXT) ; testcase.msg = TEXT ; -> testcase.err_event

`define WBM_MODEL_WRITE_IN_TYPE   [65: 0]
`define WBM_MODEL_WRITE_WAITS                 [31: 0]
`define WBM_MODEL_WRITE_ALLOWED_SLAVE_WAITS   [63:32]
`define WBM_MODEL_WRITE_LAST                  [64:64]
`define WBM_MODEL_WRITE_FAST_B2B              [65:65]

`define WBM_MODEL_WRITE_OUT_TYPE  [36: 0]
`define WBM_MODEL_WRITE_SLAVE_WAITS   [31: 0]
`define WBM_MODEL_WRITE_STIM_ERR      [32:32]
`define WBM_MODEL_WRITE_DESIGN_ERR    [33:33]
`define WBM_MODEL_WRITE_SLAVE_ACK     [34:34]
`define WBM_MODEL_WRITE_SLAVE_ERR     [35:35]
`define WBM_MODEL_WRITE_SLAVE_RTY     [36:36]

`define WBM_MODEL_READ_IN_TYPE   [65: 0]
`define WBM_MODEL_READ_WAITS                 [31: 0]
`define WBM_MODEL_READ_ALLOWED_SLAVE_WAITS   [63:32]
`define WBM_MODEL_READ_LAST                  [64:64]
`define WBM_MODEL_READ_FAST_B2B              [65:65]

`define WBM_MODEL_READ_OUT_TYPE  [36: 0]
`define WBM_MODEL_READ_SLAVE_WAITS   [31: 0]
`define WBM_MODEL_READ_STIM_ERR      [32:32]
`define WBM_MODEL_READ_DESIGN_ERR    [33:33]
`define WBM_MODEL_READ_SLAVE_ACK     [34:34]
`define WBM_MODEL_READ_SLAVE_ERR     [35:35]
`define WBM_MODEL_READ_SLAVE_RTY     [36:36]

