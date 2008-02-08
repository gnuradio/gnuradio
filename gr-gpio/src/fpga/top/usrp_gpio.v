// -*- verilog -*-
//
//  USRP - Universal Software Radio Peripheral
//
//  Copyright (C) 2003,2004 Matt Ettus
//  Copyright (C) 2008 Corgan Enterprises LLC
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

// Top level module for a full setup with DUCs and DDCs

// Define DEBUG_OWNS_IO_PINS if we're using the daughterboard i/o pins
// for debugging info.  NB, This can kill the m'board and/or d'board if you
// have anything except basic d'boards installed.

// Uncomment the following to include optional circuitry

`include "../top/config.vh"
`include "../../../../usrp/firmware/include/fpga_regs_common.v"
`include "../../../../usrp/firmware/include/fpga_regs_standard.v"

module usrp_gpio
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

   wire   tx_underrun, rx_overrun;    
   wire   clear_status = FX2_1;
   assign FX2_2 = rx_overrun;
   assign FX2_3 = tx_underrun;
      
   wire [15:0] usbdata_out;
   
   wire [3:0]  dac0mux,dac1mux,dac2mux,dac3mux;
   
   wire        tx_realsignals;
   wire [3:0]  rx_numchan;
   wire [2:0]  tx_numchan;
   
   wire [7:0]  interp_rate, decim_rate;
   wire [31:0] tx_debugbus, rx_debugbus;
   
   wire        enable_tx, enable_rx;
   wire        tx_dsp_reset, rx_dsp_reset, tx_bus_reset, rx_bus_reset;
   wire [7:0]  settings;
   
   // Tri-state bus macro
   bustri bustri( .data(usbdata_out),.enabledt(OE),.tridata(usbdata) );

   assign      clk64 = master_clk;

   wire [15:0] ch0tx,ch1tx,ch2tx,ch3tx; //,ch4tx,ch5tx,ch6tx,ch7tx;
   wire [15:0] ch0rx,ch1rx,ch2rx,ch3rx,ch4rx,ch5rx,ch6rx,ch7rx;
   wire [15:0] ch0rx_ext,ch1rx_ext;  
 
   // TX
   wire [15:0] i_out_0,i_out_1,q_out_0,q_out_1;//analog signals
   wire [15:0] bb_tx_i0,bb_tx_q0,bb_tx_i1,bb_tx_q1;  // bb_tx_i2,bb_tx_q2,bb_tx_i3,bb_tx_q3;
   
   wire        strobe_interp, tx_sample_strobe;
   wire        tx_empty;
   
   wire        serial_strobe;
   wire [6:0]  serial_addr;
   wire [31:0] serial_data;

   reg [15:0] debug_counter;
   reg [15:0] loopback_i_0,loopback_q_0;

  //TX_DIG streaming digital IO signals
   wire i_out_dig_0,i_out_dig_1,q_out_dig_0,q_out_dig_1;
   wire rx_dig0_i, rx_dig0_q,rx_dig1_i,rx_dig1_q;
   
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // Transmit Side
`ifdef TX_ON
   
   tx_buffer tx_buffer
     ( .usbclk(usbclk), .bus_reset(tx_bus_reset),
       .usbdata(usbdata),.WR(WR), .have_space(have_space),
       .tx_underrun(tx_underrun), .clear_status(clear_status),
       .txclk(clk64), .reset(tx_dsp_reset),
       .channels({tx_numchan,1'b0}),
       .tx_i_0(ch0tx),.tx_q_0(ch1tx),
       .tx_i_1(ch2tx),.tx_q_1(ch3tx),
       .txstrobe(strobe_interp),
       .tx_empty(tx_empty),
       .debugbus(tx_debugbus) );
   
 `ifdef TX_EN_0
   tx_chain tx_chain_0
     ( .clock(clk64),.reset(tx_dsp_reset),.enable(enable_tx),
       .interp_rate(interp_rate),.sample_strobe(tx_sample_strobe),
       .interpolator_strobe(strobe_interp),.freq(),
       .i_in(bb_tx_i0),.q_in(bb_tx_q0),.i_out(i_out_0),.q_out(q_out_0));
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

   wire  tx_dig_a_a = (dac0mux[1] ? (dac0mux[0] ? q_out_dig_1 : i_out_dig_1) : (dac0mux[0] ? q_out_dig_0 : i_out_dig_0));
   wire  tx_dig_b_a = (dac1mux[1] ? (dac1mux[0] ? q_out_dig_1 : i_out_dig_1) : (dac1mux[0] ? q_out_dig_0 : i_out_dig_0));
   wire  tx_dig_a_b = (dac2mux[1] ? (dac2mux[0] ? q_out_dig_1 : i_out_dig_1) : (dac2mux[0] ? q_out_dig_0 : i_out_dig_0));
   wire  tx_dig_b_b = (dac3mux[1] ? (dac3mux[0] ? q_out_dig_1 : i_out_dig_1) : (dac3mux[0] ? q_out_dig_0 : i_out_dig_0));

   //wire [1:0] tx_dig_a = {tx_dig_a_a,tx_dig_b_a};
   //wire [1:0] tx_dig_b = {tx_dig_a_b,tx_dig_b_b};

   //wire tx_dig_a_chan = (dac0mux[1] | dac1mux[1] );
   //wire tx_dig_b_chan = (dac2mux[1] | dac3mux[1] );

   //TODO make enabling tx_dig configurable through register
 
   wire enable_tx_dig_a = 1'b1 & enable_tx;
   wire enable_tx_dig_b = 1'b1 & enable_tx;

   wire tx_dig_a_a_en = dac0mux[3] & enable_tx_dig_a;
   wire tx_dig_b_a_en = dac1mux[3] & enable_tx_dig_a;
   wire tx_dig_a_b_en = dac2mux[3] & enable_tx_dig_b;
   wire tx_dig_b_b_en = dac3mux[3] & enable_tx_dig_b;

  //TODO make gpio bits used for tx_dig configurable through register
   assign io_tx_a_out = {tx_dig_a_a_en?tx_dig_a_a:reg_0[15],tx_dig_b_a_en?tx_dig_b_a:reg_0[14],reg_0[13:0]};
   assign io_tx_b_out = {tx_dig_a_b_en?tx_dig_a_b:reg_2[15],tx_dig_b_b_en?tx_dig_b_b:reg_2[14],reg_2[13:0]};
   assign io_tx_a_force_output = {tx_dig_a_a_en,tx_dig_b_a_en,14'b0};
   assign io_tx_b_force_output = {tx_dig_a_b_en,tx_dig_b_b_en,14'b0};


 `ifdef TX_EN_DIG_0
   //TODO make enabling tx_dig configurable through register
   //tx_chain_dig tx_chain_dig_0
   //  ( .clock(clk64),.reset(tx_dsp_reset),.enable(enable_tx),
   //    .i_in(ch0tx), q_in(ch1tx),
   //    .i_out_ana(bb_tx_i0),
   //    .q_out_ana(bb_tx_q0),
   //    .i_out_dig(i_out_dig_0),
   //    .q_out_dig(q_out_dig_0)
   //   );
   tx_chain_dig tx_chain_dig_0
     ( .clock(clk64),.reset(tx_dsp_reset),.enable(enable_tx),
       .i_in(ch0tx),.q_in(ch1tx),
       .i_out_ana(bb_tx_i0),.q_out_ana(bb_tx_q0),
       .i_out_dig(i_out_dig_0),.q_out_dig(q_out_dig_0));
 `else
   assign      bb_tx_i0 = ch0tx;
   assign      bb_tx_q0 = ch1tx;
   assign      i_out_dig_0=1'b0;
   assign      q_out_dig_0=1'b0;
 `endif

 `ifdef TX_EN_DIG_1
   //TODO make enabling tx_dig configurable through register
   tx_chain_dig tx_chain_dig_1
     ( .clock(clk64),.reset(tx_dsp_reset),.enable(enable_tx),
       .i_in(ch2tx),.q_in(ch3tx),
       .i_out_ana(bb_tx_i1),.q_out_ana(bb_tx_q1),
       .i_out_dig(i_out_dig_1),.q_out_dig(q_out_dig_1));
//   tx_chain_dig tx_chain_dig_1
//     ( .clock(clk64),.reset(tx_dsp_reset),.enable(enable_tx),
//       .i_in(ch2tx), q_in(ch3tx),
//       .i_out_ana(bb_tx_i1),
//       .q_out_ana(bb_tx_q1),
//       .i_out_dig(i_out_dig_1),
//       .q_out_dig(q_out_dig_1)
//      );
 `else
   assign      bb_tx_i1 = ch2tx;
   assign      bb_tx_q1 = ch3tx;
   assign      i_out_dig_1=1'b0;
   assign      q_out_dig_1=1'b0;
 `endif

   wire txsync = tx_sample_strobe;
   assign TXSYNC_A = txsync;
   assign TXSYNC_B = txsync;

   assign tx_a = txsync ? tx_b_a[15:2] : tx_a_a[15:2];
   assign tx_b = txsync ? tx_b_b[15:2] : tx_a_b[15:2];
`else //  `ifdef TX_ON
   assign      io_tx_a_out = reg_0;
   assign      io_tx_b_out = reg_2;
   assign      io_tx_a_force_output=16'b0;
   assign      io_tx_b_force_output=16'b0;
`endif   
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


   wire [15:0] ddc0_in_i,ddc0_in_q,ddc1_in_i,ddc1_in_q,ddc2_in_i,ddc2_in_q,ddc3_in_i,ddc3_in_q;
   wire [31:0] rssi_0,rssi_1,rssi_2,rssi_3;
   
   adc_interface adc_interface(.clock(clk64),.reset(rx_dsp_reset),.enable(1'b1),
			       .serial_addr(serial_addr),.serial_data(serial_data),.serial_strobe(serial_strobe),
			       .rx_a_a(rx_a_a),.rx_b_a(rx_b_a),.rx_a_b(rx_a_b),.rx_b_b(rx_b_b),
			       .rssi_0(rssi_0),.rssi_1(rssi_1),.rssi_2(rssi_2),.rssi_3(rssi_3),
			       .ddc0_in_i(ddc0_in_i),.ddc0_in_q(ddc0_in_q),
			       .ddc1_in_i(ddc1_in_i),.ddc1_in_q(ddc1_in_q),
			       .ddc2_in_i(ddc2_in_i),.ddc2_in_q(ddc2_in_q),
			       .ddc3_in_i(ddc3_in_i),.ddc3_in_q(ddc3_in_q),.rx_numchan(rx_numchan) );
   
   rx_buffer rx_buffer
     ( .usbclk(usbclk),.bus_reset(rx_bus_reset),.reset(rx_dsp_reset),
       .reset_regs(rx_dsp_reset),
       .usbdata(usbdata_out),.RD(RD),.have_pkt_rdy(have_pkt_rdy),.rx_overrun(rx_overrun),
       .channels(rx_numchan),
       .ch_0(ch0rx_ext),.ch_1(ch1rx_ext),
       .ch_2(ch2rx),.ch_3(ch3rx),
       .ch_4(ch4rx),.ch_5(ch5rx),
       .ch_6(ch6rx),.ch_7(ch7rx),
       .rxclk(clk64),.rxstrobe(hb_strobe),
       .clear_status(clear_status),
       .serial_addr(serial_addr),.serial_data(serial_data),.serial_strobe(serial_strobe),
       .debugbus(rx_debugbus) );
   
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

 `ifdef RX_DIG_ON
   wire enable_rx_dig = 1'b1 & enable_rx;//TODO make  enabling rx_dig configurable through register
   assign      io_rx_a_force_input = {enable_rx_dig,enable_rx_dig,14'b0};
   assign      io_rx_b_force_input = {enable_rx_dig,enable_rx_dig,14'b0};
   gpio_input  gpio_input(.clock(clk64),.reset(rx_dsp_reset),.enable(1'b1),
                          .out_strobe(hb_strobe),
			  .serial_addr(serial_addr),.serial_data(serial_data),.serial_strobe(serial_strobe),
			  .io_rx_a_in(io_rx_a),.io_rx_b_in(io_rx_b),
                          //.io_tx_a_in(io_tx_a),.io_tx_b_in(io_tx_b),
			  .rx_dig0_i(rx_dig0_i),.rx_dig0_q(rx_dig0_q),
			  .rx_dig1_i(rx_dig1_i),.rx_dig1_q(rx_dig1_q) );

  `ifdef RX_EN_DIG_0
    rx_chain_dig rx_chain_dig_0
     ( .clock(clk64),.reset(rx_dsp_reset),.enable(enable_rx_dig),
       .i_in_ana(bb_rx_i0),.q_in_ana(bb_rx_q0),
       .i_in_dig(rx_dig0_i),.q_in_dig(rx_dig0_q),
       .i_out(ch0rx),.q_out(ch1rx));
  `else
    assign      ch0rx = bb_rx_i0;
    assign      ch1rx = bb_rx_q0;
  `endif

   assign ch0rx_ext = counter ? debug_counter : loopback ? loopback_i_0 : ch0rx;
   assign ch1rx_ext = counter ? debug_counter + 16'd1 : loopback ? loopback_q_0 : ch1rx;

  `ifdef RX_EN_DIG_1
    rx_chain_dig rx_chain_dig_1
     ( .clock(clk64),.reset(rx_dsp_reset),.enable(enable_rx_dig),
       .i_in_ana(bb_rx_i1),.q_in_ana(bb_rx_q1),
       .i_in_dig(rx_dig1_i),.q_in_dig(rx_dig1_q),
       .i_out(ch2rx),.q_out(ch3rx));
  `else
    assign      ch2rx = bb_rx_i1;
    assign      ch3rx = bb_rx_q1;
  `endif

   assign ch4rx = bb_rx_i2;
   assign ch5rx = bb_rx_q2;
   assign ch6rx = bb_rx_i3;
   assign ch7rx = bb_rx_q3;
 `else //  `ifdef RX_DIG_ON
   assign ch0rx = counter ? debug_counter : loopback ? loopback_i_0 : bb_rx_i0;
   assign ch1rx = counter ? debug_counter + 16'd1 : loopback ? loopback_q_0 : bb_rx_q0;
   assign ch2rx = bb_rx_i1;
   assign ch3rx = bb_rx_q1;
   assign ch4rx = bb_rx_i2;
   assign ch5rx = bb_rx_q2;
   assign ch6rx = bb_rx_i3;
   assign ch7rx = bb_rx_q3;
 `endif //  `ifdef RX_DIG_ON

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
       .serial_addr(serial_addr),.serial_data(serial_data),.serial_strobe(serial_strobe),
       .readback_0({io_rx_a,io_tx_a}),.readback_1({io_rx_b,io_tx_b}),.readback_2(capabilities),.readback_3(32'hf0f0931a),
       .readback_4(rssi_0),.readback_5(rssi_1),.readback_6(rssi_2),.readback_7(rssi_3)
       );

   wire [15:0] reg_0,reg_1,reg_2,reg_3;
   wire [15:0] io_tx_a_out;
   wire [15:0] io_tx_b_out;
   wire [15:0] io_tx_a_force_output;
   wire [15:0] io_tx_b_force_output; 
   wire [15:0] io_rx_a_force_input;
   wire [15:0] io_rx_b_force_input;

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
       .debug_0(tx_debugbus[15:0]),.debug_1(tx_debugbus[31:16]),
       .debug_2(rx_debugbus[15:0]),.debug_3(rx_debugbus[31:16]),
       //.tx_dig_a(tx_dig_a),tx_dig_b(tx_dig_b),
       .reg_0(reg_0),.reg_1(reg_1),.reg_2(reg_2),.reg_3(reg_3) );
  
   io_pins io_pins
     (.io_0(io_tx_a),.io_1(io_rx_a),.io_2(io_tx_b),.io_3(io_rx_b),
      .reg_0(io_tx_a_out),.reg_1(reg_1),.reg_2(io_tx_b_out),.reg_3(reg_3),
      .io_0_force_output(io_tx_a_force_output), .io_2_force_output(io_tx_b_force_output), 
      .io_1_force_input(io_rx_a_force_input),   .io_3_force_input(io_rx_b_force_input),
      .clock(clk64),.rx_reset(rx_dsp_reset),.tx_reset(tx_dsp_reset),
      .serial_addr(serial_addr),.serial_data(serial_data),.serial_strobe(serial_strobe));
   
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // Misc Settings
   setting_reg #(`FR_MODE) sr_misc(.clock(clk64),.reset(rx_dsp_reset),.strobe(serial_strobe),.addr(serial_addr),.in(serial_data),.out(settings));

endmodule // usrp_gpio
