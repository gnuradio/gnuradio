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

// Interface to Cypress FX2 bus
// A packet is 512 Bytes.  Each fifo line is 2 bytes
// Fifo has 1024 or 2048 lines

module tx_buffer
  ( input usbclk,
    input bus_reset,  // Used here for the 257-Hack to fix the FX2 bug
    input reset,  // standard DSP-side reset
    input [15:0] usbdata,
    input wire WR,
    output wire have_space,
    output reg tx_underrun,
    input wire [3:0] channels,
    output reg [15:0] tx_i_0,
    output reg [15:0] tx_q_0,
    output reg [15:0] tx_i_1,
    output reg [15:0] tx_q_1,
    output reg [15:0] tx_i_2,
    output reg [15:0] tx_q_2,
    output reg [15:0] tx_i_3,
    output reg [15:0] tx_q_3,
    input txclk,
    input txstrobe,
    input clear_status,
    output wire tx_empty,
    output [11:0] debugbus
    );
   
   wire [11:0] txfifolevel;
   reg [8:0] write_count;
   wire tx_full;
   wire [15:0] fifodata;
   wire rdreq;

   reg [3:0] load_next;

   // DAC Side of FIFO
   assign    rdreq = ((load_next != channels) & !tx_empty);
   
   always @(posedge txclk)
     if(reset)
       begin
	  {tx_i_0,tx_q_0,tx_i_1,tx_q_1,tx_i_2,tx_q_2,tx_i_3,tx_q_3}
	    <= #1 128'h0;
	  load_next <= #1 4'd0;
       end
     else
       if(load_next != channels)
	 begin
	    load_next <= #1 load_next + 4'd1;
	    case(load_next)
	      4'd0 : tx_i_0 <= #1 tx_empty ? 16'd0 : fifodata;
	      4'd1 : tx_q_0 <= #1 tx_empty ? 16'd0 : fifodata;
	      4'd2 : tx_i_1 <= #1 tx_empty ? 16'd0 : fifodata;
	      4'd3 : tx_q_1 <= #1 tx_empty ? 16'd0 : fifodata;
	      4'd4 : tx_i_2 <= #1 tx_empty ? 16'd0 : fifodata;
	      4'd5 : tx_q_2 <= #1 tx_empty ? 16'd0 : fifodata;
	      4'd6 : tx_i_3 <= #1 tx_empty ? 16'd0 : fifodata;
	      4'd7 : tx_q_3 <= #1 tx_empty ? 16'd0 : fifodata;
	    endcase // case(load_next)
	 end // if (load_next != channels)
       else if(txstrobe & (load_next == channels))
	 begin
	    load_next <= #1 4'd0;
	 end

   // USB Side of FIFO
   assign have_space = (txfifolevel <= (4095-256));

   always @(posedge usbclk)
     if(bus_reset)        // Use bus reset because this is on usbclk
       write_count <= #1 0;
     else if(WR & ~write_count[8])
       write_count <= #1 write_count + 9'd1;
     else
       write_count <= #1 WR ? write_count : 9'b0;

   // Detect Underruns
   always @(posedge txclk)
     if(reset)
       tx_underrun <= 1'b0;
     else if(txstrobe & (load_next != channels))
       tx_underrun <= 1'b1;
     else if(clear_status)
       tx_underrun <= 1'b0;

   // FIFO
   fifo_4k txfifo 
     ( .data ( usbdata ),
       .wrreq ( WR & ~write_count[8] ),
       .wrclk ( usbclk ),
       
       .q ( fifodata ),			
       .rdreq ( rdreq ),
       .rdclk ( txclk ),
       
       .aclr ( reset ),  // asynch, so we can use either
       
       .rdempty ( tx_empty ),
       .rdusedw (  ),
       .wrfull ( tx_full ),
       .wrusedw ( txfifolevel )
       );
   
   // Debugging Aids
   assign debugbus[0] = WR;
   assign debugbus[1] = have_space;
   assign debugbus[2] = tx_empty;
   assign debugbus[3] = tx_full;
   assign debugbus[4] = tx_underrun;
   assign debugbus[5] = write_count[8];
   assign debugbus[6] = txstrobe;
   assign debugbus[7] = rdreq;
   assign debugbus[11:8] = load_next;
   
endmodule // tx_buffer

