/*
 * Multi-port ZBT SRAM WISHBONE controller
 * Copyright (C) 2008 Sebastien Bourdeauducq - http://lekernel.net
 * This file is part of Milkymist.
 *
 * Milkymist is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Library General Public License as published
 * by the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
 * USA.
 */

module wb_zbt(
	input               clk,
	input               rst,
	// Wishbone bus A, highest priority, with prefetch
	input      [31:0]   wbA_adr_i,
	input      [31:0]   wbA_dat_i,
	output     [31:0]   wbA_dat_o,
	input      [ 3:0]   wbA_sel_i,
	input               wbA_cyc_i,
	input               wbA_stb_i,
	output              wbA_ack_o,
	input               wbA_we_i,
	// Wishbone bus B, lower priority
	input      [31:0]   wbB_adr_i,
	input      [31:0]   wbB_dat_i,
	output     [31:0]   wbB_dat_o,
	input      [ 3:0]   wbB_sel_i,
	input               wbB_cyc_i,
	input               wbB_stb_i,
	output              wbB_ack_o,
	input               wbB_we_i,
	// Memory connection
	output              sram_clk,
	output     [17:0]   sram_a,
	inout      [31:0]   sram_d,
	output              sram_we,
	output     [ 3:0]   sram_bw,
	output              sram_adv,
	output              sram_ce,
	output              sram_oe,
	output              sram_mode,
	output              sram_zz
);

assign sram_clk = clk;
assign sram_oe = 1'b0;
assign sram_ce = 1'b0;
assign sram_adv = 1'b0;
assign sram_mode = 1'b0;
assign sram_zz = 1'b0;

/* Wishbone decoding */

wire busA_active;
wire busB_active;

assign busA_active = wbA_cyc_i & wbA_stb_i;
assign busB_active = wbB_cyc_i & wbB_stb_i;

/* Those are used to represent the state of the SRAM pipeline
 * Bit 0 = Write Enable
 * Bits 18..1 = Address
 */
wire [18:0] pipeline_in;
reg [18:0] pipeline_internal;
reg [18:0] pipeline_out;

always @(posedge clk or posedge rst) begin
	if(rst) begin
		pipeline_internal <= 0;
		pipeline_out <= 0;
	end else begin
		pipeline_internal <= pipeline_in;
		pipeline_out <= pipeline_internal;
	end
end

/* Pipeline contents decode */

wire inprogressA;
wire inprogressB;

assign inprogressA = (pipeline_internal[18:1] == wbA_adr_i[19:2]) & (pipeline_internal[0] == wbA_we_i);
assign inprogressB = (pipeline_internal[18:1] == wbB_adr_i[19:2]) & (pipeline_internal[0] == wbB_we_i);

wire hitA;
wire hitB;

assign hitA = (pipeline_out[18:1] == wbA_adr_i[19:2]) & (pipeline_out[0] == wbA_we_i);
assign hitB = (pipeline_out[18:1] == wbB_adr_i[19:2]) & (pipeline_out[0] == wbB_we_i);

/* Access arbitration */

wire [1:0] bus_request;

assign bus_request[0] = busA_active & ~inprogressA & ~hitA;
assign bus_request[1] = busB_active & ~inprogressB & ~hitB;

wire prefetch_enable;
reg [17:0] prefetch_address;

assign prefetch_enable = ~bus_request[0] & ~bus_request[1];
always @(posedge clk) begin
	if(prefetch_enable)
		prefetch_address <= wbA_adr_i[19:2] + 2;
	else
		prefetch_address <= wbA_adr_i[19:2] + 1;
end

wire [1:0] bus_selected;

assign bus_selected[0] = bus_request[0];
assign bus_selected[1] = bus_request[1] & ~bus_request[0];

assign pipeline_in[18:1] = ({18{bus_selected[0]}} & wbA_adr_i[19:2])
	| ({18{bus_selected[1]}} & wbB_adr_i[19:2])
	| ({18{prefetch_enable}} & prefetch_address);
assign pipeline_in[0] = (bus_selected[0] & wbA_we_i)|(bus_selected[1] & wbB_we_i);

/* SRAM control */

assign sram_a = pipeline_in[18:1];
assign sram_bw = ~(({4{bus_selected[0]}} & wbA_sel_i)|({4{bus_selected[1]}} & wbB_sel_i));
assign sram_we = ~pipeline_in[0];

/* SRAM data */

wire [31:0] bus_wdata;

assign wbA_ack_o = busA_active & hitA;
assign wbB_ack_o = busB_active & hitB;

assign bus_wdata = ({32{hitA}} & wbA_dat_i)|({32{hitB}} & wbB_dat_i);
assign sram_d = pipeline_out[0] ? bus_wdata : 32'hzzzzzzzz;
assign wbA_dat_o = sram_d;
assign wbB_dat_o = sram_d;

endmodule
