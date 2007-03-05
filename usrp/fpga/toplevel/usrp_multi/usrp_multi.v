// -*- verilog -*-
//
//  USRP - Universal Software Radio Peripheral
//
//  Copyright (C) 2003,2004,2005,2006 Matt Ettus
//  Copyright (C) 2006 Martin Dudok van Heel
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

`include "config.vh"
`include "../../../firmware/include/fpga_regs_common.v"
`include "../../../firmware/include/fpga_regs_standard.v"

module usrp_multi
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
   wire [15:0] tx_debugbus, rx_debugbus;
   
   wire        enable_tx, enable_rx;
   wire        reset_data;
`ifdef MULTI_ON
   wire        sync_rx;
   assign reset_data = sync_rx;
`else
   assign reset_data = 1'b0;
`endif //  `ifdef MULTI_ON

   wire        tx_dsp_reset, rx_dsp_reset, tx_bus_reset, rx_bus_reset;
   wire [7:0]  settings;
   
   // Tri-state bus macro
   bustri bustri( .data(usbdata_out),.enabledt(OE),.tridata(usbdata) );

   assign      clk64 = master_clk;

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
`ifdef COUNTER_32BIT_ON
   reg [31:0] sample_counter_32bit;
`endif //  `ifdef COUNTER_32BIT_ON
   reg [15:0] loopback_i_0,loopback_q_0;
   
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // Transmit Side
`ifdef TX_ON
   assign     bb_tx_i0 = ch0tx;
   assign      bb_tx_q0 = ch1tx;
   assign      bb_tx_i1 = ch2tx;
   assign      bb_tx_q1 = ch3tx;
   
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
       .tx_empty(tx_empty),
       .debugbus(tx_debugbus) );

   tx_chain tx_chain_0
     ( .clock(clk64),.reset(tx_dsp_reset),.enable(enable_tx),
       .interp_rate(interp_rate),.sample_strobe(tx_sample_strobe),
       .interpolator_strobe(strobe_interp),.freq(),
       .i_in(bb_tx_i0),.q_in(bb_tx_q0),.i_out(i_out_0),.q_out(q_out_0) );

   tx_chain tx_chain_1
     ( .clock(clk64),.reset(tx_dsp_reset),.enable(enable_tx),
       .interp_rate(interp_rate),.sample_strobe(tx_sample_strobe),
       .interpolator_strobe(strobe_interp),.freq(),
       .i_in(bb_tx_i1),.q_in(bb_tx_q1),.i_out(i_out_1),.q_out(q_out_1) );

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
`ifdef COUNTER_32BIT_ON
   wire counter_32bit = settings[2];

   always @(posedge clk64)
     if(rx_dsp_reset)
       sample_counter_32bit <= #1 32'd0;
     else if(~enable_rx | reset_data)
       sample_counter_32bit <=#1 32'd0;
     else if(hb_strobe)
       sample_counter_32bit <=#1 sample_counter_32bit + 32'd1;
`endif //  `ifdef COUNTER_32BIT_ON

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
   
`ifdef COUNTER_32BIT_ON
   assign ch0rx = counter_32bit?sample_counter_32bit[31:16]:counter ? debug_counter : loopback ? loopback_i_0 : bb_rx_i0;
   assign ch1rx = counter_32bit?sample_counter_32bit[15:0]:counter ? debug_counter + 16'd1 : loopback ? loopback_q_0 : bb_rx_q0;
   assign ch2rx = bb_rx_i1;
   assign ch3rx = bb_rx_q1;
   assign ch4rx = counter_32bit?bb_rx_i0:bb_rx_i2;
   assign ch5rx = counter_32bit?bb_rx_q0:bb_rx_q2;// If using counter replicate channels here to be able to get rx_i0 when using counter
                           //This means if you use 4 channels that channel 3 will be replaced by channel 0
                           // and channel 0 will output the 32 bit counter.
   assign ch6rx = bb_rx_i3;
   assign ch7rx = bb_rx_q3;
`else
   assign ch0rx = counter ? debug_counter : loopback ? loopback_i_0 : bb_rx_i0;
   assign ch1rx = counter ? debug_counter + 16'd1 : loopback ? loopback_q_0 : bb_rx_q0;
   assign ch2rx = bb_rx_i1;
   assign ch3rx = bb_rx_q1;
   assign ch4rx = bb_rx_i2;
   assign ch5rx = bb_rx_q2;
   assign ch6rx = bb_rx_i3;
   assign ch7rx = bb_rx_q3;
