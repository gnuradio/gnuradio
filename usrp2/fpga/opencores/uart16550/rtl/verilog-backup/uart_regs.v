//////////////////////////////////////////////////////////////////////
////                                                              ////
////  uart_regs.v                                                 ////
////                                                              ////
////                                                              ////
////  This file is part of the "UART 16550 compatible" project    ////
////  http://www.opencores.org/cores/uart16550/                   ////
////                                                              ////
////  Documentation related to this project:                      ////
////  - http://www.opencores.org/cores/uart16550/                 ////
////                                                              ////
////  Projects compatibility:                                     ////
////  - WISHBONE                                                  ////
////  RS232 Protocol                                              ////
////  16550D uart (mostly supported)                              ////
////                                                              ////
////  Overview (main Features):                                   ////
////  Registers of the uart 16550 core                            ////
////                                                              ////
////  Known problems (limits):                                    ////
////  Inserts 1 wait state in all WISHBONE transfers              ////
////                                                              ////
////  To Do:                                                      ////
////  Nothing or verification.                                    ////
////                                                              ////
////  Author(s):                                                  ////
////      - gorban@opencores.org                                  ////
////      - Jacob Gorban                                          ////
////                                                              ////
////  Created:        2001/05/12                                  ////
////  Last Updated:   (See log for the revision history           ////
////                                                              ////
////                                                              ////
//////////////////////////////////////////////////////////////////////
////                                                              ////
//// Copyright (C) 2000 Jacob Gorban, gorban@opencores.org        ////
////                                                              ////
//// This source file may be used and distributed without         ////
//// restriction provided that this copyright statement is not    ////
//// removed from the file and that any derivative work contains  ////
//// the original copyright notice and the associated disclaimer. ////
////                                                              ////
//// This source file is free software; you can redistribute it   ////
//// and/or modify it under the terms of the GNU Lesser General   ////
//// Public License as published by the Free Software Foundation; ////
//// either version 2.1 of the License, or (at your option) any   ////
//// later version.                                               ////
////                                                              ////
//// This source is distributed in the hope that it will be       ////
//// useful, but WITHOUT ANY WARRANTY; without even the implied   ////
//// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR      ////
//// PURPOSE.  See the GNU Lesser General Public License for more ////
//// details.                                                     ////
////                                                              ////
//// You should have received a copy of the GNU Lesser General    ////
//// Public License along with this source; if not, download it   ////
//// from http://www.opencores.org/lgpl.shtml                     ////
////                                                              ////
//////////////////////////////////////////////////////////////////////
//
// CVS Revision History
//
// $Log: uart_regs.v,v $
// Revision 1.10  2001/06/23 11:21:48  gorban
// DL made 16-bit long. Fixed transmission/reception bugs.
//
// Revision 1.9  2001/05/31 20:08:01  gorban
// FIFO changes and other corrections.
//
// Revision 1.8  2001/05/29 20:05:04  gorban
// Fixed some bugs and synthesis problems.
//
// Revision 1.7  2001/05/27 17:37:49  gorban
// Fixed many bugs. Updated spec. Changed FIFO files structure. See CHANGES.txt file.
//
// Revision 1.6  2001/05/21 19:12:02  gorban
// Corrected some Linter messages.
//
// Revision 1.5  2001/05/17 18:34:18  gorban
// First 'stable' release. Should be sythesizable now. Also added new header.
//
// Revision 1.0  2001-05-17 21:27:11+02  jacob
// Initial revision
//
//

`include "timescale.v"
`include "uart_defines.v"

`define UART_DL1 7:0
`define UART_DL2 15:8

module uart_regs (clk,
	wb_rst_i, wb_addr_i, wb_dat_i, wb_dat_o, wb_we_i, wb_re_i, 

// additional signals
	modem_inputs,
	stx_pad_o, srx_pad_i,
	enable,
	rts_pad_o, dtr_pad_o, int_o
	);

