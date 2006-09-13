// -*- verilog -*-
//
//  USRP - Universal Software Radio Peripheral
//
//  Copyright (C) 2003 Matt Ettus
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



// Basic Phase accumulator for DDS


module phase_acc (clk,reset,enable,strobe,serial_addr,serial_data,serial_strobe,phase);   
   parameter FREQADDR = 0;
   parameter PHASEADDR = 0;
   parameter resolution = 32;
   
   input     clk, reset, enable, strobe;
   input [6:0] serial_addr;
   input [31:0] serial_data;
   input 	serial_strobe;
   
   output reg [resolution-1:0] phase;
   wire [resolution-1:0] freq;

   setting_reg #(FREQADDR) sr_rxfreq0(.clock(clk),.reset(1'b0),.strobe(serial_strobe),.addr(serial_addr),.in(serial_data),.out(freq));

   always @(posedge clk)
     if(reset)
       phase <= #1 32'b0;
     else if(serial_strobe & (serial_addr == PHASEADDR))
       phase <= #1 serial_data;
     else if(enable & strobe)
       phase <= #1 phase + freq;

endmodule // phase_acc

   