`endif //  `ifdef COUNTER_32BIT_ON


   wire [15:0] ddc0_in_i,ddc0_in_q,ddc1_in_i,ddc1_in_q,ddc2_in_i,ddc2_in_q,ddc3_in_i,ddc3_in_q;
   adc_interface adc_interface(.clock(clk64),.reset(rx_dsp_reset),.enable(1'b1),
			       .serial_addr(serial_addr),.serial_data(serial_data),.serial_strobe(serial_strobe),
			       .rx_a_a(rx_a_a),.rx_b_a(rx_b_a),.rx_a_b(rx_a_b),.rx_b_b(rx_b_b),
			       .ddc0_in_i(ddc0_in_i),.ddc0_in_q(ddc0_in_q),
			       .ddc1_in_i(ddc1_in_i),.ddc1_in_q(ddc1_in_q),
			       .ddc2_in_i(ddc2_in_i),.ddc2_in_q(ddc2_in_q),
			       .ddc3_in_i(ddc3_in_i),.ddc3_in_q(ddc3_in_q),.rx_numchan(rx_numchan) );
   
   rx_buffer rx_buffer
     ( .usbclk(usbclk),.bus_reset(rx_bus_reset),.reset(rx_dsp_reset | reset_data),
       .reset_regs(rx_dsp_reset),
       .usbdata(usbdata_out),.RD(RD),.have_pkt_rdy(have_pkt_rdy),.rx_overrun(rx_overrun),
       .channels(rx_numchan),
       .ch_0(ch0rx),.ch_1(ch1rx),
       .ch_2(ch2rx),.ch_3(ch3rx),
       .ch_4(ch4rx),.ch_5(ch5rx),
       .ch_6(ch6rx),.ch_7(ch7rx),
       .rxclk(clk64),.rxstrobe(hb_strobe),
       .clear_status(clear_status),
       .serial_addr(serial_addr),.serial_data(serial_data),.serial_strobe(serial_strobe),
       .debugbus(rx_debugbus) );
   
 `ifdef RX_EN_0
   rx_chain #(`FR_RX_FREQ_0,`FR_RX_PHASE_0) rx_chain_0
     ( .clock(clk64),.reset(reset_data),.enable(enable_rx),
       .decim_rate(decim_rate),.sample_strobe(rx_sample_strobe),.decimator_strobe(strobe_decim),.hb_strobe(hb_strobe),
       .serial_addr(serial_addr),.serial_data(serial_data),.serial_strobe(serial_strobe),
       .i_in(ddc0_in_i),.q_in(ddc0_in_q),.i_out(bb_rx_i0),.q_out(bb_rx_q0),.debugdata(debugdata),.debugctrl(debugctrl));
 `else
   assign      bb_rx_i0=16'd0;
   assign      bb_rx_q0=16'd0;
 `endif
   
 `ifdef RX_EN_1
   rx_chain #(`FR_RX_FREQ_1,`FR_RX_PHASE_1) rx_chain_1
     ( .clock(clk64),.reset(reset_data),.enable(enable_rx),
       .decim_rate(decim_rate),.sample_strobe(rx_sample_strobe),.decimator_strobe(strobe_decim),.hb_strobe(),
       .serial_addr(serial_addr),.serial_data(serial_data),.serial_strobe(serial_strobe),
       .i_in(ddc1_in_i),.q_in(ddc1_in_q),.i_out(bb_rx_i1),.q_out(bb_rx_q1));
 `else
   assign      bb_rx_i1=16'd0;
   assign      bb_rx_q1=16'd0;
 `endif
   
 `ifdef RX_EN_2
   rx_chain #(`FR_RX_FREQ_2,`FR_RX_PHASE_2) rx_chain_2
     ( .clock(clk64),.reset(reset_data),.enable(enable_rx),
       .decim_rate(decim_rate),.sample_strobe(rx_sample_strobe),.decimator_strobe(strobe_decim),.hb_strobe(),
       .serial_addr(serial_addr),.serial_data(serial_data),.serial_strobe(serial_strobe),
       .i_in(ddc2_in_i),.q_in(ddc2_in_q),.i_out(bb_rx_i2),.q_out(bb_rx_q2));
 `else
   assign      bb_rx_i2=16'd0;
   assign      bb_rx_q2=16'd0;
 `endif

 `ifdef RX_EN_3
   rx_chain #(`FR_RX_FREQ_3,`FR_RX_PHASE_3) rx_chain_3
     ( .clock(clk64),.reset(reset_data),.enable(enable_rx),
       .decim_rate(decim_rate),.sample_strobe(rx_sample_strobe),.decimator_strobe(strobe_decim),.hb_strobe(),
       .serial_addr(serial_addr),.serial_data(serial_data),.serial_strobe(serial_strobe),
       .i_in(ddc3_in_i),.q_in(ddc3_in_q),.i_out(bb_rx_i3),.q_out(bb_rx_q3));
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
       .serial_addr(serial_addr),.serial_data(serial_data),.serial_strobe(serial_strobe),
       .readback_0({io_rx_a,io_tx_a}),.readback_1({io_rx_b,io_tx_b}),.readback_2(capabilities),.readback_3(32'hf0f0931a) );

   wire [15:0] reg_0,reg_1,reg_2,reg_3;

`ifdef MULTI_ON

   master_control_multi master_control
     ( .master_clk(clk64),.usbclk(usbclk),
       .serial_addr(serial_addr),.serial_data(serial_data),.serial_strobe(serial_strobe),
       .rx_slave_sync(io_rx_a[`bitnoFR_RX_SYNC_INPUT_IOPIN]),
       .tx_bus_reset(tx_bus_reset),.rx_bus_reset(rx_bus_reset),
       .tx_dsp_reset(tx_dsp_reset),.rx_dsp_reset(rx_dsp_reset),
       .enable_tx(enable_tx),.enable_rx(enable_rx),
       .sync_rx(sync_rx),
       .interp_rate(interp_rate),.decim_rate(decim_rate),
       .tx_sample_strobe(tx_sample_strobe),.strobe_interp(strobe_interp),
       .rx_sample_strobe(rx_sample_strobe),.strobe_decim(strobe_decim),
       .tx_empty(tx_empty),
       //.debug_0(rx_a_a),.debug_1(ddc0_in_i),
       .debug_0(rx_debugbus),.debug_1(ddc0_in_i),
       .debug_2({rx_sample_strobe,strobe_decim,serial_strobe,serial_addr}),.debug_3({rx_dsp_reset,tx_dsp_reset,rx_bus_reset,tx_bus_reset,enable_rx,tx_underrun,rx_overrun,decim_rate}),
       .reg_0(reg_0),.reg_1(reg_1),.reg_2(reg_2),.reg_3(reg_3) );

`else //`ifdef MULTI_ON

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

`endif //`ifdef MULTI_ON   

   io_pins io_pins
     (.io_0(io_tx_a),.io_1(io_rx_a),.io_2(io_tx_b),.io_3(io_rx_b),
      .reg_0(reg_0),.reg_1(reg_1),.reg_2(reg_2),.reg_3(reg_3),
      .clock(clk64),.rx_reset(rx_dsp_reset),.tx_reset(tx_dsp_reset),
      .serial_addr(serial_addr),.serial_data(serial_data),.serial_strobe(serial_strobe));
   
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // Misc Settings
   setting_reg #(`FR_MODE) sr_misc(.clock(clk64),.reset(rx_dsp_reset),.strobe(serial_strobe),.addr(serial_addr),.in(serial_data),.out(settings));

endmodule // usrp_multi