input		clk;
input		wb_rst_i;
input	[`UART_ADDR_WIDTH-1:0]	wb_addr_i;
input	[7:0]	wb_dat_i;
output	[7:0]	wb_dat_o;
input		wb_we_i;
input   wb_re_i;

output		stx_pad_o;
input		srx_pad_i;

input	[3:0]	modem_inputs;
output		enable;
output		rts_pad_o;
output		dtr_pad_o;
output		int_o;

wire	[3:0]	modem_inputs;
reg		enable;
wire		stx_pad_o;		// received from transmitter module
wire		srx_pad_i;

reg	[7:0]	wb_dat_o;

wire	[`UART_ADDR_WIDTH-1:0]	wb_addr_i;
wire	[7:0]	wb_dat_i;


reg	[3:0]	ier;
reg	[3:0]	iir;
reg	[1:0]	fcr;  /// bits 7 and 6 of fcr. Other bits are ignored
reg	[4:0]	mcr;
reg	[7:0]	lcr;
reg	[7:0]	lsr;
reg	[7:0]	msr;
reg	[15:0]	dl;  // 32-bit divisor latch
reg		start_dlc; // activate dlc on writing to UART_DL1
reg		lsr_mask;
reg		msi_reset; // reset MSR 4 lower bits indicator
reg		threi_clear; // THRE interrupt clear flag
reg	[15:0]	dlc;  // 32-bit divisor latch counter
reg		int_o;

reg	[3:0]	trigger_level; // trigger level of the receiver FIFO
reg		rx_reset;
reg		tx_reset;

wire		dlab;			   // divisor latch access bit
wire		cts_pad_i, dsr_pad_i, ri_pad_i, dcd_pad_i; // modem status bits
wire		loopback;		   // loopback bit (MCR bit 4)
wire		cts, dsr, ri, dcd;	   // effective signals (considering loopback)
wire		rts_pad_o, dtr_pad_o;		   // modem control outputs

//
// ASSINGS
//
assign {cts_pad_i, dsr_pad_i, ri_pad_i, dcd_pad_i} = modem_inputs;
assign {cts, dsr, ri, dcd} = loopback ? {mcr[`UART_MC_RTS],mcr[`UART_MC_DTR],mcr[`UART_MC_OUT1],mcr[`UART_MC_OUT2]}
		 : ~{cts_pad_i,dsr_pad_i,ri_pad_i,dcd_pad_i};

assign dlab = lcr[`UART_LC_DL];
assign loopback = mcr[4];

// assign modem outputs
assign	rts_pad_o = mcr[`UART_MC_RTS];
assign	dtr_pad_o = mcr[`UART_MC_DTR];

// Interrupt signals
reg	rls_int;  // receiver line status interrupt
reg	rda_int;  // receiver data available interrupt
reg	ti_int;   // timeout indicator interrupt
reg	thre_int; // transmitter holding register empty interrupt
reg	ms_int;   // modem status interrupt

// FIFO signals
reg				tf_push;
reg				rf_pop;
wire	[`UART_FIFO_REC_WIDTH-1:0]	rf_data_out;
wire				rf_error_bit; // an error (parity or framing) is inside the fifo
wire	[`UART_FIFO_COUNTER_W-1:0]	rf_count;
wire	[`UART_FIFO_COUNTER_W-1:0]	tf_count;
wire	[2:0]			state;
wire	[5:0]			counter_t;
wire	[3:0]			counter_b;
wire            rx_lsr_mask;

// Transmitter Instance
uart_transmitter transmitter(clk, wb_rst_i, lcr, tf_push, wb_dat_i, enable, stx_pad_o, state, tf_count, tx_reset);

// Receiver Instance
uart_receiver receiver(clk, wb_rst_i, lcr, rf_pop, srx_pad_i, enable, rda_int,
	counter_t, counter_b, rf_count, rf_data_out, rf_error_bit, rf_overrun, rx_reset, rx_lsr_mask);

