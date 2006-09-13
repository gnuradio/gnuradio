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


// testbench for fullchip

`timescale 1ns/1ns

module fullchip_tb();

`include "usrp_tasks.v"

fullchip fullchip
  (
   .clk_120mhz(clk_120mhz),
   .reset(reset),
   .enable_rx(enable_rx),
   .enable_tx(enable_tx),
   .SLD(serload),
   .SEN(serenable),
   .clear_status(),
   .SDI(serdata),
   .SCLK(serclk),
	  
   .adc1_data(adc1_data),
   .adc2_data(adc2_data),
   .adc3_data(adc1_data),
   .adc4_data(adc2_data),
   
   .dac1_data(dac1_data),
   .dac2_data(dac2_data),
   .dac3_data(),.dac4_data(),
   
   .adclk0(adclk),.adclk1(),
   
   .adc_oeb(),.adc_otr(4'b0),
   
   .clk_out(clk_out),
   
   .misc_pins(),
   
   // USB interface
   .usbclk(usbclk),.usbctl(usbctl),
   .usbrdy(usbrdy),.usbdata(usbdata)
   );	
	
	reg clk_120mhz;
	reg usbclk;
	reg reset;

	reg [11:0] adc1_data, adc2_data;
	wire [13:0] dac1_data, dac2_data;

	wire [5:0] usbctl;
	wire [5:0] usbrdy;

	wire [15:0] usbdata;

	reg WE, RD, OE;

        assign usbctl[0] = WE;
        assign usbctl[1] = RD;
        assign usbctl[2] = OE;
	assign usbctl[5:3] = 0;

        wire have_packet_rdy = usbrdy[1];
   
	reg tb_oe;
   initial tb_oe=1'b1;
   
	assign usbdata = tb_oe ? usbdatareg : 16'hxxxx;
	reg serload, serenable, serclk, serdata;
	reg enable_tx, enable_rx;
	reg [15:0] usbdatareg;

///////////////////////////////////////////////
//  Simulation Control
initial
begin
	$dumpfile("fullchip_tb.vcd");
	$dumpvars(0, fullchip_tb);
end

//initial #1000000 $finish;

///////////////////////////////////////////////
//  Monitors

//initial $monitor(dac1_data);

///////////////////////////////////////////////
// Clock and reset

initial clk_120mhz = 0;
initial usbclk = 0;
always #24 clk_120mhz = ~clk_120mhz;
always #60 usbclk = ~usbclk;

initial reset = 1'b1;
initial #500 reset = 1'b0;

/////////////////////////////////////////////////
// Run AD input

always @(posedge adclk)	 adc1_data <= #1 12'd1234;
always @(posedge adclk)	 adc2_data <= #1 12'd1234;

/////////////////////////////////////////////////
// USB interface

  initial 
    begin
       initialize_usb;
       #30000 @(posedge usbclk);
       burst_usb_write(257);
       
       #30000 burst_usb_read(256);
       #10000 $finish;
   
//	repeat(30)
//	begin
//		write_from_usb;
//		read_from_usb;
//	end
end

/////////////////////////////////////////////////
// TX and RX enable

initial enable_tx = 1'b0;
initial #40000 enable_tx = 1'b1;
initial enable_rx = 1'b0;
initial #40000 enable_rx = 1'b1;

//////////////////////////////////////////////////
// Set up control bus

initial 
begin
	#1000 send_config_word(`ch1in_freq,32'h0); // 1 MHz on 60 MHz clock
	send_config_word(`ch2in_freq,32'h0);
	send_config_word(`ch3in_freq,32'h0);
	send_config_word(`ch4in_freq,32'h0);
	send_config_word(`ch1out_freq,32'h01234567);
	send_config_word(`ch2out_freq,32'h0);
	send_config_word(`ch3out_freq,32'h0);
	send_config_word(`ch4out_freq,32'h0);
	send_config_word(`misc,32'h0);
	send_config_word(`rates,{8'd2,8'd12,8'h0f,8'h07});
	// adc, ext, interp, decim
end

/////////////////////////////////////////////////////////

endmodule

