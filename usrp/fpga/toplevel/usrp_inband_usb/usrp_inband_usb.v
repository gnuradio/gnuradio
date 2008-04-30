// -*- verilog -*-
//
//  USRP - Universal Software Radio Peripheral
//
//  Copyright (C) 2003,2004 Matt Ettus
//  Copyright 2007 Free Software Foundation, Inc.
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
`define TX_IN_BAND
`define RX_IN_BAND

`include "config.vh"
`include "../../../firmware/include/fpga_regs_common.v"
`include "../../../firmware/include/fpga_regs_standard.v"

module usrp_inband_usb
(output MYSTERY_SIGNAL,
 input master_clk,
 input SCLK,
 input SDI,
 inout SDO,
 input SEN_FPGA,

 input FX2_1,
 output FX2_2,
 output FX2_3,
 
 input wire [11:0] rx_a_a,
 input wire [11:0] rx_b_a,
 input wire [11:0] rx_a_b,
 input wire [11:0] rx_b_b,

 output wire [13:0] tx_a,
 output wire [13:0] tx_b,

 output wire TXSYNC_A,
 output wire TXSYNC_B,
 
  // USB interface
 input usbclk,
 input wire [2:0] usbctl,
 output wire [1:0] usbrdy,
 inout [15:0] usbdata,  // NB Careful, inout

 // These are the general purpose i/o's that go to the daughterboard slots
 inout wire [15:0] io_tx_a,
 inout wire [15:0] io_tx_b,
 inout wire [15:0] io_rx_a,
 inout wire [15:0] io_rx_b
 );	
   wire [15:0] debugdata,debugctrl;
   assign MYSTERY_SIGNAL = 1'b0;
   
   wire clk64,clk128;
   
   wire WR = usbctl[0];
   wire RD = usbctl[1];
   wire OE = usbctl[2];

   wire have_space, have_pkt_rdy;
   assign usbrdy[0] = have_space;
   assign usbrdy[1] = have_pkt_rdy;

   wire   rx_overrun;    
   wire   clear_status = FX2_1;
   assign FX2_2 = rx_overrun;
   assign FX2_3 = (tx_underrun == 0);
      
   wire [15:0] usbdata_out;
   
   wire [3:0]  dac0mux,dac1mux,dac2mux,dac3mux;
   
   wire        tx_realsignals;
   wire [3:0]  rx_numchan;
   wire [2:0]  tx_numchan;
   
   wire [7:0]  interp_rate, decim_rate;
   wire [15:0] tx_debugbus, rx_debugbus;
   
   wire        enable_tx, enable_rx;
   wire        tx_dsp_reset, rx_dsp_reset, tx_bus_reset, rx_bus_reset;
   wire [7:0]  settings;
   
   // Tri-state bus macro
   bustri bustri( .data(usbdata_out),.enabledt(OE),.tridata(usbdata) );

   wire [15:0] ch0tx,ch1tx,ch2tx,ch3tx; //,ch4tx,ch5tx,ch6tx,ch7tx;
   wire [15:0] ch0rx,ch1rx,ch2rx,ch3rx,ch4rx,ch5rx,ch6rx,ch7rx;
   
   // TX
   wire [15:0] i_out_0,i_out_1,q_out_0,q_out_1;
   wire [15:0] bb_tx_i0,bb_tx_q0,bb_tx_i1,bb_tx_q1;  // bb_tx_i2,bb_tx_q2,bb_tx_i3,bb_tx_q3;
   
   wire        strobe_interp, tx_sample_strobe;
   wire        tx_empty;
   
   wire        serial_strobe;
   wire [6:0]  serial_addr;
   wire [31:0] serial_data;

   reg [15:0] debug_counter;
   reg [15:0] loopback_i_0,loopback_q_0;
   

   //Connection RX inband <-> TX inband
   wire rx_WR;
   wire [15:0] rx_databus;
   wire rx_WR_done;
   wire rx_WR_enabled;
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // Transmit Side
`ifdef TX_ON
   assign      bb_tx_i0 = ch0tx;
   assign      bb_tx_q0 = ch1tx;
   assign      bb_tx_i1 = ch2tx;
   assign      bb_tx_q1 = ch3tx;

wire [1:0] tx_underrun;