/*
always @(posedge clk or posedge wb_rst_i)   // synchrounous reading
begin
    if (wb_rst_i)
    begin
	wb_dat_o <= #1 8'b0;
    end
    else
    if (wb_re_i)   //if (we're not writing)
	case (wb_addr_i)
	`UART_REG_RB : if (dlab) // Receiver FIFO or DL byte 1
			wb_dat_o <= #1 dl[`UART_DL1];
		  else
			wb_dat_o <= #1 rf_data_out[9:2];
	`UART_REG_IE	: wb_dat_o <= #1 dlab ? dl[`UART_DL2] : ier;
	`UART_REG_II	: wb_dat_o <= #1 {4'b1100,iir};
	`UART_REG_LC	: wb_dat_o <= #1 lcr;
	`UART_REG_LS	: wb_dat_o <= #1 lsr;
	`UART_REG_MS	: wb_dat_o <= #1 msr;
	default:  wb_dat_o <= #1 8'b0; // ??
	endcase
    else
	wb_dat_o <= #1 8'b0;
end
*/

always @(wb_addr_i or dlab or dl or rf_data_out or ier or iir or lcr or lsr or msr)
begin
	case (wb_addr_i)
	`UART_REG_RB : if (dlab) // Receiver FIFO or DL byte 1
        			wb_dat_o <= dl[`UART_DL1];
		        else
			        wb_dat_o <= rf_data_out[9:2];
	`UART_REG_IE	: wb_dat_o <= dlab ? dl[`UART_DL2] : ier;
	`UART_REG_II	: wb_dat_o <= {4'b1100,iir};
	`UART_REG_LC	: wb_dat_o <= lcr;
	`UART_REG_LS	: wb_dat_o <= lsr;
	`UART_REG_MS	: wb_dat_o <= msr;
	default:  wb_dat_o <= 8'b0; // ??
	endcase
end

// rf_pop signal handling
always @(posedge clk or posedge wb_rst_i)
begin
	if (wb_rst_i)
		rf_pop <= #1 0; 
	else
	if (rf_pop)	// restore the signal to 0 after one clock cycle
		rf_pop <= #1 0;
	else
	if (wb_re_i && wb_addr_i == `UART_REG_RB && !dlab)
		rf_pop <= #1 1; // advance read pointer
end

// lsr_mask signal handling
always @(posedge clk or posedge wb_rst_i)
begin
	if (wb_rst_i)
		lsr_mask <= #1 0;
	else
	if (lsr_mask)
		lsr_mask <= #1 0;
	else
	if (wb_re_i && wb_addr_i == `UART_REG_LS && !dlab)
		lsr_mask <= #1 1; // reset bits in the Line Status Register
end

assign rx_lsr_mask = lsr_mask;

// msi_reset signal handling
always @(posedge clk or posedge wb_rst_i)
begin
	if (wb_rst_i)
		msi_reset <= #1 0;
	else
	if (msi_reset)
		msi_reset <= #1 0;
	else
	if (wb_re_i && wb_addr_i == `UART_REG_MS)
		msi_reset <= #1 1; // reset bits in Modem Status Register
end

// threi_clear signal handling
always @(posedge clk or posedge wb_rst_i)
begin
	if (wb_rst_i)
		threi_clear <= #1 0;
	else
	if (threi_clear && !lsr[`UART_LS_TFE] && (tf_count==0)) // reset clear flag when tx fifo clears
		threi_clear <= #1 0;
	else
	if (wb_re_i && wb_addr_i == `UART_REG_II)
		threi_clear <= #1 1; // reset bits in Modem Status Register
end

//
//   WRITES AND RESETS   //
//
// Line Control Register
always @(posedge clk or posedge wb_rst_i)
	if (wb_rst_i)
		lcr <= #1 8'b00000011; // 8n1 setting
	else
	if (wb_we_i && wb_addr_i==`UART_REG_LC)
		lcr <= #1 wb_dat_i;

