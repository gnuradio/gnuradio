/* -*- verilog -*- */
/*
 * Copyright (C) 2003 Matt Ettus
 * Copyright (C) 2007 Corgan Enterprises LLC
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

// Model of Altera FIFO with common clock domain

module fifo_1clk(data, wrreq, rdreq, clock, sclr, q,
		 full, empty, usedw);

   parameter width = 32;
   parameter depth = 4096;
   //`define rd_req 0;  // Set this to 0 for rd_ack, 1 for rd_req
      
   input [31:0]  data;
   input 	 wrreq;
   input 	 rdreq;
   input 	 clock;
   input 	 sclr;
   output [31:0] q;
   output 	 full;
   output 	 empty;
   output [11:0] usedw;
   
   reg [width-1:0] mem [0:depth-1];
   reg [7:0] 	   rdptr;
   reg [7:0] 	   wrptr;
   
`ifdef rd_req
   reg [width-1:0] q;
`else
   wire [width-1:0] q;
`endif
   
   reg [11:0] 	  usedw;
   
   integer 	    i;
   
   always @( sclr)
     begin
	wrptr <= #1 0;
	rdptr <= #1 0;
	for(i=0;i<depth;i=i+1)
	  mem[i] <= #1 0;
     end
   
   always @(posedge clock)
     if(wrreq)
       begin
	  wrptr <= #1 wrptr+1;
	  mem[wrptr] <= #1 data;
       end
   
   always @(posedge clock)
     if(rdreq)
       begin
   	  rdptr <= #1 rdptr+1;
`ifdef rd_req
	  q <= #1 mem[rdptr];
`endif
       end
   
`ifdef rd_req
`else
   assign q = mem[rdptr];
`endif
   
   always @(posedge clock)
     usedw <= #1 wrptr - rdptr;
   
   assign empty = (wrptr == rdptr);
endmodule
