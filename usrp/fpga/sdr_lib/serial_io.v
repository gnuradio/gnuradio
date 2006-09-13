// -*- verilog -*-
//
//  USRP - Universal Software Radio Peripheral
//
//  Copyright (C) 2003,2004 Matt Ettus
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



// Serial Control Bus from Cypress chip

module serial_io
  ( input master_clk,
    input serial_clock,
    input serial_data_in,
    input enable,
    input reset,
    inout wire serial_data_out,
    output reg [6:0] serial_addr,
    output reg [31:0] serial_data,
    output wire serial_strobe,
    input wire [31:0] readback_0,
    input wire [31:0] readback_1,
    input wire [31:0] readback_2,
    input wire [31:0] readback_3,
    input wire [31:0] readback_4,
    input wire [31:0] readback_5,
    input wire [31:0] readback_6,
    input wire [31:0] readback_7
    );
       
   reg 	      is_read;
   reg [7:0]  ser_ctr;
   reg 	      write_done;

   assign serial_data_out = is_read ? serial_data[31] : 1'bz;

   always @(posedge serial_clock, posedge reset, negedge enable)
     if(reset)
       ser_ctr <= #1 8'd0;
     else if(~enable)
       ser_ctr <= #1 8'd0;
     else if(ser_ctr == 39)
       ser_ctr <= #1 8'd0;
     else
       ser_ctr <= #1 ser_ctr + 8'd1;

   always @(posedge serial_clock, posedge reset, negedge enable)
     if(reset)
       is_read <= #1 1'b0;
     else if(~enable)
       is_read <= #1 1'b0;
     else if((ser_ctr == 7)&&(serial_addr[6]==1))
       is_read <= #1 1'b1;
   
   always @(posedge serial_clock, posedge reset)
     if(reset)
       begin
	  serial_addr <= #1 7'b0;
	  serial_data <= #1 32'b0;
	  write_done <= #1 1'b0;
       end
     else if(~enable)
       begin
	  //serial_addr <= #1 7'b0;
	  //serial_data <= #1 32'b0;
	  write_done <= #1 1'b0;
       end
     else 
       begin
	  if(~is_read && (ser_ctr == 39))
	    write_done <= #1 1'b1;
	  else
	    write_done <= #1 1'b0;
	  if(is_read & (ser_ctr==8))
	    case (serial_addr)
	      7'd1: serial_data <= #1 readback_0;
	      7'd2: serial_data <= #1 readback_1;
	      7'd3: serial_data <= #1 readback_2;
	      7'd4: serial_data <= #1 readback_3;
	      7'd5: serial_data <= #1 readback_4;
	      7'd6: serial_data <= #1 readback_5;
	      7'd7: serial_data <= #1 readback_6;
	      7'd8: serial_data <= #1 readback_7;
	      default: serial_data <= #1 32'd0;
	    endcase // case(serial_addr)
	  else if(ser_ctr >= 8)
	    serial_data <= #1 {serial_data[30:0],serial_data_in};
	  else if(ser_ctr < 8)
	    serial_addr <= #1 {serial_addr[5:0],serial_data_in};
       end // else: !if(~enable)

   reg enable_d1, enable_d2;
   always @(posedge master_clk)
     begin
	enable_d1 <= #1 enable;
	enable_d2 <= #1 enable_d1;
     end

   assign serial_strobe = enable_d2 & ~enable_d1;
   
endmodule // serial_io