// Interrupt Enable Register or UART_DL2
always @(posedge clk or posedge wb_rst_i)
	if (wb_rst_i)
	begin
		ier <= #1 4'b0000; // no interrupts after reset
		dl[`UART_DL2] <= #1 8'b0;
	end
	else
	if (wb_we_i && wb_addr_i==`UART_REG_IE)
		if (dlab)
		begin
			dl[`UART_DL2] <= #1 wb_dat_i;
		end
		else
			ier <= #1 wb_dat_i[3:0]; // ier uses only 4 lsb


// FIFO Control Register and rx_reset, tx_reset signals
always @(posedge clk or posedge wb_rst_i)
	if (wb_rst_i) begin
		fcr <= #1 2'b11; 
		rx_reset <= #1 0;
		tx_reset <= #1 0;
	end else
	if (wb_we_i && wb_addr_i==`UART_REG_FC) begin
		fcr <= #1 wb_dat_i[7:6];
		rx_reset <= #1 wb_dat_i[1];
		tx_reset <= #1 wb_dat_i[2];
	end else begin // clear rx_reset, tx_reset signals when not written to
		rx_reset <= #1 0;
		tx_reset <= #1 0;
	end

// Modem Control Register
always @(posedge clk or posedge wb_rst_i)
	if (wb_rst_i)
		mcr <= #1 5'b0; 
	else
	if (wb_we_i && wb_addr_i==`UART_REG_MC)
			mcr <= #1 wb_dat_i[4:0];

// TX_FIFO or UART_DL1
always @(posedge clk or posedge wb_rst_i)
	if (wb_rst_i)
	begin
		dl[`UART_DL1]  <= #1 8'b0;
		tf_push   <= #1 1'b0;
		start_dlc <= #1 1'b0;
	end
	else
	if (wb_we_i && wb_addr_i==`UART_REG_TR)
		if (dlab)
		begin
			dl[`UART_DL1] <= #1 wb_dat_i;
			start_dlc <= #1 1'b1; // enable DL counter
			tf_push <= #1 1'b0;
		end
		else
		begin
			tf_push   <= #1 1'b1;
			start_dlc <= #1 1'b0;
		end
	else
	begin
		start_dlc <= #1 1'b0;
		tf_push   <= #1 1'b0;
	end

// Receiver FIFO trigger level selection logic (asynchronous mux)
always @(fcr[`UART_FC_TL])
	case (fcr[`UART_FC_TL])
		2'b00 : trigger_level = 1;
		2'b01 : trigger_level = 4;
		2'b10 : trigger_level = 8;
		2'b11 : trigger_level = 14;
	endcase
	
//
//  STATUS REGISTERS  //
//

// Modem Status Register
always @(posedge clk or posedge wb_rst_i)
begin
	if (wb_rst_i)
		msr <= #1 0;
	else begin
		msr[`UART_MS_DDCD:`UART_MS_DCTS] <= #1 msi_reset ? 4'b0 :
			msr[`UART_MS_DDCD:`UART_MS_DCTS] | ({dcd, ri, dsr, cts} ^ msr[`UART_MS_CDCD:`UART_MS_CCTS]);
		msr[`UART_MS_CDCD:`UART_MS_CCTS] <= #1 {dcd, ri, dsr, cts};
	end
end

