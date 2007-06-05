// -*- verilog -*-
//
//  USRP - Universal Software Radio Peripheral
//
//  Copyright (C) 2003,2004 Matt Ettus
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

// Top level module for a full setup with DUCs and DDCs

// Define DEBUG_OWNS_IO_PINS if we're using the daughterboard i/o pins
// for debugging info.  NB, This can kill the m'board and/or d'board if you
// have anything except basic d'boards installed.

// Uncomment the following to include optional circuitry

module usrp_sounder
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
   
   // wire WR = usbctl[0];
   wire RD = usbctl[1];
   wire OE = usbctl[2];

   wire have_pkt_rdy;
   assign usbrdy[0] = 1'b0; // have_space;
   assign usbrdy[1] = have_pkt_rdy;

   wire   tx_underrun, rx_overrun;    
   wire   clear_status = FX2_1;
   assign FX2_2 = rx_overrun;
   assign FX2_3 = 1'b0; // tx_underrun;
      
   wire [15:0] usbdata_out;
   
   wire [3:0]  rx_numchan;
   wire        enable_tx, enable_rx;
   wire        tx_dsp_reset, rx_dsp_reset, tx_bus_reset, rx_bus_reset;
   
   // Tri-state bus macro
   bustri bustri( .data(usbdata_out),.enabledt(OE),.tridata(usbdata) );

   assign      clk64 = master_clk;

   // TX
   wire        tx_sample_strobe;
   wire        tx_empty;
   
   wire        serial_strobe;
   wire [6:0]  serial_addr;
   wire [31:0] serial_data;

   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // Transmit Side
   
   wire [13:0] tx_i, tx_q;
   wire [13:0] tx_dac;
   
   dac_interface dac(.clk_i(clk64),.rst_i(tx_dsp_reset),.ena_i(enable_tx),
		     .strobe_i(tx_sample_strobe),.tx_i_i(tx_i),.tx_q_i(tx_q),
		     .tx_data_o(tx_dac),.tx_sync_o(TXSYNC_A));

   assign tx_a = tx_dac;

   // Wedge DAC #2 at zero
   assign TXSYNC_B = 1'b0;
   assign tx_b = 14'b0;
   
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // Receive Side
   wire        rx_sample_strobe, rx_strobe;
   wire [15:0] rx_adc0_i, rx_adc0_q;
   wire [15:0] rx_buf_i, rx_buf_q;
   
   adc_interface adc_interface(.clock(clk64),.reset(rx_dsp_reset),.enable(enable_rx),
			       .serial_addr(serial_addr),.serial_data(serial_data),.serial_strobe(serial_strobe),
			       .rx_a_a(rx_a_a),.rx_b_a(rx_b_a),.rx_a_b(),.rx_b_b(),
			       .rssi_0(),.rssi_1(),.rssi_2(),.rssi_3(),
			       .ddc0_in_i(rx_adc0_i),.ddc0_in_q(rx_adc0_q),
			       .ddc1_in_i(),.ddc1_in_q(),
			       .ddc2_in_i(),.ddc2_in_q(),
			       .ddc3_in_i(),.ddc3_in_q(),.rx_numchan(rx_numchan) );

   rx_buffer rx_buffer
     ( .usbclk(usbclk),.bus_reset(rx_bus_reset),.reset(rx_dsp_reset),
       .reset_regs(rx_dsp_reset),
       .usbdata(usbdata_out),.RD(RD),.have_pkt_rdy(have_pkt_rdy),.rx_overrun(rx_overrun),
       .channels(rx_numchan),
       .ch_0(rx_buf_i),.ch_1(rx_buf_q),
       .ch_2(),.ch_3(),
       .ch_4(),.ch_5(),
       .ch_6(),.ch_7(),
       .rxclk(clk64),.rxstrobe(rx_strobe),
       .clear_status(clear_status),
       .serial_addr(serial_addr),.serial_data(serial_data),.serial_strobe(serial_strobe),
       .debugbus() );
   

   ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // Top level application

   sounder sounder
     ( .clk_i(clk64),.saddr_i(serial_addr),.sdata_i(serial_data),.s_strobe_i(serial_strobe),
       .tx_strobe_o(tx_sample_strobe),.tx_dac_i_o(tx_i),.tx_dac_q_o(tx_q),
       .rx_adc_i_i(rx_adc0_i),.rx_adc_q_i(rx_adc0_q),
       .rx_strobe_o(rx_strobe),.rx_imp_i_o(rx_buf_i),.rx_imp_q_o(rx_buf_q)
       );

   
   ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // Control Functions

   wire [31:0] capabilities;
   assign capabilities[7]   = 0;  // `TX_CAP_HB;
   assign capabilities[6:4] = 2;  // `TX_CAP_NCHAN;
   assign capabilities[3]   = 0;  // `RX_CAP_HB;
   assign capabilities[2:0] = 2;  // `RX_CAP_NCHAN;

   serial_io serial_io
     ( .master_clk(clk64),.serial_clock(SCLK),.serial_data_in(SDI),
       .enable(SEN_FPGA),.reset(1'b0),.serial_data_out(SDO),
       .serial_addr(serial_addr),.serial_data(serial_data),.serial_strobe(serial_strobe),
       .readback_0({io_rx_a,io_tx_a}),.readback_1({io_rx_b,io_tx_b}),.readback_2(capabilities),.readback_3(32'hf0f0931a),
       .readback_4(),.readback_5(),.readback_6(),.readback_7()
       );

   wire [15:0] reg_0,reg_1,reg_2,reg_3;
   master_control master_control
     ( .master_clk(clk64),.usbclk(usbclk),
       .serial_addr(serial_addr),.serial_data(serial_data),.serial_strobe(serial_strobe),
       .tx_bus_reset(tx_bus_reset),.rx_bus_reset(rx_bus_reset),
       .tx_dsp_reset(tx_dsp_reset),.rx_dsp_reset(rx_dsp_reset),
       .enable_tx(enable_tx),.enable_rx(enable_rx),
       .interp_rate(),.decim_rate(),
       .tx_sample_strobe(),.strobe_interp(),
       .rx_sample_strobe(rx_sample_strobe),.strobe_decim(),
       .tx_empty(tx_empty),
       .debug_0(),.debug_1(),
       .debug_2(),.debug_3(),
       .reg_0(reg_0),.reg_1(reg_1),.reg_2(reg_2),.reg_3(reg_3) );
   
   io_pins io_pins
     (.io_0(io_tx_a),.io_1(io_rx_a),.io_2(io_tx_b),.io_3(io_rx_b),
      .reg_0(reg_0),.reg_1(reg_1),.reg_2(reg_2),.reg_3(reg_3),
      .clock(clk64),.rx_reset(rx_dsp_reset),.tx_reset(tx_dsp_reset),
      .serial_addr(serial_addr),.serial_data(serial_data),.serial_strobe(serial_strobe));
   
endmodule // usrp_sounder
