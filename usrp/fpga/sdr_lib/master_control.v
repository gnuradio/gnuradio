// -*- verilog -*-
//
//  USRP - Universal Software Radio Peripheral
//
//  Copyright (C) 2003,2005 Matt Ettus
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

// Clock, enable, and reset controls for whole system

module master_control
  ( input master_clk, input usbclk,
    input wire [6:0] serial_addr, input wire [31:0] serial_data, input wire serial_strobe,
    output tx_bus_reset, output rx_bus_reset,
    output wire tx_dsp_reset, output wire rx_dsp_reset,
    output wire enable_tx, output wire enable_rx,
    output wire [7:0] interp_rate, output wire [7:0] decim_rate,
    output tx_sample_strobe, output strobe_interp,
    output rx_sample_strobe, output strobe_decim,
    input tx_empty,
    input wire [15:0] debug_0,input wire [15:0] debug_1,input wire [15:0] debug_2,input wire [15:0] debug_3,
    output wire [15:0] reg_0, output wire [15:0] reg_1, output wire [15:0] reg_2, output wire [15:0] reg_3
    );
   
   // FIXME need a separate reset for all control settings 
   // Master Controls assignments
   wire [7:0] master_controls;
   setting_reg #(`FR_MASTER_CTRL) sr_mstr_ctrl(.clock(master_clk),.reset(1'b0),.strobe(serial_strobe),.addr(serial_addr),.in(serial_data),.out(master_controls));
   assign     enable_tx = master_controls[0];
   assign     enable_rx = master_controls[1];
   assign     tx_dsp_reset = master_controls[2];
   assign     rx_dsp_reset = master_controls[3];
   // Unused - 4-7

   // Strobe Generators
   setting_reg #(`FR_INTERP_RATE) sr_interp(.clock(master_clk),.reset(tx_dsp_reset),.strobe(serial_strobe),.addr(serial_addr),.in(serial_data),.out(interp_rate));
   setting_reg #(`FR_DECIM_RATE) sr_decim(.clock(master_clk),.reset(rx_dsp_reset),.strobe(serial_strobe),.addr(serial_addr),.in(serial_data),.out(decim_rate));
   
   strobe_gen da_strobe_gen
     ( .clock(master_clk),.reset(tx_dsp_reset),.enable(enable_tx),
       .rate(8'd1),.strobe_in(1'b1),.strobe(tx_sample_strobe) );

   strobe_gen tx_strobe_gen
     ( .clock(master_clk),.reset(tx_dsp_reset),.enable(enable_tx),
       .rate(interp_rate),.strobe_in(tx_sample_strobe),.strobe(strobe_interp) );

   assign  rx_sample_strobe = 1'b1;
   
   strobe_gen decim_strobe_gen
     ( .clock(master_clk),.reset(rx_dsp_reset),.enable(enable_rx),
       .rate(decim_rate),.strobe_in(rx_sample_strobe),.strobe(strobe_decim) );
   
   // Reset syncs for bus (usbclk) side
   // The RX bus side reset isn't used, the TX bus side one may not be needed
   reg 	  tx_reset_bus_sync1, rx_reset_bus_sync1, tx_reset_bus_sync2, rx_reset_bus_sync2;
 	   
   always @(posedge usbclk)
     begin
   	tx_reset_bus_sync1 <= #1 tx_dsp_reset;
   	rx_reset_bus_sync1 <= #1 rx_dsp_reset;
   	tx_reset_bus_sync2 <= #1 tx_reset_bus_sync1;
   	rx_reset_bus_sync2 <= #1 rx_reset_bus_sync1;
     end

   assign tx_bus_reset = tx_reset_bus_sync2;
   assign rx_bus_reset = rx_reset_bus_sync2;

   wire [7:0]   txa_refclk, rxa_refclk, txb_refclk, rxb_refclk;
   wire        txaclk,txbclk,rxaclk,rxbclk;
   wire [3:0]  debug_en, txcvr_ctrl;

   wire [31:0] txcvr_rxlines, txcvr_txlines;
      
   setting_reg #(`FR_TX_A_REFCLK) sr_txaref(.clock(master_clk),.reset(tx_dsp_reset),.strobe(serial_strobe),.addr(serial_addr),.in(serial_data),.out(txa_refclk));
   setting_reg #(`FR_RX_A_REFCLK) sr_rxaref(.clock(master_clk),.reset(rx_dsp_reset),.strobe(serial_strobe),.addr(serial_addr),.in(serial_data),.out(rxa_refclk));
   setting_reg #(`FR_TX_B_REFCLK) sr_txbref(.clock(master_clk),.reset(tx_dsp_reset),.strobe(serial_strobe),.addr(serial_addr),.in(serial_data),.out(txb_refclk));
   setting_reg #(`FR_RX_B_REFCLK) sr_rxbref(.clock(master_clk),.reset(rx_dsp_reset),.strobe(serial_strobe),.addr(serial_addr),.in(serial_data),.out(rxb_refclk));

   setting_reg #(`FR_DEBUG_EN) sr_debugen(.clock(master_clk),.reset(rx_dsp_reset|tx_dsp_reset),.strobe(serial_strobe),.addr(serial_addr),.in(serial_data),.out(debug_en));
         
   clk_divider clk_div_0 (.reset(tx_dsp_reset),.in_clk(master_clk),.out_clk(txaclk),.ratio(txa_refclk[6:0]));
   clk_divider clk_div_1 (.reset(rx_dsp_reset),.in_clk(master_clk),.out_clk(rxaclk),.ratio(rxa_refclk[6:0]));
   clk_divider clk_div_2 (.reset(tx_dsp_reset),.in_clk(master_clk),.out_clk(txbclk),.ratio(txb_refclk[6:0]));
   clk_divider clk_div_3 (.reset(rx_dsp_reset),.in_clk(master_clk),.out_clk(rxbclk),.ratio(rxb_refclk[6:0]));
   
   reg [15:0]  io_0_reg,io_1_reg,io_2_reg,io_3_reg;
   // Upper 16 bits are mask for lower 16
   always @(posedge master_clk)
     if(serial_strobe)
       case(serial_addr)
	 `FR_IO_0 : io_0_reg
	   <= #1 (io_0_reg & ~serial_data[31:16]) | (serial_data[15:0] & serial_data[31:16] );
	 `FR_IO_1 : io_1_reg
	   <= #1 (io_1_reg & ~serial_data[31:16]) | (serial_data[15:0] & serial_data[31:16] );
	 `FR_IO_2 : io_2_reg
	   <= #1 (io_2_reg & ~serial_data[31:16]) | (serial_data[15:0] & serial_data[31:16] );
	 `FR_IO_3 : io_3_reg
	   <= #1 (io_3_reg & ~serial_data[31:16]) | (serial_data[15:0] & serial_data[31:16] );
       endcase // case(serial_addr)

   wire        transmit_now;
   wire        atr_ctl;
   wire [11:0] atr_tx_delay, atr_rx_delay;
   wire [15:0] atr_mask_0, atr_txval_0, atr_rxval_0, atr_mask_1, atr_txval_1, atr_rxval_1, atr_mask_2, atr_txval_2, atr_rxval_2, atr_mask_3, atr_txval_3, atr_rxval_3;
      
   setting_reg #(`FR_ATR_MASK_0) sr_atr_mask_0(.clock(master_clk),.reset(1'b0),.strobe(serial_strobe),.addr(serial_addr),.in(serial_data),.out(atr_mask_0));
   setting_reg #(`FR_ATR_TXVAL_0) sr_atr_txval_0(.clock(master_clk),.reset(1'b0),.strobe(serial_strobe),.addr(serial_addr),.in(serial_data),.out(atr_txval_0));
   setting_reg #(`FR_ATR_RXVAL_0) sr_atr_rxval_0(.clock(master_clk),.reset(1'b0),.strobe(serial_strobe),.addr(serial_addr),.in(serial_data),.out(atr_rxval_0));

   setting_reg #(`FR_ATR_MASK_1) sr_atr_mask_1(.clock(master_clk),.reset(1'b0),.strobe(serial_strobe),.addr(serial_addr),.in(serial_data),.out(atr_mask_1));
   setting_reg #(`FR_ATR_TXVAL_1) sr_atr_txval_1(.clock(master_clk),.reset(1'b0),.strobe(serial_strobe),.addr(serial_addr),.in(serial_data),.out(atr_txval_1));
   setting_reg #(`FR_ATR_RXVAL_1) sr_atr_rxval_1(.clock(master_clk),.reset(1'b0),.strobe(serial_strobe),.addr(serial_addr),.in(serial_data),.out(atr_rxval_1));

   setting_reg #(`FR_ATR_MASK_2) sr_atr_mask_2(.clock(master_clk),.reset(1'b0),.strobe(serial_strobe),.addr(serial_addr),.in(serial_data),.out(atr_mask_2));
   setting_reg #(`FR_ATR_TXVAL_2) sr_atr_txval_2(.clock(master_clk),.reset(1'b0),.strobe(serial_strobe),.addr(serial_addr),.in(serial_data),.out(atr_txval_2));
   setting_reg #(`FR_ATR_RXVAL_2) sr_atr_rxval_2(.clock(master_clk),.reset(1'b0),.strobe(serial_strobe),.addr(serial_addr),.in(serial_data),.out(atr_rxval_2));

   setting_reg #(`FR_ATR_MASK_3) sr_atr_mask_3(.clock(master_clk),.reset(1'b0),.strobe(serial_strobe),.addr(serial_addr),.in(serial_data),.out(atr_mask_3));
   setting_reg #(`FR_ATR_TXVAL_3) sr_atr_txval_3(.clock(master_clk),.reset(1'b0),.strobe(serial_strobe),.addr(serial_addr),.in(serial_data),.out(atr_txval_3));
   setting_reg #(`FR_ATR_RXVAL_3) sr_atr_rxval_3(.clock(master_clk),.reset(1'b0),.strobe(serial_strobe),.addr(serial_addr),.in(serial_data),.out(atr_rxval_3));

   //setting_reg #(`FR_ATR_CTL) sr_atr_ctl(.clock(master_clk),.reset(1'b0),.strobe(serial_strobe),.addr(serial_addr),.in(serial_data),.out(atr_ctl));
   setting_reg #(`FR_ATR_TX_DELAY) sr_atr_tx_delay(.clock(master_clk),.reset(1'b0),.strobe(serial_strobe),.addr(serial_addr),.in(serial_data),.out(atr_tx_delay));
   setting_reg #(`FR_ATR_RX_DELAY) sr_atr_rx_delay(.clock(master_clk),.reset(1'b0),.strobe(serial_strobe),.addr(serial_addr),.in(serial_data),.out(atr_rx_delay));

   assign      atr_ctl = 1'b1;

   atr_delay atr_delay(.clk_i(master_clk),.rst_i(tx_dsp_reset),.ena_i(atr_ctl),.tx_empty_i(tx_empty),
		       .tx_delay_i(atr_tx_delay),.rx_delay_i(atr_rx_delay),.atr_tx_o(transmit_now));
   
   wire [15:0] atr_selected_0 = transmit_now ? atr_txval_0 : atr_rxval_0;
   wire [15:0] io_0 = ({{16{atr_ctl}}} &  atr_mask_0 & atr_selected_0) | (~({{16{atr_ctl}}} & atr_mask_0) & io_0_reg);
   
   wire [15:0] atr_selected_1 = transmit_now ? atr_txval_1 : atr_rxval_1;
   wire [15:0] io_1 = ({{16{atr_ctl}}} &  atr_mask_1 & atr_selected_1) | (~({{16{atr_ctl}}} & atr_mask_1) & io_1_reg);
   
   wire [15:0] atr_selected_2 = transmit_now ? atr_txval_2 : atr_rxval_2;
   wire [15:0] io_2 = ({{16{atr_ctl}}} &  atr_mask_2 & atr_selected_2) | (~({{16{atr_ctl}}} & atr_mask_2) & io_2_reg);
   
   wire [15:0] atr_selected_3 = transmit_now ? atr_txval_3 : atr_rxval_3;
   wire [15:0] io_3 = ({{16{atr_ctl}}} &  atr_mask_3 & atr_selected_3) | (~({{16{atr_ctl}}} & atr_mask_3) & io_3_reg);
   
   assign reg_0 = debug_en[0] ? debug_0 : txa_refclk[7] ? {io_0[15:1],txaclk} : io_0;
   assign reg_1 = debug_en[1] ? debug_1 : rxa_refclk[7] ? {io_1[15:1],rxaclk} : io_1;
   assign reg_2 = debug_en[2] ? debug_2 : txb_refclk[7] ? {io_2[15:1],txbclk} : io_2;
   assign reg_3 = debug_en[3] ? debug_3 : rxb_refclk[7] ? {io_3[15:1],rxbclk} : io_3;

   
endmodule // master_control