`ifdef TX_IN_BAND
 	tx_buffer_inband tx_buffer
     ( .usbclk(usbclk),.bus_reset(tx_bus_reset),.reset(tx_dsp_reset),
       .usbdata(usbdata),.WR(WR),.have_space(have_space),
       .tx_underrun(tx_underrun),.channels({tx_numchan,1'b0}),
       .tx_i_0(ch0tx),.tx_q_0(ch1tx),
       .tx_i_1(ch2tx),.tx_q_1(ch3tx),
       .tx_i_2(),.tx_q_2(),
       .tx_i_3(),.tx_q_3(),
       .txclk(clk64),.txstrobe(strobe_interp),
       .clear_status(clear_status),
       .tx_empty(tx_empty),
	   .rx_WR(rx_WR),
	   .rx_databus(rx_databus), 
	   .rx_WR_done(rx_WR_done),
	   .rx_WR_enabled(rx_WR_enabled),
	   .reg_addr(reg_addr),
	   .reg_data_out(reg_data_out),
	   .reg_data_in(reg_data_in),
	   .reg_io_enable(reg_io_enable),
	   .debugbus(rx_debugbus),
	   .rssi_0(rssi_0), .rssi_1(rssi_1), .rssi_2(rssi_2), 
       .rssi_3(rssi_3), .threshhold(rssi_threshhold), .rssi_wait(rssi_wait),
	   .stop(stop), .stop_time(stop_time));

  `ifdef TX_DUAL
    defparam tx_buffer.NUM_CHAN=2;
  `endif

`else
   tx_buffer tx_buffer
     ( .usbclk(usbclk),.bus_reset(tx_bus_reset),.reset(tx_dsp_reset),
       .usbdata(usbdata),.WR(WR),.have_space(have_space),.tx_underrun(tx_underrun),
       .channels({tx_numchan,1'b0}),
       .tx_i_0(ch0tx),.tx_q_0(ch1tx),
       .tx_i_1(ch2tx),.tx_q_1(ch3tx),
       .tx_i_2(),.tx_q_2(),
       .tx_i_3(),.tx_q_3(),
       .txclk(clk64),.txstrobe(strobe_interp),
       .clear_status(clear_status),
       .tx_empty(tx_empty));
`endif

 `ifdef TX_EN_0
   tx_chain tx_chain_0
     ( .clock(clk64),.reset(tx_dsp_reset),.enable(enable_tx),
       .interp_rate(interp_rate),.sample_strobe(tx_sample_strobe),
       .interpolator_strobe(strobe_interp),.freq(),
       .i_in(bb_tx_i0),.q_in(bb_tx_q0),.i_out(i_out_0),.q_out(q_out_0) );
 `else
   assign      i_out_0=16'd0;
   assign      q_out_0=16'd0;
 `endif

 `ifdef TX_EN_1
   tx_chain tx_chain_1
     ( .clock(clk64),.reset(tx_dsp_reset),.enable(enable_tx),
       .interp_rate(interp_rate),.sample_strobe(tx_sample_strobe),
       .interpolator_strobe(strobe_interp),.freq(),
       .i_in(bb_tx_i1),.q_in(bb_tx_q1),.i_out(i_out_1),.q_out(q_out_1) );
 `else
   assign      i_out_1=16'd0;
   assign      q_out_1=16'd0;
 `endif

   setting_reg #(`FR_TX_MUX) 
     sr_txmux(.clock(clk64),.reset(tx_dsp_reset),.strobe(serial_strobe),.addr(serial_addr),.in(serial_data),
	      .out({dac3mux,dac2mux,dac1mux,dac0mux,tx_realsignals,tx_numchan}));
   
   wire [15:0] tx_a_a = dac0mux[3] ? (dac0mux[1] ? (dac0mux[0] ? q_out_1 : i_out_1) : (dac0mux[0] ? q_out_0 : i_out_0)) : 16'b0;
   wire [15:0] tx_b_a = dac1mux[3] ? (dac1mux[1] ? (dac1mux[0] ? q_out_1 : i_out_1) : (dac1mux[0] ? q_out_0 : i_out_0)) : 16'b0;
   wire [15:0] tx_a_b = dac2mux[3] ? (dac2mux[1] ? (dac2mux[0] ? q_out_1 : i_out_1) : (dac2mux[0] ? q_out_0 : i_out_0)) : 16'b0;
   wire [15:0] tx_b_b = dac3mux[3] ? (dac3mux[1] ? (dac3mux[0] ? q_out_1 : i_out_1) : (dac3mux[0] ? q_out_0 : i_out_0)) : 16'b0;

   wire txsync = tx_sample_strobe;
   assign TXSYNC_A = txsync;
   assign TXSYNC_B = txsync;

   assign tx_a = txsync ? tx_b_a[15:2] : tx_a_a[15:2];
   assign tx_b = txsync ? tx_b_b[15:2] : tx_a_b[15:2];
`endif //  `ifdef TX_ON
   
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // Receive Side
`ifdef RX_ON
   wire        rx_sample_strobe,strobe_decim,hb_strobe;
   wire [15:0] bb_rx_i0,bb_rx_q0,bb_rx_i1,bb_rx_q1,
	       bb_rx_i2,bb_rx_q2,bb_rx_i3,bb_rx_q3;

   wire loopback = settings[0];
   wire counter = settings[1];

   always @(posedge clk64)
     if(rx_dsp_reset)
       debug_counter <= #1 16'd0;
     else if(~enable_rx)
       debug_counter <= #1 16'd0;
     else if(hb_strobe)
       debug_counter <=#1 debug_counter + 16'd2;
   
   always @(posedge clk64)
     if(strobe_interp)
       begin
	  loopback_i_0 <= #1 ch0tx;
	  loopback_q_0 <= #1 ch1tx;
       end
   
   assign ch0rx = counter ? debug_counter : loopback ? loopback_i_0 : bb_rx_i0;
   assign ch1rx = counter ? debug_counter + 16'd1 : loopback ? loopback_q_0 : bb_rx_q0;
   assign ch2rx = bb_rx_i1;
   assign ch3rx = bb_rx_q1;
   assign ch4rx = bb_rx_i2;
   assign ch5rx = bb_rx_q2;
   assign ch6rx = bb_rx_i3;
   assign ch7rx = bb_rx_q3;

   wire [15:0] ddc0_in_i,ddc0_in_q,ddc1_in_i,ddc1_in_q,ddc2_in_i,ddc2_in_q,ddc3_in_i,ddc3_in_q;
   wire [31:0] rssi_0,rssi_1,rssi_2,rssi_3;
   adc_interface adc_interface(.clock(clk64),.reset(rx_dsp_reset),.enable(1'b1),
			       .serial_addr(serial_addr),.serial_data(serial_data),.serial_strobe(serial_strobe),
			       .rx_a_a(rx_a_a),.rx_b_a(rx_b_a),.rx_a_b(rx_a_b),.rx_b_b(rx_b_b),
			       .rssi_0(rssi_0),.rssi_1(rssi_1),.rssi_2(rssi_2),.rssi_3(rssi_3),
			       .ddc0_in_i(ddc0_in_i),.ddc0_in_q(ddc0_in_q),
			       .ddc1_in_i(ddc1_in_i),.ddc1_in_q(ddc1_in_q),
			       .ddc2_in_i(ddc2_in_i),.ddc2_in_q(ddc2_in_q),
			       .ddc3_in_i(ddc3_in_i),.ddc3_in_q(ddc3_in_q),.rx_numchan(rx_numchan));
   `ifdef RX_IN_BAND
   rx_buffer_inband rx_buffer
     ( .usbclk(usbclk),.bus_reset(rx_bus_reset),.reset(rx_dsp_reset),
       .reset_regs(rx_dsp_reset),
       .usbdata(usbdata_out),.RD(RD),.have_pkt_rdy(have_pkt_rdy),.rx_overrun(rx_overrun),
       .channels(rx_numchan),
       .ch_0(ch0rx),.ch_1(ch1rx),
       .ch_2(ch2rx),.ch_3(ch3rx),
       .ch_4(ch4rx),.ch_5(ch5rx),
       .ch_6(ch6rx),.ch_7(ch7rx),
       .rxclk(clk64),.rxstrobe(hb_strobe),
       .clear_status(clear_status),
	   .rx_WR(rx_WR),
	   .rx_databus(rx_databus),
	   .rx_WR_done(rx_WR_done),
	   .rx_WR_enabled(rx_WR_enabled),
	   .debugbus(tx_debugbus),
	   .rssi_0(rssi_0), .rssi_1(rssi_1), .rssi_2(rssi_2), .rssi_3(rssi_3),
	   .tx_underrun(tx_underrun));
    
    `ifdef RX_DUAL
      defparam rx_buffer.NUM_CHAN=2;
    `endif

   `else
   rx_buffer rx_buffer
     ( .usbclk(usbclk),.bus_reset(rx_bus_reset),.reset(rx_dsp_reset),
       .reset_regs(rx_dsp_reset),
       .usbdata(usbdata_out),.RD(RD),.have_pkt_rdy(have_pkt_rdy),.rx_overrun(rx_overrun),
       .channels(rx_numchan),
       .ch_0(ch0rx),.ch_1(ch1rx),
       .ch_2(ch2rx),.ch_3(ch3rx),
       .ch_4(ch4rx),.ch_5(ch5rx),
       .ch_6(ch6rx),.ch_7(ch7rx),
       .rxclk(clk64),.rxstrobe(hb_strobe),
       .clear_status(clear_status),
       .serial_addr(serial_addr),.serial_data(serial_data),.serial_strobe(serial_strobe));
   `endif
   
 `ifdef RX_EN_0
   rx_chain #(`FR_RX_FREQ_0,`FR_RX_PHASE_0) rx_chain_0
     ( .clock(clk64),.reset(1'b0),.enable(enable_rx),
       .decim_rate(decim_rate),.sample_strobe(rx_sample_strobe),.decimator_strobe(strobe_decim),.hb_strobe(hb_strobe),
       .serial_addr(serial_addr),.serial_data(serial_data),.serial_strobe(serial_strobe),
       .i_in(ddc0_in_i),.q_in(ddc0_in_q),.i_out(bb_rx_i0),.q_out(bb_rx_q0),.debugdata(debugdata),.debugctrl(debugctrl));
 `else
   assign      bb_rx_i0=16'd0;
   assign      bb_rx_q0=16'd0;
 `endif
   
 `ifdef RX_EN_1
   rx_chain #(`FR_RX_FREQ_1,`FR_RX_PHASE_1) rx_chain_1
     ( .clock(clk64),.reset(1'b0),.enable(enable_rx),
       .decim_rate(decim_rate),.sample_strobe(rx_sample_strobe),.decimator_strobe(strobe_decim),.hb_strobe(),
       .serial_addr(serial_addr),.serial_data(serial_data),.serial_strobe(serial_strobe),
       .i_in(ddc1_in_i),.q_in(ddc1_in_q),.i_out(bb_rx_i1),.q_out(bb_rx_q1));
 `else
   assign      bb_rx_i1=16'd0;
   assign      bb_rx_q1=16'd0;
 `endif
   
 `ifdef RX_EN_2
   rx_chain #(`FR_RX_FREQ_2,`FR_RX_PHASE_2) rx_chain_2
     ( .clock(clk64),.reset(1'b0),.enable(enable_rx),
       .decim_rate(decim_rate),.sample_strobe(rx_sample_strobe),.decimator_strobe(strobe_decim),.hb_strobe(),
       .serial_addr(serial_addr),.serial_data(serial_data),.serial_strobe(serial_strobe),
       .i_in(ddc2_in_i),.q_in(ddc2_in_q),.i_out(bb_rx_i2),.q_out(bb_rx_q2));
 `else
   assign      bb_rx_i2=16'd0;
   assign      bb_rx_q2=16'd0;
 `endif

 `ifdef RX_EN_3
   rx_chain #(`FR_RX_FREQ_3,`FR_RX_PHASE_3) rx_chain_3
     ( .clock(clk64),.reset(1'b0),.enable(enable_rx),
       .decim_rate(decim_rate),.sample_strobe(rx_sample_strobe),.decimator_strobe(strobe_decim),.hb_strobe(),
       .serial_addr(serial_addr),.serial_data(serial_data),.serial_strobe(serial_strobe),
       .i_in(ddc3_in_i),.q_in(ddc3_in_q),.i_out(bb_rx_i3),.q_out(bb_rx_q3));
 `else
   assign      bb_rx_i3=16'd0;
   assign      bb_rx_q3=16'd0;
 `endif

`endif //  `ifdef RX_ON
   
   ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // Control Functions

   wire [31:0] capabilities;
   assign      capabilities[7] =   `TX_CAP_HB;
   assign      capabilities[6:4] = `TX_CAP_NCHAN;
   assign      capabilities[3] =   `RX_CAP_HB;
   assign      capabilities[2:0] = `RX_CAP_NCHAN;

   serial_io serial_io
     ( .master_clk(clk64),.serial_clock(SCLK),.serial_data_in(SDI),
       .enable(SEN_FPGA),.reset(1'b0),.serial_data_out(SDO),
       .serial_addr(addr_db),.serial_data(data_db),.serial_strobe(strobe_db),
       .readback_0({io_rx_a,io_tx_a}),.readback_1({io_rx_b,io_tx_b}),.readback_2(capabilities),.readback_3(32'hf0f0931a),
       .readback_4(rssi_0),.readback_5(rssi_1),.readback_6(rssi_2),.readback_7(rssi_3)
       );

   wire [6:0] reg_addr;
   wire [31:0] reg_data_out;
   wire [31:0] reg_data_in;
   wire [1:0] reg_io_enable;
   wire [31:0] rssi_threshhold;
   wire [31:0] rssi_wait;
   wire [6:0] addr_wr;
   wire [31:0] data_wr;
   wire strobe_wr;
   wire [6:0] addr_db;
   wire [31:0] data_db;
   wire strobe_db;
   assign serial_strobe = strobe_db | strobe_wr;
   assign serial_addr = (strobe_db)? (addr_db) : (addr_wr);
   assign serial_data = (strobe_db)? (data_db) : (data_wr);	
   //assign serial_strobe = strobe_wr;
   //assign serial_data = data_wr;
   //assign serial_addr = addr_wr;

   register_io register_control
    (.clk(clk64),.reset(1'b0),.enable(reg_io_enable),.addr(reg_addr),.datain(reg_data_in),
     .dataout(reg_data_out), .addr_wr(addr_wr), .data_wr(data_wr), .strobe_wr(strobe_wr),
     .rssi_0(rssi_0), .rssi_1(rssi_1), .rssi_2(rssi_2), 
     .rssi_3(rssi_3), .threshhold(rssi_threshhold), .rssi_wait(rssi_wait),
	 .reg_0(reg_0),.reg_1(reg_1),.reg_2(reg_2),.reg_3(reg_3),
     .debug_en(debug_en), .misc(settings), 
	 .txmux({dac3mux,dac2mux,dac1mux,dac0mux,tx_realsignals,tx_numchan}));
   
   
   //implementing freeze mode
   reg [15:0] timestop;
   wire stop;
   wire [15:0] stop_time;
   assign	clk64 = (timestop == 0) ? master_clk : 0;
   always @(posedge master_clk)
		if (timestop[15:0] != 0)
			timestop <= timestop - 16'd1;
		else if (stop)
			timestop <= stop_time;
						

   wire [15:0] reg_0,reg_1,reg_2,reg_3;
   master_control master_control
     ( .master_clk(clk64),.usbclk(usbclk),
       .serial_addr(serial_addr),.serial_data(serial_data),.serial_strobe(serial_strobe),
       .tx_bus_reset(tx_bus_reset),.rx_bus_reset(rx_bus_reset),
       .tx_dsp_reset(tx_dsp_reset),.rx_dsp_reset(rx_dsp_reset),
       .enable_tx(enable_tx),.enable_rx(enable_rx),
       .interp_rate(interp_rate),.decim_rate(decim_rate),
       .tx_sample_strobe(tx_sample_strobe),.strobe_interp(strobe_interp),
       .rx_sample_strobe(rx_sample_strobe),.strobe_decim(strobe_decim),
       .tx_empty(tx_empty),
       //.debug_0(rx_a_a),.debug_1(ddc0_in_i),
       .debug_0(rx_debugbus),.debug_1(ddc0_in_i),
       .debug_2({rx_sample_strobe,strobe_decim,serial_strobe,serial_addr}),.debug_3({rx_dsp_reset,tx_dsp_reset,rx_bus_reset,tx_bus_reset,enable_rx,tx_underrun,rx_overrun,decim_rate}),
       .reg_0(reg_0),.reg_1(reg_1),.reg_2(reg_2),.reg_3(reg_3) );
   
   io_pins io_pins
     (.io_0(io_tx_a),.io_1(io_rx_a),.io_2(io_tx_b),.io_3(io_rx_b),
      .reg_0(reg_0),.reg_1(reg_1),.reg_2(reg_2),.reg_3(reg_3),
      .clock(clk64),.rx_reset(rx_dsp_reset),.tx_reset(tx_dsp_reset),
      .serial_addr(serial_addr),.serial_data(serial_data),.serial_strobe(serial_strobe));
   
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // Misc Settings
   setting_reg #(`FR_MODE) sr_misc(.clock(clk64),.reset(rx_dsp_reset),.strobe(serial_strobe),.addr(serial_addr),.in(serial_data),.out(settings));

endmodule // usrp_inband_usb
