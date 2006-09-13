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

// Tasks

/////////////////////////////////////////////////
// USB interface

task initialize_usb;
begin
	OE = 0;WE = 0;RD = 0;
	usbdatareg <= 16'h0;
end
endtask

task write_from_usb;
begin
	tb_oe <= 1'b1;
	@(posedge usbclk);
	usbdatareg <= #1 $random % 65536;
	WE <= #1 1'b1;
	@(posedge usbclk)
	WE <= #1 1'b0;
	tb_oe <= #1 1'b0;
end
endtask

task burst_usb_write;
      input [31:0] repeat_count;
      
      begin
	 tb_oe <= 1'b1;
	 repeat(repeat_count)
	   begin
	      @(posedge usbclk)
		usbdatareg <= #1 usbdatareg + 1;	      //$random % 65536;
	      WE <= #1 1'b1;
	   end
	 @(posedge usbclk)
	   WE <= #1 1'b0;
	 tb_oe <= 1'b0;
      end
endtask // burst_usb_write


task read_from_usb;
begin
	@(posedge usbclk);
	RD <= #1 1'b1;
	@(posedge usbclk);
	RD <= #1 1'b0;
	OE <= #1 1'b1;
	@(posedge usbclk);
	OE <= #1 1'b0;
end
endtask

task burst_usb_read;
      input [31:0] repeat_count;
      begin
	 while (~have_packet_rdy) begin
	    @(posedge usbclk);
	 end
	 
	 @(posedge usbclk)
	   RD <= #1 1'b1;
	 repeat(repeat_count)
	   begin
	      @(posedge usbclk)
		OE <= #1 1'b1;
	   end
	 RD <= #1 1'b0;
	 @(posedge usbclk);
	 OE <= #1 1'b0;
      end
endtask // burst_usb_read

/////////////////////////////////////////////////
// TX and RX enable

//////////////////////////////////////////////////
// Set up control bus

`define ch1in_freq 0
`define ch2in_freq 1
`define ch3in_freq 2
`define ch4in_freq 3
`define ch1out_freq 4
`define ch2out_freq 5
`define ch3out_freq 6
`define ch4out_freq 7
`define rates 8
`define misc 9
  
  task send_config_word;
      input [7:0] addr;
      input [31:0] data;
      integer i;
      
      begin
	 #10 serenable = 1;
	 for(i=7;i>=0;i=i-1)
	   begin
	      #10 serdata = addr[i];
	      #10 serclk = 0;
	      #10 serclk = 1;
	      #10 serclk = 0;
	   end
	 for(i=31;i>=0;i=i-1)
	   begin
	      #10 serdata = data[i];
	      #10 serclk = 0;
	      #10 serclk = 1;
	      #10 serclk = 0;
	   end
	 #10 serenable = 0;
	 //	#10 serload = 0;
	 //	#10 serload = 1;
	 #10 serclk = 1;
	 #10 serclk = 0;
	 //#10 serload = 0;
      end
  endtask // send_config_word


/////////////////////////////////////////////////////////

