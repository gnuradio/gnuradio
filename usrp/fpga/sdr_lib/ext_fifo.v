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

 
// Vendor Independent FIFO module
// Width and Depth should be parameterizable
// Asynchronous clocks for each side
// Read side is read-acknowledge, not read-request
// FIFO does not enforce "don't write when full, don't read when empty"
// That is up to the connecting modules
// The FIFO only holds 2^N-1 entries, not 2^N

module fifo (reset,data,write,wrclk,wr_used,q,read_ack,rdclk,rd_used);
   parameter width=32;
   parameter depth=10;

   input reset;  // Asynchronous
   input [width-1:0] data;
   input write;
   input wrclk;
   output [depth-1:0] wr_used;
   output [width-1:0] q;
   input read_ack;
   input rdclk;
   output [depth-1:0] rd_used;

   reg [depth-1:0] read_addr, write_addr, 
		   read_addr_gray, read_addr_gray_sync,
		   write_addr_gray, write_addr_gray_sync;

   // Pseudo-dual-port RAM
   dpram #(.depth(10),.width(width),.size(1024))
         fifo_ram (.wclk(wrclk),.wdata(data),.waddr(write_addr),.wen(write),
		   .rclk(rdclk), .rdata(q),.raddr(read_addr) );

   wire [depth-1:0] wag,rag;

   // Keep track of own side's pointer
   always @(posedge wrclk or posedge reset)
     if(reset) write_addr <= #1 0;
     else if(write) write_addr <= #1 write_addr + 1;

   always @(posedge rdclk or posedge reset)
     if(reset) read_addr <= #1 0;
     else if(read_ack) read_addr <= #1 read_addr + 1;

   // Convert own side pointer to gray
   bin2gray #(depth) write_b2g (write_addr,wag);
   bin2gray #(depth) read_b2g (read_addr,rag);

   // Latch it
   always @(posedge wrclk or posedge reset)
     if(reset) write_addr_gray <= #1 0;
     else write_addr_gray <= #1 wag;

   always @(posedge rdclk or posedge reset)
     if(reset) read_addr_gray <= #1 0;
     else read_addr_gray <= #1 rag;

   // Send it to other side and latch
   always @(posedge wrclk or posedge reset)
     if(reset) read_addr_gray_sync <= #1 0;
     else read_addr_gray_sync <= #1 read_addr_gray;

   always @(posedge rdclk or posedge reset)
     if(reset) write_addr_gray_sync <= #1 0;
     else write_addr_gray_sync <= #1 write_addr_gray;

   wire [depth-1:0] write_addr_sync, read_addr_sync;
   
   // Convert back to binary
   gray2bin #(depth) write_g2b (write_addr_gray_sync, write_addr_sync);
   gray2bin #(depth) read_g2b (read_addr_gray_sync, read_addr_sync);
 
   assign rd_used = write_addr_sync - read_addr;
   assign wr_used = write_addr - read_addr_sync;
   		  
endmodule // fifo

module bin2gray(bin_val,gray_val);
   parameter width = 8;
   input [width-1:0] bin_val;
   output reg [width-1:0] gray_val;
   
   integer i;

   always @*
     begin
	gray_val[width-1] = bin_val[width-1];
	for(i=0;i<width-1;i=i+1)
	  gray_val[i] = bin_val[i] ^ bin_val[i+1];
     end
endmodule // bin2gray

module gray2bin(gray_val,bin_val);
   parameter width = 8;
   input [width-1:0] gray_val;
   output reg [width-1:0] bin_val;

   integer i;
   
   always @*
     begin
	bin_val[width-1] = gray_val[width-1];
	for(i=width-2;i>=0;i=i-1)
	  bin_val[i] = bin_val[i+1] ^ gray_val[i];
     end
endmodule // gray2bin
