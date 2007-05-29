// -*- verilog -*-
//
//  USRP - Universal Software Radio Peripheral
//
//  Copyright (C) 2007 Corgan Enterprises LLC
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

`timescale 1ns/1ps

`include "../lib/sounder.v"

`define FR_MODE 		7'd64
`define bmFR_MODE_RESET		32'h0001
`define bmFR_MODE_TX            32'h0002
`define bmFR_MODE_RX            32'h0004
`define bmFR_MODE_LP            32'h0008

`define FR_DEGREE		7'd65

module sounder_tb;

   // System bus
   reg         clk;
   reg         rst;
   reg         ena;
   
   // Configuration bus
   reg [6:0]   saddr;
   reg [31:0]  sdata;
   reg 	       s_strobe;

   // DAC bus
   reg         tx_strobe;
   wire [13:0] tx_dac_i;
   wire [13:0] tx_dac_q;

   // ADC bus
   reg         rx_strobe;
   reg [15:0]  rx_adc_i;
   reg [15:0]  rx_adc_q;
   
   // FIFO bus
   wire        fifo_strobe;
   wire [15:0] fifo_i;
   wire [15:0] fifo_q;
   
   // Configuration shadow registers
   reg [31:0]  mode;
   reg [31:0]  degree;
   
   sounder uut
     (.clk_i(clk),.saddr_i(saddr),.sdata_i(sdata),.s_strobe_i(s_strobe),
      .tx_strobe_i(tx_strobe),.tx_dac_i_o(tx_dac_i),.tx_dac_q_o(tx_dac_q),
      .rx_strobe_i(rx_strobe),.rx_adc_i_i(rx_adc_i),.rx_adc_q_i(rx_adc_q),
      .rx_strobe_o(fifo_strobe),.rx_imp_i_o(fifo_i),.rx_imp_q_o(fifo_q));

   // Drive tx_strobe @ half clock rate
   always @(posedge clk)
     tx_strobe <= ~tx_strobe;
   
   // Start up initialization
   initial
     begin
	clk = 0;
	rst = 0;
	ena = 0;
	saddr = 0;
	sdata = 0;
	s_strobe = 0;
	tx_strobe = 0;
	rx_strobe = 1;
	rx_adc_i = 0;
	rx_adc_q = 0;
	mode = 0;
	degree = 0;
	
	@(posedge clk);
	rst = 1;
	@(posedge clk);
	rst = 0;
	@(posedge clk);
	ena = 1;
     end
   
   always
     #5 clk <= ~clk;
   
   initial
     begin
	$monitor($time, " clk=%b rst=%b tx_strobe=%b fifo_strobe=%b phs=%x pn_o=%b pn_ref=%b fifo_i=%x fifo_q=", 
		 clk, uut.reset, tx_strobe, fifo_strobe, uut.receiver.phase_strobe.count_o,
		 uut.transmitter.pn, uut.receiver.pn_ref, fifo_i, fifo_q);
	$dumpfile("sounder_tb.vcd");
	$dumpvars(0, sounder_tb);
     end

   // Test tasks
   task write_cfg_register;
      input [6:0]  regno;
      input [31:0] value;
      
      begin
	 @(posedge clk);
	 saddr <= regno;
	 sdata <= value;
	 s_strobe <= 1'b1;
	 @(posedge clk);
	 s_strobe <= 0;
      end
   endtask // write_cfg_register
   
   // Application reset line
   task set_reset;
      input reset;
      
      begin
	 mode = reset ? (mode | `bmFR_MODE_RESET) : (mode & ~`bmFR_MODE_RESET);
	 write_cfg_register(`FR_MODE, mode);
      end
   endtask // reset
   
   // Set the PN code degree
   task set_degree;
      input [5:0] degree;
      begin
	 write_cfg_register(`FR_DEGREE, degree);
      end
   endtask // set_degree
   
   // Turn on or off the transmitter
   task enable_tx;
      input tx;

      begin
	 mode = tx ? (mode | `bmFR_MODE_TX) : (mode & ~`bmFR_MODE_TX);
	 write_cfg_register(`FR_MODE, mode);
      end
   endtask // enable_tx

   // Turn on or off the receiver
   task enable_rx;
      input rx;

      begin
	 mode = rx ? (mode | `bmFR_MODE_RX) : (mode & ~`bmFR_MODE_RX);
	 write_cfg_register(`FR_MODE, mode);
      end
   endtask // enable_rx


   // Turn on or off digital loopback
   task enable_lp;
      input lp;

      begin
	 mode = lp ? (mode | `bmFR_MODE_LP) : (mode & ~`bmFR_MODE_LP);
	 write_cfg_register(`FR_MODE, mode);
      end
   endtask // enable_lp
   
   // Test transmitter functionality
   task test_tx;
      input [5:0] degree;
      
      begin
	 #20 set_reset(1);
	 #20 set_degree(degree);
	 #20 enable_tx(1);
	 #20 set_reset(0);
	 #(uut.len*20);		// One PN code period

      end
   endtask // test_tx
   
   // Test loopback functionality
   task test_lp;
      input [5:0] degree;
      
      begin
	 #20 set_reset(1);
	 #20 set_degree(degree);
	 #20 enable_tx(1);
	 #20 enable_rx(1);
	 #20 enable_lp(1);
	 #20 set_reset(0);
	 #((uut.len+1)*uut.len*20*2);
      end
   endtask // test_lp
   
   // Execute tests
   initial
     begin
        // #20 test_tx(12);
	#20 test_lp(12);
	#100 $finish;
     end
endmodule

