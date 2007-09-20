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

`include "../lib/radar.v"

module radar_tb;

   // System bus
   reg         clk;
   reg         rst;
   reg         ena;
   
   // Configuration bus
   reg [6:0]   saddr;
   reg [31:0]  sdata;
   reg 	       s_strobe;

   // DAC bus
   wire        tx_strobe;
   wire [13:0] tx_dac_i;
   wire [13:0] tx_dac_q;

   // ADC bus
   reg [15:0]  rx_adc_i;
   reg [15:0]  rx_adc_q;
   
   // FIFO bus
   wire        fifo_strobe;
   wire [15:0] fifo_i;
   wire [15:0] fifo_q;
   
   // Configuration shadow registers
   reg [31:0]  mode;
   
   radar uut
     (.clk_i(clk),.saddr_i(saddr),.sdata_i(sdata),.s_strobe_i(s_strobe),
      .tx_strobe_o(tx_strobe),.tx_dac_i_o(tx_dac_i),.tx_dac_q_o(tx_dac_q),
      .rx_adc_i_i(rx_adc_i),.rx_adc_q_i(rx_adc_q),
      .rx_strobe_o(fifo_strobe),.rx_ech_i_o(fifo_i),.rx_ech_q_o(fifo_q));

   // Start up initialization
   initial
     begin
	clk = 0;
	rst = 0;
	ena = 0;
	saddr = 0;
	sdata = 0;
	s_strobe = 0;
	rx_adc_i = 0;
	rx_adc_q = 0;
	mode = 0;
	
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
	//$monitor($time, " clk=%b rst=%b", clk, uut.reset);
	
	$dumpfile("radar_tb.vcd");
	$dumpvars(0, radar_tb);
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
	 mode = reset ? (mode | `bmFR_RADAR_MODE_RESET) : (mode & ~`bmFR_RADAR_MODE_RESET);
	 write_cfg_register(`FR_RADAR_MODE, mode);
      end
   endtask // reset
   
   // Waveform on time
   task set_ton;
      input [23:0] t_on;

      begin
	 write_cfg_register(`FR_RADAR_TON, t_on);
      end
   endtask // set_ton

   // Transmitter switching time
   task set_tsw;
      input [23:0] t_sw;

      begin
	 write_cfg_register(`FR_RADAR_TSW, t_sw);
      end
   endtask // t_sw

   // Receiver look time
   task set_tlook;
      input [23:0] t_look;

      begin
	 write_cfg_register(`FR_RADAR_TLOOK, t_look);
      end
   endtask // set_tlook

   // Inter-pulse idle time
   task set_tidle;
      input [23:0] t_idle;

      begin
	 write_cfg_register(`FR_RADAR_TIDLE, t_idle);
      end
   endtask // set_tidle

   // Chirp amplitude
   task set_ampl;
      input [31:0] ampl;

      begin
	 write_cfg_register(`FR_RADAR_AMPL, ampl);
      end
   endtask // set_ampl

   // Chirp start frequency
   task set_fstart;
      input [31:0] fstart;

      begin
	 write_cfg_register(`FR_RADAR_FSTART, fstart);
      end
   endtask // set_fstart
   
   // Chirp frequency increment
   task set_fincr;
      input [31:0] fincr;

      begin
	 write_cfg_register(`FR_RADAR_FINCR, fincr);
      end
   endtask // set_fincr

   // Chirp frequency increment
   task set_atrdel;
      input [31:0] atrdel;

      begin
	 write_cfg_register(`FR_RADAR_ATRDEL, atrdel);
      end
   endtask // set_fincr

   // Test transmitter functionality
   task test_tx;
      begin
	 #20 set_reset(1);

	 #20 set_ton(320-1);	// 5us on time
	 #20 set_tsw(26-1);	// 406ns switching time
	 #20 set_tlook(640-1);  // 10us look time
	 #20 set_tidle(2854-1); // 60us pulse period
	 
	 #20 set_ampl(16'd9946);
	 #20 set_fstart(32'h80000000); // -16 to 16 MHz
	 #20 set_fincr (32'h0199999A);
	 #20 set_atrdel(32'h00400046); // 64 TX clks, 70 RX clks	 
	 #20 set_reset(0);
	 #200000;
      end
   endtask // test_tx
   
   // Execute tests
   initial
     begin
        #20 test_tx;
	#100 $finish;
     end
endmodule
