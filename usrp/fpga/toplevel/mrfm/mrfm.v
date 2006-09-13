// -*- verilog -*-
//
//  USRP - Universal Software Radio Peripheral
//
//  Copyright (C) 2006 Matt Ettus
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

// Uncomment the following to include optional circuitry

`include "mrfm.vh"
`include "../../../firmware/include/fpga_regs_common.v"
`include "../../../firmware/include/fpga_regs_standard.v"

module mrfm
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
   
   wire clk64;
   
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
   
   wire [15:0] tx_debugbus, rx_debugbus;
   
   wire        enable_tx, enable_rx;
   wire        tx_dsp_reset, rx_dsp_reset, tx_bus_reset, rx_bus_reset;
   wire [7:0]  settings;
   
   // Tri-state bus macro
   bustri bustri( .data(usbdata_out),.enabledt(OE),.tridata(usbdata) );

   assign      clk64 = master_clk;

   wire [15:0] ch0tx,ch1tx,ch2tx,ch3tx; 
   wire [15:0] ch0rx,ch1rx,ch2rx,ch3rx,ch4rx,ch5rx,ch6rx,ch7rx;
   
   wire        serial_strobe;
   wire [6:0]  serial_addr;
   wire [31:0] serial_data;
   
   /////////////////////////////////////////////////////////////////////////////////////////////////////
   
   setting_reg #(`FR_TX_MUX) 
     sr_txmux(.clock(clk64),.reset(tx_dsp_reset),.strobe(serial_strobe),.addr(serial_addr),.in(serial_data),
	      .out({dac3mux,dac2mux,dac1mux,dac0mux,tx_realsignals,tx_numchan}));
      
   //////////////////////////////////////////////////////////////////////////////////////////////////////
   // Signal Processing Chain

   reg [15:0]  adc0;
   wire [15:0] dac0;
   wire [15:0] i,q,ip,qp;
   wire        strobe_out;
   wire        sync_out;
   
   always @(posedge clk64)
     adc0 <= #1 {rx_a_a[11],rx_a_a[11:0],3'b0};

   wire [15:0] adc0_corr;
   rx_dcoffset #(0)rx_dcoffset0(.clock(clk64),.enable(1'b1),.reset(reset),.adc_in(adc0),.adc_out(adc0_corr),
	.serial_addr(7'd0),.serial_data(32'd0),.serial_strobe(1'b0));

   //wire [63:0] filt_debug = 64'd0;
   
   mrfm_proc mrfm_proc(.clock(clk64),.reset(rx_dsp_reset),.enable(enable_rx),
		       .serial_addr(serial_addr),.serial_data(serial_data),.serial_strobe(serial_strobe),
		       .signal_in(adc0_corr),.signal_out(dac0),.sync_out(sync_out),
		       .i(i),.q(q),.ip(ip),.qp(qp),.strobe_out(strobe_out),
		       .debugbus( /* filt_debug */ ));
          
   wire txsync = 1'b0;
   assign TXSYNC_A = txsync;
   assign TXSYNC_B = txsync;

   assign tx_a = dac0[15:2];

   //////////////////////////////////////////////////////////////////////////////////////////////////
   //  Data Collection on RX Buffer

   assign     rx_numchan[0] = 1'b0;
   setting_reg #(`FR_RX_MUX) sr_rxmux(.clock(clk64),.reset(rx_dsp_reset),.strobe(serial_strobe),.addr(serial_addr),
				      .in(serial_data),.out(rx_numchan[3:1]));
   
   rx_buffer rx_buffer
     ( .usbclk(usbclk),.bus_reset(rx_bus_reset),.reset(rx_dsp_reset),
       .usbdata(usbdata_out),.RD(RD),.have_pkt_rdy(have_pkt_rdy),.rx_overrun(rx_overrun),
       .channels(rx_numchan),
       .ch_0(i),.ch_1(q),
       .ch_2(ip),.ch_3(qp),
       .ch_4(16'd0),.ch_5(16'd0),
       .ch_6(16'd0),.ch_7(16'd0),
       .rxclk(clk64),.rxstrobe(strobe_out),
       .clear_status(clear_status),
       .serial_addr(serial_addr),.serial_data(serial_data),.serial_strobe(serial_strobe),
       .debugbus(rx_debugbus) );
   
   //////////////////////////////////////////////////////////////////////////////
   // Control Functions
       
   wire [31:0] capabilities = 32'd2;

   serial_io serial_io
     ( .master_clk(clk64),.serial_clock(SCLK),.serial_data_in(SDI),
       .enable(SEN_FPGA),.reset(1'b0),.serial_data_out(SDO),
       .serial_addr(serial_addr),.serial_data(serial_data),.serial_strobe(serial_strobe),
       .readback_0({io_rx_a,io_tx_a}),.readback_1({io_rx_b,io_tx_b}),.readback_2(capabilities),.readback_3(32'hf0f0931a) );

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
       .debug_0({15'd0,sync_out}), //filt_debug[63:48]),
       .debug_1({15'd0,sync_out}), //filt_debug[47:32]),
       .debug_2({15'd0,sync_out}), //filt_debug[31:16]),
       .debug_3({15'd0,sync_out}), //filt_debug[15:0]),
       .reg_0(reg_0),.reg_1(reg_1),.reg_2(reg_2),.reg_3(reg_3) );
   
   io_pins io_pins
     (.io_0(io_tx_a),.io_1(io_rx_a),.io_2(io_tx_b),.io_3(io_rx_b),
      .reg_0(reg_0),.reg_1(reg_1),.reg_2(reg_2),.reg_3(reg_3),
      .clock(clk64),.rx_reset(rx_dsp_reset),.tx_reset(tx_dsp_reset),
      .serial_addr(serial_addr),.serial_data(serial_data),.serial_strobe(serial_strobe));
   
endmodule // mrfm

