// -*- verilog -*-
//
//  USRP - Universal Software Radio Peripheral
//
//  Copyright (C) 2005,2006 Matt Ettus
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Boston, MA  02110-1301  USA
//

`include "../../firmware/include/fpga_regs_common.v"
`include "../../firmware/include/fpga_regs_standard.v"

module io_pins
  ( inout wire [15:0] io_0, inout wire [15:0] io_1, inout wire [15:0] io_2, inout wire [15:0] io_3,
    input wire [15:0] reg_0, input wire [15:0] reg_1, input wire [15:0] reg_2, input wire [15:0] reg_3,
    input clock, input rx_reset, input tx_reset,
    input [6:0] serial_addr, input [31:0] serial_data, input serial_strobe);
   
   reg [15:0] io_0_oe,io_1_oe,io_2_oe,io_3_oe;
   
   bidir_reg bidir_reg_0 (.tristate(io_0),.oe(io_0_oe),.reg_val(reg_0));
   bidir_reg bidir_reg_1 (.tristate(io_1),.oe(io_1_oe),.reg_val(reg_1));
   bidir_reg bidir_reg_2 (.tristate(io_2),.oe(io_2_oe),.reg_val(reg_2));
   bidir_reg bidir_reg_3 (.tristate(io_3),.oe(io_3_oe),.reg_val(reg_3));
   
   // Upper 16 bits are mask for lower 16
   always @(posedge clock)
     if(serial_strobe)
       case(serial_addr)
	 `FR_OE_0 : io_0_oe
	   <= #1 (io_0_oe & ~serial_data[31:16]) | (serial_data[15:0] & serial_data[31:16] );
	 `FR_OE_1 : io_1_oe
	   <= #1 (io_1_oe & ~serial_data[31:16]) | (serial_data[15:0] & serial_data[31:16] );
	 `FR_OE_2 : io_2_oe
	   <= #1 (io_2_oe & ~serial_data[31:16]) | (serial_data[15:0] & serial_data[31:16] );
	 `FR_OE_3 : io_3_oe
	   <= #1 (io_3_oe & ~serial_data[31:16]) | (serial_data[15:0] & serial_data[31:16] );
       endcase // case(serial_addr)

endmodule // io_pins
