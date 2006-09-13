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



module dpram(wclk,wdata,waddr,wen,rclk,rdata,raddr);
   parameter depth = 4;
   parameter width = 16;
   parameter size = 16;
   
   input wclk;
   input [width-1:0] wdata;
   input [depth-1:0] waddr;
   input 	     wen;

   input rclk;
   output reg [width-1:0] rdata;
   input [depth-1:0]  raddr;
   
   reg [width-1:0]    ram [0:size-1];
   
   always @(posedge wclk)
     if(wen)
       ram[waddr] <= #1 wdata;
   
   always @(posedge rclk)
     rdata <= #1 ram[raddr];
   
endmodule // dpram
