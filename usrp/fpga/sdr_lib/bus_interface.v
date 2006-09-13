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
// A packet is 512 Bytes.  Each fifo line is 4 bytes
// Fifo has 1024 or 2048 lines

module bus_interface
  (     input usbclk,
	input reset,
	inout [15:0] usbdata,   // TRISTATE
	input wire [5:0] usbctl,
	output wire [5:0] usbrdy,
	output [31:0] txdata,
	input [31:0] rxdata,
	input txclk,
	input txstrobe,
	input rxclk,
	input rxstrobe,
	output [11:0] debugbus,
	input clear_status
	);

   parameter   IN_CHANNELS = 1;
   parameter   OUT_CHANNELS = 1;
   parameter   bitmask = (IN_CHANNELS*2)-1;

   wire have_space, have_pkt_rdy;
   wire WR, RD, OE;
   reg tx_underrun, rx_overrun;
   
   assign WR = usbctl[0];
   assign RD = usbctl[1];
   assign OE = usbctl[2];
   
   assign usbrdy[0] = have_space;
   assign usbrdy[1] = have_pkt_rdy;
   assign usbrdy[2] = tx_underrun;
   assign usbrdy[3] = rx_overrun;
   
   reg [IN_CHANNELS*2*16-1:0] fifo_in;
   wire [OUT_CHANNELS*2*16-1:0] fifo_out;
   
   wire [15:0] usbdata_in = usbdata;
   
   reg select_out;
   reg select_in;
   
   reg commit;
   reg rd_next;
   reg [15:0] usbdata_out;
   wire [10:0] txfifolevel,rxfifolevel;
   reg [8:0] write_count;
   wire tx_empty;
   wire tx_full;
   wire rx_empty;
   wire rx_full;
   wire [31:0] txd;
   wire rdreq;
	
   // Tri-state bus macro
   bustri bustri(.data(usbdata_out),
		 .enabledt(OE),
		 .tridata(usbdata)  );

   //////////////////////////////////////////////
   // TX Side (USB --> DAC)
   always @(posedge usbclk, posedge reset)
     begin
	if(reset)
	  begin
	     fifo_in <= #1 0;
	     write_count <= #1 0;
	  end
	else 
	  if(WR & ~write_count[8])
	    begin
	       case(write_count[0])
		 1'b0 : fifo_in[31:16] <= #1 usbdata_in;  // I
		 1'b1 : fifo_in[15:0] <= #1 usbdata_in;   // Q
	       endcase
	       write_count <= #1 write_count + 9'd1;
	    end
	  else
	    write_count <= #1 WR ? write_count : 9'b0;
     end
   
   always @(posedge usbclk)
     if(reset)
       commit <= #1 1'b0;
     else
       if(write_count[0] && ~write_count[8] && WR)
	 commit <= #1 1'b1;
       else 
	 commit <= #1 1'b0;
   
   assign rdreq = txstrobe & !tx_empty;
   assign txdata = tx_empty ? 32'b0 : txd;
   
   always @(posedge txclk)
     if(reset)
       tx_underrun <= 1'b0;
     else if(txstrobe & tx_empty)
       tx_underrun <= 1'b1;
     else if(clear_status)
       tx_underrun <= 1'b0;

   fifo_1c_2k	txfifo (.data ( fifo_in ),
			.wrreq ( commit ),
			.wrclk ( usbclk ),

			.q ( txd ),			
			.rdreq ( rdreq),
			.rdclk ( txclk ),
			
			.aclr ( reset ),

			.rdempty ( tx_empty ),
			.rdusedw (  ),
			.wrfull ( tx_full ),
			.wrusedw ( txfifolevel )
			);
   
   assign have_space = (txfifolevel <= (2048-128));
   
   //////////////////////////////
   // Receive FIFO (ADC --> USB)

   always @(posedge rxclk)
     if(reset)
       rx_overrun <= 1'b0;
     else if(rxstrobe & rx_full)
       rx_overrun <= 1'b1;
     else if(clear_status)
       rx_overrun <= 1'b0;

   always @(select_out, fifo_out)
     case(select_out)
       0 : usbdata_out = fifo_out[31:16];  // I
       1 : usbdata_out = fifo_out[15:0];   // Q
     endcase
  
/*
	always @(posedge usbclk, posedge reset)
	if(reset)
		usbdata_out <= #1 16'b0;
	else
		if(select_out)
			usbdata_out = fifo_out[31:16];
		else
			usbdata_out = fifo_out[15:0];
	*/
	 
   always @(negedge usbclk, posedge reset)
     if(reset)
       select_out <= #1 1'b0;
     else if(~RD)
       select_out <= #1 1'b0;
     else 
       select_out <= #1 ~select_out;
 
   fifo_1c_2k	rxfifo (.data ( rxdata ), // counter ),
			.wrreq (rxstrobe & ~rx_full ),
			.wrclk ( rxclk ),
			
			.q ( fifo_out ),
			.rdreq ( select_out ),// & RD ), // FIXME
			.rdclk ( usbclk ),
			
			.aclr ( reset ),
			
			.rdempty ( rx_empty ),
			.rdusedw ( rxfifolevel ),
			.wrfull ( rx_full ),
			.wrusedw (  )
			);
   
   assign have_pkt_rdy = (rxfifolevel >= 128);

   // Debugging Aids
   assign debugbus[0] = tx_underrun;
   assign debugbus[1] = rx_overrun;
   assign debugbus[2] = tx_empty;
   assign debugbus[3] = tx_full;
   assign debugbus[4] = rx_empty;
   assign debugbus[5] = rx_full;
   assign debugbus[6] = txstrobe;
   assign debugbus[7] = rxstrobe;
   assign debugbus[8] = select_out;
   assign debugbus[9] = rxstrobe & ~rx_full;
   assign debugbus[10] = have_space;
   assign debugbus[11] = have_pkt_rdy;
   
endmodule // bus_interface