// Line Status Register
always @(posedge clk or posedge wb_rst_i)
begin
	if (wb_rst_i)
		lsr <= #1 8'b01100000;
	else
	if (lsr_mask)
		lsr <= #1 lsr & 8'b00000001;
	else
	begin
		lsr[0] <= #1 (rf_count!=4'b0);  // data in receiver fifo available
		lsr[1] <= #1 rf_overrun;     // Receiver overrun error
		lsr[2] <= #1 rf_data_out[1]; // parity error bit
		lsr[3] <= #1 rf_data_out[0]; // framing error bit
		lsr[4] <= #1 (counter_b==4'b0); // break counter reached 0
		lsr[5] <= #1 (tf_count==5'b0);  // transmitter fifo is empty
		lsr[6] <= #1 (tf_count==5'b0 && (state == /*`S_IDLE */ 0)); // transmitter empty
		lsr[7] <= #1 rf_error_bit;
	end
end

// Enable signal generation logic
always @(posedge clk or posedge wb_rst_i)
begin
	if (wb_rst_i)
	begin
		dlc    <= #1 0;
		enable <= #1 1'b0;
	end
	else
	begin
		if (start_dlc)
		begin
			enable <= #1 1'b0;
			dlc    <= #1 dl;
		end
		else
		begin
			if (dl!=0)
			begin
				if ( (dlc-1)==0 )
				begin
					enable <= #1 1'b1;
					dlc <= #1 dl;
				end
				else
				begin
					enable <= #1 1'b0;
					dlc <= #1 dlc - 1;
				end
			end
			else
			begin
				dlc <= #1 0;
				enable <= #1 1'b0;
			end
		end
	end
end

//
//	INTERRUPT LOGIC
//
always @(posedge clk or posedge wb_rst_i)
begin
	if (wb_rst_i)
	begin
		rls_int  <= #1 1'b0;
		rda_int  <= #1 1'b0;
		ti_int   <= #1 1'b0;
		thre_int <= #1 1'b0;
		ms_int   <= #1 1'b0;
	end
	else
	begin
		rls_int  <= #1 ier[`UART_IE_RLS] && (lsr[`UART_LS_OE] || lsr[`UART_LS_PE] || lsr[`UART_LS_FE] || lsr[`UART_LS_BI]);
		rda_int  <= #1 ier[`UART_IE_RDA] && (rf_count >= {1'b0,trigger_level});
		thre_int <= #1 threi_clear ? 0 : ier[`UART_IE_THRE] && lsr[`UART_LS_TFE];
		ms_int   <= #1 ier[`UART_IE_MS] && (| msr[3:0]);
		ti_int   <= #1 ier[`UART_IE_RDA] && (counter_t == 6'b0);
	end
end

always @(posedge clk or posedge wb_rst_i)
begin
	if (wb_rst_i)	
		int_o <= #1 1'b0;
	else
		if (| {rls_int,rda_int,thre_int,ms_int,ti_int})
			int_o <= #1 1'b1;
		else
			int_o <= #1 1'b0;
end


// Interrupt Identification register
always @(posedge clk or posedge wb_rst_i)
begin
	if (wb_rst_i)
		iir <= #1 1;
	else
	if (rls_int)  // interrupt occured and is enabled  (not masked)
	begin
		iir[`UART_II_II] <= #1 `UART_II_RLS;	// set identification register to correct value
		iir[`UART_II_IP] <= #1 1'b0;		// and clear the IIR bit 0 (interrupt pending)
	end
	else
	if (rda_int)
	begin
		iir[`UART_II_II] <= #1 `UART_II_RDA;
		iir[`UART_II_IP] <= #1 1'b0;
	end
	else
	if (ti_int)
	begin
		iir[`UART_II_II] <= #1 `UART_II_TI;
		iir[`UART_II_IP] <= #1 1'b0;
	end
	else
	if (thre_int)
	begin
		iir[`UART_II_II] <= #1 `UART_II_THRE;
		iir[`UART_II_IP] <= #1 1'b0;
	end
	else
	if (ms_int)
	begin
		iir[`UART_II_II] <= #1 `UART_II_MS;
		iir[`UART_II_IP] <= #1 1'b0;
	end
	else	// no interrupt is pending
	begin
		iir[`UART_II_IP] <= #1 1'b1;
	end
end

endmodule
