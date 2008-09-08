// $Header: /devl/xcs/repo/env/Databases/CAEInterfaces/verunilibs/data/unisims/RAMB16_S1_S2.v,v 1.10 2005/03/14 22:54:41 wloo Exp $
///////////////////////////////////////////////////////////////////////////////
// Copyright (c) 1995/2005 Xilinx, Inc.
// All Right Reserved.
///////////////////////////////////////////////////////////////////////////////
//   ____  ____
//  /   /\/   /
// /___/  \  /    Vendor : Xilinx
// \   \   \/     Version : 8.1i (I.13)
//  \   \         Description : Xilinx Functional Simulation Library Component
//  /   /                  16K-Bit Data and 2K-Bit Parity Dual Port Block RAM
// /___/   /\     Filename : RAMB16_S1_S2.v
// \   \  /  \    Timestamp : Thu Mar 10 16:43:35 PST 2005
//  \___\/\___\
//
// Revision:
//    03/23/04 - Initial version.
// End Revision

`ifdef legacy_model

`timescale  1 ps / 1 ps

module RAMB16_S1_S2 (DOA, DOB, ADDRA, ADDRB, CLKA, CLKB, DIA, DIB, ENA, ENB, SSRA, SSRB, WEA, WEB);

    parameter INIT_A = 1'h0;
    parameter INIT_B = 2'h0;
    parameter SRVAL_A = 1'h0;
    parameter SRVAL_B = 2'h0;
    parameter WRITE_MODE_A = "WRITE_FIRST";
    parameter WRITE_MODE_B = "WRITE_FIRST";
    parameter SIM_COLLISION_CHECK = "ALL";
    localparam SETUP_ALL = 1000;
    localparam SETUP_READ_FIRST = 3000;

    parameter INIT_00 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_01 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_02 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_03 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_04 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_05 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_06 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_07 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_08 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_09 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_0A = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_0B = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_0C = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_0D = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_0E = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_0F = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_10 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_11 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_12 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_13 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_14 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_15 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_16 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_17 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_18 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_19 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_1A = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_1B = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_1C = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_1D = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_1E = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_1F = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_20 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_21 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_22 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_23 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_24 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_25 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_26 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_27 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_28 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_29 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_2A = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_2B = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_2C = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_2D = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_2E = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_2F = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_30 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_31 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_32 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_33 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_34 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_35 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_36 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_37 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_38 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_39 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_3A = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_3B = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_3C = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_3D = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_3E = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_3F = 256'h0000000000000000000000000000000000000000000000000000000000000000;

    output [0:0] DOA;
    reg [0:0] doa_out;
    wire doa_out0;

    input [13:0] ADDRA;
    input [0:0] DIA;
    input ENA, CLKA, WEA, SSRA;

    output [1:0] DOB;
    reg [1:0] dob_out;
    wire dob_out0, dob_out1;

    input [12:0] ADDRB;
    input [1:0] DIB;
    input ENB, CLKB, WEB, SSRB;

    reg [18431:0] mem;
    reg [8:0] count;
    reg [1:0] wr_mode_a, wr_mode_b;

    reg [5:0] dmi, dbi;
    reg [5:0] pmi, pbi;

    wire [13:0] addra_int;
    reg [13:0] addra_reg;
    wire [0:0] dia_int;
    wire ena_int, clka_int, wea_int, ssra_int;
    reg ena_reg, wea_reg, ssra_reg;
    wire [12:0] addrb_int;
    reg [12:0] addrb_reg;
    wire [1:0] dib_int;
    wire enb_int, clkb_int, web_int, ssrb_int;
    reg display_flag;
    reg enb_reg, web_reg, ssrb_reg;

    time time_clka, time_clkb;
    time time_clka_clkb;
    time time_clkb_clka;

    reg setup_all_a_b;
    reg setup_all_b_a;
    reg setup_zero;
    reg setup_rf_a_b;
    reg setup_rf_b_a;
    reg [1:0] data_collision, data_collision_a_b, data_collision_b_a;
    reg memory_collision, memory_collision_a_b, memory_collision_b_a;
    reg address_collision, address_collision_a_b, address_collision_b_a;
    reg change_clka;
    reg change_clkb;

    wire [14:0] data_addra_int;
    wire [14:0] data_addra_reg;
    wire [14:0] data_addrb_int;
    wire [14:0] data_addrb_reg;
    wire [15:0] parity_addra_int;
    wire [15:0] parity_addra_reg;
    wire [15:0] parity_addrb_int;
    wire [15:0] parity_addrb_reg;

    tri0 GSR = glbl.GSR;

    always @(GSR)
	if (GSR) begin
	    assign doa_out = INIT_A[0:0];
	    assign dob_out = INIT_B[1:0];
	end
	else begin
	    deassign doa_out;
	    deassign dob_out;
	end

    buf b_doa_out0 (doa_out0, doa_out[0]);
    buf b_dob_out0 (dob_out0, dob_out[0]);
    buf b_dob_out1 (dob_out1, dob_out[1]);

    buf b_doa0 (DOA[0], doa_out0);
    buf b_dob0 (DOB[0], dob_out0);
    buf b_dob1 (DOB[1], dob_out1);

    buf b_addra_0 (addra_int[0], ADDRA[0]);
    buf b_addra_1 (addra_int[1], ADDRA[1]);
    buf b_addra_2 (addra_int[2], ADDRA[2]);
    buf b_addra_3 (addra_int[3], ADDRA[3]);
    buf b_addra_4 (addra_int[4], ADDRA[4]);
    buf b_addra_5 (addra_int[5], ADDRA[5]);
    buf b_addra_6 (addra_int[6], ADDRA[6]);
    buf b_addra_7 (addra_int[7], ADDRA[7]);
    buf b_addra_8 (addra_int[8], ADDRA[8]);
    buf b_addra_9 (addra_int[9], ADDRA[9]);
    buf b_addra_10 (addra_int[10], ADDRA[10]);
    buf b_addra_11 (addra_int[11], ADDRA[11]);
    buf b_addra_12 (addra_int[12], ADDRA[12]);
    buf b_addra_13 (addra_int[13], ADDRA[13]);
    buf b_dia_0 (dia_int[0], DIA[0]);
    buf b_ena (ena_int, ENA);
    buf b_clka (clka_int, CLKA);
    buf b_ssra (ssra_int, SSRA);
    buf b_wea (wea_int, WEA);
    buf b_addrb_0 (addrb_int[0], ADDRB[0]);
    buf b_addrb_1 (addrb_int[1], ADDRB[1]);
    buf b_addrb_2 (addrb_int[2], ADDRB[2]);
    buf b_addrb_3 (addrb_int[3], ADDRB[3]);
    buf b_addrb_4 (addrb_int[4], ADDRB[4]);
    buf b_addrb_5 (addrb_int[5], ADDRB[5]);
    buf b_addrb_6 (addrb_int[6], ADDRB[6]);
    buf b_addrb_7 (addrb_int[7], ADDRB[7]);
    buf b_addrb_8 (addrb_int[8], ADDRB[8]);
    buf b_addrb_9 (addrb_int[9], ADDRB[9]);
    buf b_addrb_10 (addrb_int[10], ADDRB[10]);
    buf b_addrb_11 (addrb_int[11], ADDRB[11]);
    buf b_addrb_12 (addrb_int[12], ADDRB[12]);
    buf b_dib_0 (dib_int[0], DIB[0]);
    buf b_dib_1 (dib_int[1], DIB[1]);
    buf b_enb (enb_int, ENB);
    buf b_clkb (clkb_int, CLKB);
    buf b_ssrb (ssrb_int, SSRB);
    buf b_web (web_int, WEB);

    initial begin
	for (count = 0; count < 256; count = count + 1) begin
	    mem[count]		  <= INIT_00[count];
	    mem[256 * 1 + count]  <= INIT_01[count];
	    mem[256 * 2 + count]  <= INIT_02[count];
	    mem[256 * 3 + count]  <= INIT_03[count];
	    mem[256 * 4 + count]  <= INIT_04[count];
	    mem[256 * 5 + count]  <= INIT_05[count];
	    mem[256 * 6 + count]  <= INIT_06[count];
	    mem[256 * 7 + count]  <= INIT_07[count];
	    mem[256 * 8 + count]  <= INIT_08[count];
	    mem[256 * 9 + count]  <= INIT_09[count];
	    mem[256 * 10 + count] <= INIT_0A[count];
	    mem[256 * 11 + count] <= INIT_0B[count];
	    mem[256 * 12 + count] <= INIT_0C[count];
	    mem[256 * 13 + count] <= INIT_0D[count];
	    mem[256 * 14 + count] <= INIT_0E[count];
	    mem[256 * 15 + count] <= INIT_0F[count];
	    mem[256 * 16 + count] <= INIT_10[count];
	    mem[256 * 17 + count] <= INIT_11[count];
	    mem[256 * 18 + count] <= INIT_12[count];
	    mem[256 * 19 + count] <= INIT_13[count];
	    mem[256 * 20 + count] <= INIT_14[count];
	    mem[256 * 21 + count] <= INIT_15[count];
	    mem[256 * 22 + count] <= INIT_16[count];
	    mem[256 * 23 + count] <= INIT_17[count];
	    mem[256 * 24 + count] <= INIT_18[count];
	    mem[256 * 25 + count] <= INIT_19[count];
	    mem[256 * 26 + count] <= INIT_1A[count];
	    mem[256 * 27 + count] <= INIT_1B[count];
	    mem[256 * 28 + count] <= INIT_1C[count];
	    mem[256 * 29 + count] <= INIT_1D[count];
	    mem[256 * 30 + count] <= INIT_1E[count];
	    mem[256 * 31 + count] <= INIT_1F[count];
	    mem[256 * 32 + count] <= INIT_20[count];
	    mem[256 * 33 + count] <= INIT_21[count];
	    mem[256 * 34 + count] <= INIT_22[count];
	    mem[256 * 35 + count] <= INIT_23[count];
	    mem[256 * 36 + count] <= INIT_24[count];
	    mem[256 * 37 + count] <= INIT_25[count];
	    mem[256 * 38 + count] <= INIT_26[count];
	    mem[256 * 39 + count] <= INIT_27[count];
	    mem[256 * 40 + count] <= INIT_28[count];
	    mem[256 * 41 + count] <= INIT_29[count];
	    mem[256 * 42 + count] <= INIT_2A[count];
	    mem[256 * 43 + count] <= INIT_2B[count];
	    mem[256 * 44 + count] <= INIT_2C[count];
	    mem[256 * 45 + count] <= INIT_2D[count];
	    mem[256 * 46 + count] <= INIT_2E[count];
	    mem[256 * 47 + count] <= INIT_2F[count];
	    mem[256 * 48 + count] <= INIT_30[count];
	    mem[256 * 49 + count] <= INIT_31[count];
	    mem[256 * 50 + count] <= INIT_32[count];
	    mem[256 * 51 + count] <= INIT_33[count];
	    mem[256 * 52 + count] <= INIT_34[count];
	    mem[256 * 53 + count] <= INIT_35[count];
	    mem[256 * 54 + count] <= INIT_36[count];
	    mem[256 * 55 + count] <= INIT_37[count];
	    mem[256 * 56 + count] <= INIT_38[count];
	    mem[256 * 57 + count] <= INIT_39[count];
	    mem[256 * 58 + count] <= INIT_3A[count];
	    mem[256 * 59 + count] <= INIT_3B[count];
	    mem[256 * 60 + count] <= INIT_3C[count];
	    mem[256 * 61 + count] <= INIT_3D[count];
	    mem[256 * 62 + count] <= INIT_3E[count];
	    mem[256 * 63 + count] <= INIT_3F[count];
	end
	address_collision <= 0;
	address_collision_a_b <= 0;
	address_collision_b_a <= 0;
	change_clka <= 0;
	change_clkb <= 0;
	data_collision <= 0;
	data_collision_a_b <= 0;
	data_collision_b_a <= 0;
	memory_collision <= 0;
	memory_collision_a_b <= 0;
	memory_collision_b_a <= 0;
	setup_all_a_b <= 0;
	setup_all_b_a <= 0;
	setup_zero <= 0;
	setup_rf_a_b <= 0;
	setup_rf_b_a <= 0;
    end

    assign data_addra_int = addra_int * 1;
    assign data_addra_reg = addra_reg * 1;
    assign data_addrb_int = addrb_int * 2;
    assign data_addrb_reg = addrb_reg * 2;


    initial begin

	display_flag = 1;

	case (SIM_COLLISION_CHECK)

	    "NONE" : begin
		         assign setup_all_a_b = 1'b0;
	                 assign setup_all_b_a = 1'b0;
	                 assign setup_zero = 1'b0;
	                 assign setup_rf_a_b = 1'b0;
	                 assign setup_rf_b_a = 1'b0;
	                 assign display_flag = 0;
	             end
	    "WARNING_ONLY" : begin
		                 assign data_collision = 2'b00;
	                         assign data_collision_a_b = 2'b00;
	                         assign data_collision_b_a = 2'b00;
	                         assign memory_collision = 1'b0;
	                         assign memory_collision_a_b = 1'b0;
	                         assign memory_collision_b_a = 1'b0;
	                     end
	    "GENERATE_X_ONLY" : begin
		                    assign display_flag = 0;
	                        end
	    "ALL" : ;
	    default : begin
		          $display("Attribute Syntax Error : The Attribute SIM_COLLISION_CHECK on RAMB16_S1_S2 instance %m is set to %s.  Legal values for this attribute are ALL, NONE, WARNING_ONLY or GENERATE_X_ONLY.", SIM_COLLISION_CHECK);
		          $finish;
	              end

	endcase // case(SIM_COLLISION_CHECK)

    end // initial begin


    always @(posedge clka_int) begin
	time_clka = $time;
	#0 time_clkb_clka = time_clka - time_clkb;
	change_clka = ~change_clka;
    end

    always @(posedge clkb_int) begin
	time_clkb = $time;
	#0 time_clka_clkb = time_clkb - time_clka;
	change_clkb = ~change_clkb;
    end

    always @(change_clkb) begin
	if ((0 < time_clka_clkb) && (time_clka_clkb < SETUP_ALL))
	    setup_all_a_b = 1;
	if ((0 < time_clka_clkb) && (time_clka_clkb < SETUP_READ_FIRST))
	    setup_rf_a_b = 1;
    end

    always @(change_clka) begin
	if ((0 < time_clkb_clka) && (time_clkb_clka < SETUP_ALL))
	    setup_all_b_a = 1;
	if ((0 < time_clkb_clka) && (time_clkb_clka < SETUP_READ_FIRST))
	    setup_rf_b_a = 1;
    end

    always @(change_clkb or change_clka) begin
	if ((time_clkb_clka == 0) && (time_clka_clkb == 0))
	    setup_zero = 1;
    end

    always @(posedge setup_zero) begin
	if ((ena_int == 1) && (wea_int == 1) &&
	    (enb_int == 1) && (web_int == 1) &&
	    (data_addra_int[14:1] == data_addrb_int[14:1]))
	    memory_collision <= 1;
    end

    always @(posedge setup_all_a_b or posedge setup_rf_a_b) begin
	if ((ena_reg == 1) && (wea_reg == 1) &&
	    (enb_int == 1) && (web_int == 1) &&
	    (data_addra_reg[14:1] == data_addrb_int[14:1]))
	    memory_collision_a_b <= 1;
    end

    always @(posedge setup_all_b_a or posedge setup_rf_b_a) begin
	if ((ena_int == 1) && (wea_int == 1) &&
	    (enb_reg == 1) && (web_reg == 1) &&
	    (data_addra_int[14:1] == data_addrb_reg[14:1]))
	    memory_collision_b_a <= 1;
    end

    always @(posedge setup_all_a_b) begin
	if (data_addra_reg[14:1] == data_addrb_int[14:1]) begin
	if ((ena_reg == 1) && (enb_int == 1)) begin
	    case ({wr_mode_a, wr_mode_b, wea_reg, web_int})
		6'b000011 : begin data_collision_a_b <= 2'b11; display_wa_wb; end
		6'b000111 : begin data_collision_a_b <= 2'b11; display_wa_wb; end
		6'b001011 : begin data_collision_a_b <= 2'b10; display_wa_wb; end
//		6'b010011 : begin data_collision_a_b <= 2'b00; display_wa_wb; end
//		6'b010111 : begin data_collision_a_b <= 2'b00; display_wa_wb; end
//		6'b011011 : begin data_collision_a_b <= 2'b00; display_wa_wb; end
		6'b100011 : begin data_collision_a_b <= 2'b01; display_wa_wb; end
		6'b100111 : begin data_collision_a_b <= 2'b01; display_wa_wb; end
		6'b101011 : begin display_wa_wb; end
		6'b000001 : begin data_collision_a_b <= 2'b10; display_ra_wb; end
//		6'b000101 : begin data_collision_a_b <= 2'b00; display_ra_wb; end
		6'b001001 : begin data_collision_a_b <= 2'b10; display_ra_wb; end
		6'b010001 : begin data_collision_a_b <= 2'b10; display_ra_wb; end
//		6'b010101 : begin data_collision_a_b <= 2'b00; display_ra_wb; end
		6'b011001 : begin data_collision_a_b <= 2'b10; display_ra_wb; end
		6'b100001 : begin data_collision_a_b <= 2'b10; display_ra_wb; end
//		6'b100101 : begin data_collision_a_b <= 2'b00; display_ra_wb; end
		6'b101001 : begin data_collision_a_b <= 2'b10; display_ra_wb; end
		6'b000010 : begin data_collision_a_b <= 2'b01; display_wa_rb; end
		6'b000110 : begin data_collision_a_b <= 2'b01; display_wa_rb; end
		6'b001010 : begin data_collision_a_b <= 2'b01; display_wa_rb; end
//		6'b010010 : begin data_collision_a_b <= 2'b00; display_wa_rb; end
//		6'b010110 : begin data_collision_a_b <= 2'b00; display_wa_rb; end
//		6'b011010 : begin data_collision_a_b <= 2'b00; display_wa_rb; end
		6'b100010 : begin data_collision_a_b <= 2'b01; display_wa_rb; end
		6'b100110 : begin data_collision_a_b <= 2'b01; display_wa_rb; end
		6'b101010 : begin data_collision_a_b <= 2'b01; display_wa_rb; end
	    endcase
	end
	end
	setup_all_a_b <= 0;
    end


    always @(posedge setup_all_b_a) begin
	if (data_addra_int[14:1] == data_addrb_reg[14:1]) begin
	if ((ena_int == 1) && (enb_reg == 1)) begin
	    case ({wr_mode_a, wr_mode_b, wea_int, web_reg})
		6'b000011 : begin data_collision_b_a <= 2'b11; display_wa_wb; end
//		6'b000111 : begin data_collision_b_a <= 2'b00; display_wa_wb; end
		6'b001011 : begin data_collision_b_a <= 2'b10; display_wa_wb; end
		6'b010011 : begin data_collision_b_a <= 2'b11; display_wa_wb; end
//		6'b010111 : begin data_collision_b_a <= 2'b00; display_wa_wb; end
		6'b011011 : begin data_collision_b_a <= 2'b10; display_wa_wb; end
		6'b100011 : begin data_collision_b_a <= 2'b01; display_wa_wb; end
		6'b100111 : begin data_collision_b_a <= 2'b01; display_wa_wb; end
		6'b101011 : begin display_wa_wb; end
		6'b000001 : begin data_collision_b_a <= 2'b10; display_ra_wb; end
		6'b000101 : begin data_collision_b_a <= 2'b10; display_ra_wb; end
		6'b001001 : begin data_collision_b_a <= 2'b10; display_ra_wb; end
		6'b010001 : begin data_collision_b_a <= 2'b10; display_ra_wb; end
		6'b010101 : begin data_collision_b_a <= 2'b10; display_ra_wb; end
		6'b011001 : begin data_collision_b_a <= 2'b10; display_ra_wb; end
		6'b100001 : begin data_collision_b_a <= 2'b10; display_ra_wb; end
		6'b100101 : begin data_collision_b_a <= 2'b10; display_ra_wb; end
		6'b101001 : begin data_collision_b_a <= 2'b10; display_ra_wb; end
		6'b000010 : begin data_collision_b_a <= 2'b01; display_wa_rb; end
		6'b000110 : begin data_collision_b_a <= 2'b01; display_wa_rb; end
		6'b001010 : begin data_collision_b_a <= 2'b01; display_wa_rb; end
//		6'b010010 : begin data_collision_b_a <= 2'b00; display_wa_rb; end
//		6'b010110 : begin data_collision_b_a <= 2'b00; display_wa_rb; end
//		6'b011010 : begin data_collision_b_a <= 2'b00; display_wa_rb; end
		6'b100010 : begin data_collision_b_a <= 2'b01; display_wa_rb; end
		6'b100110 : begin data_collision_b_a <= 2'b01; display_wa_rb; end
		6'b101010 : begin data_collision_b_a <= 2'b01; display_wa_rb; end
	    endcase
	end
	end
	setup_all_b_a <= 0;
    end


    always @(posedge setup_zero) begin
	if (data_addra_int[14:1] == data_addrb_int[14:1]) begin
	if ((ena_int == 1) && (enb_int == 1)) begin
	    case ({wr_mode_a, wr_mode_b, wea_int, web_int})
		6'b000011 : begin data_collision <= 2'b11; display_wa_wb; end
		6'b000111 : begin data_collision <= 2'b11; display_wa_wb; end
		6'b001011 : begin data_collision <= 2'b10; display_wa_wb; end
		6'b010011 : begin data_collision <= 2'b11; display_wa_wb; end
		6'b010111 : begin data_collision <= 2'b11; display_wa_wb; end
		6'b011011 : begin data_collision <= 2'b10; display_wa_wb; end
		6'b100011 : begin data_collision <= 2'b01; display_wa_wb; end
		6'b100111 : begin data_collision <= 2'b01; display_wa_wb; end
		6'b101011 : begin display_wa_wb; end
		6'b000001 : begin data_collision <= 2'b10; display_ra_wb; end
//		6'b000101 : begin data_collision <= 2'b00; display_ra_wb; end
		6'b001001 : begin data_collision <= 2'b10; display_ra_wb; end
		6'b010001 : begin data_collision <= 2'b10; display_ra_wb; end
//		6'b010101 : begin data_collision <= 2'b00; display_ra_wb; end
		6'b011001 : begin data_collision <= 2'b10; display_ra_wb; end
		6'b100001 : begin data_collision <= 2'b10; display_ra_wb; end
//		6'b100101 : begin data_collision <= 2'b00; display_ra_wb; end
		6'b101001 : begin data_collision <= 2'b10; display_ra_wb; end
		6'b000010 : begin data_collision <= 2'b01; display_wa_rb; end
		6'b000110 : begin data_collision <= 2'b01; display_wa_rb; end
		6'b001010 : begin data_collision <= 2'b01; display_wa_rb; end
//		6'b010010 : begin data_collision <= 2'b00; display_wa_rb; end
//		6'b010110 : begin data_collision <= 2'b00; display_wa_rb; end
//		6'b011010 : begin data_collision <= 2'b00; display_wa_rb; end
		6'b100010 : begin data_collision <= 2'b01; display_wa_rb; end
		6'b100110 : begin data_collision <= 2'b01; display_wa_rb; end
		6'b101010 : begin data_collision <= 2'b01; display_wa_rb; end
	    endcase
	end
	end
	setup_zero <= 0;
    end

    task display_ra_wb;
    begin
	if (display_flag)
        $display("Memory Collision Error on RAMB16_S1_S2:%m at simulation time %.3f ns\nA read was performed on address %h (hex) of Port A while a write was requested to the same address on Port B. The write will be successful however the read value on Port A is unknown until the next CLKA cycle.", $time/1000.0, addra_int);
    end
    endtask

    task display_wa_rb;
    begin
	if (display_flag)
        $display("Memory Collision Error on RAMB16_S1_S2:%m at simulation time %.3f ns\nA read was performed on address %h (hex) of Port B while a write was requested to the same address on Port A. The write will be successful however the read value on Port B is unknown until the next CLKB cycle.", $time/1000.0, addrb_int);
    end
    endtask

    task display_wa_wb;
    begin
	if (display_flag)
        $display("Memory Collision Error on RAMB16_S1_S2:%m at simulation time %.3f ns\nA write was requested to the same address simultaneously at both Port A and Port B of the RAM. The contents written to the RAM at address location %h (hex) of Port A and address location %h (hex) of Port B are unknown.", $time/1000.0, addra_int, addrb_int);
    end
    endtask


    always @(posedge setup_rf_a_b) begin
	if (data_addra_reg[14:1] == data_addrb_int[14:1]) begin
	if ((ena_reg == 1) && (enb_int == 1)) begin
	    case ({wr_mode_a, wr_mode_b, wea_reg, web_int})
//		6'b000011 : begin data_collision_a_b <= 2'b00; display_wa_wb; end
//		6'b000111 : begin data_collision_a_b <= 2'b00; display_wa_wb; end
//		6'b001011 : begin data_collision_a_b <= 2'b00; display_wa_wb; end
		6'b010011 : begin data_collision_a_b <= 2'b11; display_wa_wb; end
		6'b010111 : begin data_collision_a_b <= 2'b11; display_wa_wb; end
		6'b011011 : begin data_collision_a_b <= 2'b10; display_wa_wb; end
//		6'b100011 : begin data_collision_a_b <= 2'b00; display_wa_wb; end
//		6'b100111 : begin data_collision_a_b <= 2'b00; display_wa_wb; end
//		6'b101011 : begin data_collision_a_b <= 2'b00; display_wa_wb; end
//		6'b000001 : begin data_collision_a_b <= 2'b00; display_ra_wb; end
//		6'b000101 : begin data_collision_a_b <= 2'b00; display_ra_wb; end
//		6'b001001 : begin data_collision_a_b <= 2'b00; display_ra_wb; end
//		6'b010001 : begin data_collision_a_b <= 2'b00; display_ra_wb; end
//		6'b010101 : begin data_collision_a_b <= 2'b00; display_ra_wb; end
//		6'b011001 : begin data_collision_a_b <= 2'b00; display_ra_wb; end
//		6'b100001 : begin data_collision_a_b <= 2'b00; display_ra_wb; end
//		6'b100101 : begin data_collision_a_b <= 2'b00; display_ra_wb; end
//		6'b101001 : begin data_collision_a_b <= 2'b00; display_ra_wb; end
//		6'b000010 : begin data_collision_a_b <= 2'b00; display_wa_rb; end
//		6'b000110 : begin data_collision_a_b <= 2'b00; display_wa_rb; end
//		6'b001010 : begin data_collision_a_b <= 2'b00; display_wa_rb; end
		6'b010010 : begin data_collision_a_b <= 2'b01; display_wa_rb; end
		6'b010110 : begin data_collision_a_b <= 2'b01; display_wa_rb; end
		6'b011010 : begin data_collision_a_b <= 2'b01; display_wa_rb; end
//		6'b100010 : begin data_collision_a_b <= 2'b00; display_wa_rb; end
//		6'b100110 : begin data_collision_a_b <= 2'b00; display_wa_rb; end
//		6'b101010 : begin data_collision_a_b <= 2'b00; display_wa_rb; end
	    endcase
	end
	end
	setup_rf_a_b <= 0;
    end


    always @(posedge setup_rf_b_a) begin
	if (data_addra_int[14:1] == data_addrb_reg[14:1]) begin
	if ((ena_int == 1) && (enb_reg == 1)) begin
	    case ({wr_mode_a, wr_mode_b, wea_int, web_reg})
//		6'b000011 : begin data_collision_b_a <= 2'b00; display_wa_wb; end
		6'b000111 : begin data_collision_b_a <= 2'b11; display_wa_wb; end
//		6'b001011 : begin data_collision_b_a <= 2'b00; display_wa_wb; end
//		6'b010011 : begin data_collision_b_a <= 2'b00; display_wa_wb; end
		6'b010111 : begin data_collision_b_a <= 2'b11; display_wa_wb; end
//		6'b011011 : begin data_collision_b_a <= 2'b00; display_wa_wb; end
//		6'b100011 : begin data_collision_b_a <= 2'b00; display_wa_wb; end
		6'b100111 : begin data_collision_b_a <= 2'b01; display_wa_wb; end
//		6'b101011 : begin data_collision_b_a <= 2'b00; display_wa_wb; end
//		6'b000001 : begin data_collision_b_a <= 2'b00; display_ra_wb; end
		6'b000101 : begin data_collision_b_a <= 2'b10; display_ra_wb; end
//		6'b001001 : begin data_collision_b_a <= 2'b00; display_ra_wb; end
//		6'b010001 : begin data_collision_b_a <= 2'b00; display_ra_wb; end
		6'b010101 : begin data_collision_b_a <= 2'b10; display_ra_wb; end
//		6'b011001 : begin data_collision_b_a <= 2'b00; display_ra_wb; end
//		6'b100001 : begin data_collision_b_a <= 2'b00; display_ra_wb; end
		6'b100101 : begin data_collision_b_a <= 2'b10; display_ra_wb; end
//		6'b101001 : begin data_collision_b_a <= 2'b00; display_ra_wb; end
//		6'b000010 : begin data_collision_b_a <= 2'b00; display_wa_rb; end
//		6'b000110 : begin data_collision_b_a <= 2'b00; display_wa_rb; end
//		6'b001010 : begin data_collision_b_a <= 2'b00; display_wa_rb; end
//		6'b010010 : begin data_collision_b_a <= 2'b00; display_wa_rb; end
//		6'b010110 : begin data_collision_b_a <= 2'b00; display_wa_rb; end
//		6'b011010 : begin data_collision_b_a <= 2'b00; display_wa_rb; end
//		6'b100010 : begin data_collision_b_a <= 2'b00; display_wa_rb; end
//		6'b100110 : begin data_collision_b_a <= 2'b00; display_wa_rb; end
//		6'b101010 : begin data_collision_b_a <= 2'b00; display_wa_rb; end
	    endcase
	end
	end
	setup_rf_b_a <= 0;
    end


    always @(posedge clka_int) begin
	addra_reg <= addra_int;
	ena_reg <= ena_int;
	ssra_reg <= ssra_int;
	wea_reg <= wea_int;
    end

    always @(posedge clkb_int) begin
	addrb_reg <= addrb_int;
	enb_reg <= enb_int;
	ssrb_reg <= ssrb_int;
	web_reg <= web_int;
    end

    // Data
    always @(posedge memory_collision) begin
	for (dmi = 0; dmi < 1; dmi = dmi + 1) begin
	    mem[data_addra_int + dmi] <= 1'bX;
	end
	memory_collision <= 0;
    end

    always @(posedge memory_collision_a_b) begin
	for (dmi = 0; dmi < 1; dmi = dmi + 1) begin
	    mem[data_addra_reg + dmi] <= 1'bX;
	end
	memory_collision_a_b <= 0;
    end

    always @(posedge memory_collision_b_a) begin
	for (dmi = 0; dmi < 1; dmi = dmi + 1) begin
	    mem[data_addra_int + dmi] <= 1'bX;
	end
	memory_collision_b_a <= 0;
    end

    always @(posedge data_collision[1]) begin
	if (ssra_int == 0) begin
	    doa_out <= 1'bX;
	end
	data_collision[1] <= 0;
    end

    always @(posedge data_collision[0]) begin
	if (ssrb_int == 0) begin
	    for (dbi = 0; dbi < 1; dbi = dbi + 1) begin
		dob_out[data_addra_int[0 : 0] + dbi] <= 1'bX;
	    end
	end
	data_collision[0] <= 0;
    end

    always @(posedge data_collision_a_b[1]) begin
	if (ssra_reg == 0) begin
	    doa_out <= 1'bX;
	end
	data_collision_a_b[1] <= 0;
    end

    always @(posedge data_collision_a_b[0]) begin
	if (ssrb_int == 0) begin
	    for (dbi = 0; dbi < 1; dbi = dbi + 1) begin
		dob_out[data_addra_reg[0 : 0] + dbi] <= 1'bX;
	    end
	end
	data_collision_a_b[0] <= 0;
    end

    always @(posedge data_collision_b_a[1]) begin
	if (ssra_int == 0) begin
	    doa_out <= 1'bX;
	end
	data_collision_b_a[1] <= 0;
    end

    always @(posedge data_collision_b_a[0]) begin
	if (ssrb_reg == 0) begin
	    for (dbi = 0; dbi < 1; dbi = dbi + 1) begin
		dob_out[data_addra_int[0 : 0] + dbi] <= 1'bX;
	    end
	end
	data_collision_b_a[0] <= 0;
    end


    initial begin
	case (WRITE_MODE_A)
	    "WRITE_FIRST" : wr_mode_a <= 2'b00;
	    "READ_FIRST"  : wr_mode_a <= 2'b01;
	    "NO_CHANGE"   : wr_mode_a <= 2'b10;
	    default       : begin
				$display("Attribute Syntax Error : The Attribute WRITE_MODE_A on RAMB16_S1_S2 instance %m is set to %s.  Legal values for this attribute are WRITE_FIRST, READ_FIRST or NO_CHANGE.", WRITE_MODE_A);
				$finish;
			    end
	endcase
    end

    initial begin
	case (WRITE_MODE_B)
	    "WRITE_FIRST" : wr_mode_b <= 2'b00;
	    "READ_FIRST"  : wr_mode_b <= 2'b01;
	    "NO_CHANGE"   : wr_mode_b <= 2'b10;
	    default       : begin
				$display("Attribute Syntax Error : The Attribute WRITE_MODE_B on RAMB16_S1_S2 instance %m is set to %s.  Legal values for this attribute are WRITE_FIRST, READ_FIRST or NO_CHANGE.", WRITE_MODE_B);
				$finish;
			    end
	endcase
    end

    // Port A
    always @(posedge clka_int) begin
	if (ena_int == 1'b1) begin
	    if (ssra_int == 1'b1) begin
		doa_out[0] <= SRVAL_A[0];
	    end
	    else begin
		if (wea_int == 1'b1) begin
		    if (wr_mode_a == 2'b00) begin
			doa_out <= dia_int;
		    end
		    else if (wr_mode_a == 2'b01) begin
			doa_out[0] <= mem[data_addra_int + 0];
		    end
		end
		else begin
		    doa_out[0] <= mem[data_addra_int + 0];
		end
	    end
	end
    end

    always @(posedge clka_int) begin
	if (ena_int == 1'b1 && wea_int == 1'b1) begin
	    mem[data_addra_int + 0] <= dia_int[0];
	end
    end

    // Port B
    always @(posedge clkb_int) begin
	if (enb_int == 1'b1) begin
	    if (ssrb_int == 1'b1) begin
		dob_out[0] <= SRVAL_B[0];
		dob_out[1] <= SRVAL_B[1];
	    end
	    else begin
		if (web_int == 1'b1) begin
		    if (wr_mode_b == 2'b00) begin
			dob_out <= dib_int;
		    end
		    else if (wr_mode_b == 2'b01) begin
			dob_out[0] <= mem[data_addrb_int + 0];
			dob_out[1] <= mem[data_addrb_int + 1];
		    end
		end
		else begin
		    dob_out[0] <= mem[data_addrb_int + 0];
		    dob_out[1] <= mem[data_addrb_int + 1];
		end
	    end
	end
    end

    always @(posedge clkb_int) begin
	if (enb_int == 1'b1 && web_int == 1'b1) begin
	    mem[data_addrb_int + 0] <= dib_int[0];
	    mem[data_addrb_int + 1] <= dib_int[1];
	end
    end

    specify
	(CLKA *> DOA) = (100, 100);
	(CLKB *> DOB) = (100, 100);
    endspecify

endmodule

`else

// $Header: /devl/xcs/repo/env/Databases/CAEInterfaces/verunilibs/data/unisims/RAMB16_S1_S2.v,v 1.10 2005/03/14 22:54:41 wloo Exp $
///////////////////////////////////////////////////////////////////////////////
// Copyright (c) 1995/2005 Xilinx, Inc.
// All Right Reserved.
///////////////////////////////////////////////////////////////////////////////
//   ____  ____
//  /   /\/   /
// /___/  \  /    Vendor : Xilinx
// \   \   \/     Version : 8.1i (I.13)
//  \   \         Description : Xilinx Timing Simulation Library Component
//  /   /                  16K-Bit Data and 2K-Bit Parity Dual Port Block RAM
// /___/   /\     Filename : RAMB16_S1_S2.v
// \   \  /  \    Timestamp : Thu Mar 10 16:44:01 PST 2005
//  \___\/\___\
//
// Revision:
//    03/23/04 - Initial version.
//    03/10/05 - Initialized outputs.
// End Revision

`timescale 1 ps/1 ps

module RAMB16_S1_S2 (DOA, DOB, ADDRA, ADDRB, CLKA, CLKB, DIA, DIB, ENA, ENB, SSRA, SSRB, WEA, WEB);

    parameter INIT_A = 1'h0;
    parameter INIT_B = 2'h0;
    parameter SRVAL_A = 1'h0;
    parameter SRVAL_B = 2'h0;
    parameter WRITE_MODE_A = "WRITE_FIRST";
    parameter WRITE_MODE_B = "WRITE_FIRST";
    parameter SIM_COLLISION_CHECK = "ALL";
    localparam SETUP_ALL = 1000;
    localparam SETUP_READ_FIRST = 3000;

    parameter INIT_00 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_01 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_02 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_03 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_04 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_05 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_06 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_07 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_08 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_09 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_0A = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_0B = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_0C = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_0D = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_0E = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_0F = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_10 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_11 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_12 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_13 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_14 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_15 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_16 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_17 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_18 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_19 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_1A = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_1B = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_1C = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_1D = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_1E = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_1F = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_20 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_21 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_22 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_23 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_24 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_25 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_26 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_27 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_28 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_29 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_2A = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_2B = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_2C = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_2D = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_2E = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_2F = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_30 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_31 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_32 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_33 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_34 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_35 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_36 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_37 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_38 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_39 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_3A = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_3B = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_3C = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_3D = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_3E = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INIT_3F = 256'h0000000000000000000000000000000000000000000000000000000000000000;

    output [0:0] DOA;
    output [1:0] DOB;

    input [13:0] ADDRA;
    input [0:0] DIA;
    input ENA, CLKA, WEA, SSRA;
    input [12:0] ADDRB;
    input [1:0] DIB;
    input ENB, CLKB, WEB, SSRB;

    reg [0:0] doa_out = INIT_A[0:0];
    reg [1:0] dob_out = INIT_B[1:0];
    
    reg [1:0] mem [8191:0];
    
    reg [8:0] count, countp;
    reg [1:0] wr_mode_a, wr_mode_b;

    reg [5:0] dmi, dbi;
    reg [5:0] pmi, pbi;

    wire [13:0] addra_int;
    reg [13:0] addra_reg;
    wire [0:0] dia_int;
    wire ena_int, clka_int, wea_int, ssra_int;
    reg ena_reg, wea_reg, ssra_reg;
    wire [12:0] addrb_int;
    reg [12:0] addrb_reg;
    wire [1:0] dib_int;
    wire enb_int, clkb_int, web_int, ssrb_int;
    reg display_flag, output_flag;
    reg enb_reg, web_reg, ssrb_reg;

    time time_clka, time_clkb;
    time time_clka_clkb;
    time time_clkb_clka;

    reg setup_all_a_b;
    reg setup_all_b_a;
    reg setup_zero;
    reg setup_rf_a_b;
    reg setup_rf_b_a;
    reg [1:0] data_collision, data_collision_a_b, data_collision_b_a;
    reg memory_collision, memory_collision_a_b, memory_collision_b_a;
    reg change_clka;
    reg change_clkb;

    wire [14:0] data_addra_int;
    wire [14:0] data_addra_reg;
    wire [14:0] data_addrb_int;
    wire [14:0] data_addrb_reg;

    wire dia_enable = ena_int && wea_int;
    wire dib_enable = enb_int && web_int;

    tri0 GSR = glbl.GSR;
    wire gsr_int;

    buf b_gsr (gsr_int, GSR);

    buf b_doa [0:0] (DOA, doa_out);
    buf b_addra [13:0] (addra_int, ADDRA);
    buf b_dia [0:0] (dia_int, DIA);
    buf b_ena (ena_int, ENA);
    buf b_clka (clka_int, CLKA);
    buf b_ssra (ssra_int, SSRA);
    buf b_wea (wea_int, WEA);

    buf b_dob [1:0] (DOB, dob_out);
    buf b_addrb [12:0] (addrb_int, ADDRB);
    buf b_dib [1:0] (dib_int, DIB);
    buf b_enb (enb_int, ENB);
    buf b_clkb (clkb_int, CLKB);
    buf b_ssrb (ssrb_int, SSRB);
    buf b_web (web_int, WEB);

    
    always @(gsr_int)
	if (gsr_int) begin
	    assign {doa_out} = INIT_A;
	    assign {dob_out} = INIT_B;
	end
	else begin
	    deassign doa_out;
	    deassign dob_out;
	end

    
    initial begin

	for (count = 0; count < 128; count = count + 1) begin
	    mem[count]          = INIT_00[(count * 2) +: 2];
	    mem[128 * 1 + count]  = INIT_01[(count * 2) +: 2];
	    mem[128 * 2 + count]  = INIT_02[(count * 2) +: 2];
	    mem[128 * 3 + count]  = INIT_03[(count * 2) +: 2];
	    mem[128 * 4 + count]  = INIT_04[(count * 2) +: 2];
	    mem[128 * 5 + count]  = INIT_05[(count * 2) +: 2];
	    mem[128 * 6 + count]  = INIT_06[(count * 2) +: 2];
	    mem[128 * 7 + count]  = INIT_07[(count * 2) +: 2];
	    mem[128 * 8 + count]  = INIT_08[(count * 2) +: 2];
	    mem[128 * 9 + count]  = INIT_09[(count * 2) +: 2];
	    mem[128 * 10 + count] = INIT_0A[(count * 2) +: 2];
	    mem[128 * 11 + count] = INIT_0B[(count * 2) +: 2];
	    mem[128 * 12 + count] = INIT_0C[(count * 2) +: 2];
	    mem[128 * 13 + count] = INIT_0D[(count * 2) +: 2];
	    mem[128 * 14 + count] = INIT_0E[(count * 2) +: 2];
	    mem[128 * 15 + count] = INIT_0F[(count * 2) +: 2];
	    mem[128 * 16 + count] = INIT_10[(count * 2) +: 2];
	    mem[128 * 17 + count] = INIT_11[(count * 2) +: 2];
	    mem[128 * 18 + count] = INIT_12[(count * 2) +: 2];
	    mem[128 * 19 + count] = INIT_13[(count * 2) +: 2];
	    mem[128 * 20 + count] = INIT_14[(count * 2) +: 2];
	    mem[128 * 21 + count] = INIT_15[(count * 2) +: 2];
	    mem[128 * 22 + count] = INIT_16[(count * 2) +: 2];
	    mem[128 * 23 + count] = INIT_17[(count * 2) +: 2];
	    mem[128 * 24 + count] = INIT_18[(count * 2) +: 2];
	    mem[128 * 25 + count] = INIT_19[(count * 2) +: 2];
	    mem[128 * 26 + count] = INIT_1A[(count * 2) +: 2];
	    mem[128 * 27 + count] = INIT_1B[(count * 2) +: 2];
	    mem[128 * 28 + count] = INIT_1C[(count * 2) +: 2];
	    mem[128 * 29 + count] = INIT_1D[(count * 2) +: 2];
	    mem[128 * 30 + count] = INIT_1E[(count * 2) +: 2];
	    mem[128 * 31 + count] = INIT_1F[(count * 2) +: 2];
	    mem[128 * 32 + count] = INIT_20[(count * 2) +: 2];
	    mem[128 * 33 + count] = INIT_21[(count * 2) +: 2];
	    mem[128 * 34 + count] = INIT_22[(count * 2) +: 2];
	    mem[128 * 35 + count] = INIT_23[(count * 2) +: 2];
	    mem[128 * 36 + count] = INIT_24[(count * 2) +: 2];
	    mem[128 * 37 + count] = INIT_25[(count * 2) +: 2];
	    mem[128 * 38 + count] = INIT_26[(count * 2) +: 2];
	    mem[128 * 39 + count] = INIT_27[(count * 2) +: 2];
	    mem[128 * 40 + count] = INIT_28[(count * 2) +: 2];
	    mem[128 * 41 + count] = INIT_29[(count * 2) +: 2];
	    mem[128 * 42 + count] = INIT_2A[(count * 2) +: 2];
	    mem[128 * 43 + count] = INIT_2B[(count * 2) +: 2];
	    mem[128 * 44 + count] = INIT_2C[(count * 2) +: 2];
	    mem[128 * 45 + count] = INIT_2D[(count * 2) +: 2];
	    mem[128 * 46 + count] = INIT_2E[(count * 2) +: 2];
	    mem[128 * 47 + count] = INIT_2F[(count * 2) +: 2];
	    mem[128 * 48 + count] = INIT_30[(count * 2) +: 2];
	    mem[128 * 49 + count] = INIT_31[(count * 2) +: 2];
	    mem[128 * 50 + count] = INIT_32[(count * 2) +: 2];
	    mem[128 * 51 + count] = INIT_33[(count * 2) +: 2];
	    mem[128 * 52 + count] = INIT_34[(count * 2) +: 2];
	    mem[128 * 53 + count] = INIT_35[(count * 2) +: 2];
	    mem[128 * 54 + count] = INIT_36[(count * 2) +: 2];
	    mem[128 * 55 + count] = INIT_37[(count * 2) +: 2];
	    mem[128 * 56 + count] = INIT_38[(count * 2) +: 2];
	    mem[128 * 57 + count] = INIT_39[(count * 2) +: 2];
	    mem[128 * 58 + count] = INIT_3A[(count * 2) +: 2];
	    mem[128 * 59 + count] = INIT_3B[(count * 2) +: 2];
	    mem[128 * 60 + count] = INIT_3C[(count * 2) +: 2];
	    mem[128 * 61 + count] = INIT_3D[(count * 2) +: 2];
	    mem[128 * 62 + count] = INIT_3E[(count * 2) +: 2];
	    mem[128 * 63 + count] = INIT_3F[(count * 2) +: 2];
	end

	
	change_clka <= 0;
	change_clkb <= 0;
	data_collision <= 0;
	data_collision_a_b <= 0;
	data_collision_b_a <= 0;
	memory_collision <= 0;
	memory_collision_a_b <= 0;
	memory_collision_b_a <= 0;
	setup_all_a_b <= 0;
	setup_all_b_a <= 0;
	setup_zero <= 0;
	setup_rf_a_b <= 0;
	setup_rf_b_a <= 0;
    end

    assign data_addra_int = addra_int * 1;
    assign data_addra_reg = addra_reg * 1;
    assign data_addrb_int = addrb_int * 2;
    assign data_addrb_reg = addrb_reg * 2;


    initial begin

	display_flag = 1;
	output_flag = 1;
	
	case (SIM_COLLISION_CHECK)

	    "NONE" : begin
		         output_flag = 0;
	                 display_flag = 0;
	             end
	    "WARNING_ONLY" : output_flag = 0;
	    "GENERATE_ONLY" : display_flag = 0;
	    "ALL" : ;

	    default : begin
		          $display("Attribute Syntax Error : The Attribute SIM_COLLISION_CHECK on RAMB16_S1_S2 instance %m is set to %s.  Legal values for this attribute are ALL, NONE, WARNING_ONLY or GENERATE_ONLY.", SIM_COLLISION_CHECK);
		          $finish;
	              end

	endcase // case(SIM_COLLISION_CHECK)

    end // initial begin

    
    always @(posedge clka_int) begin
	if ((output_flag || display_flag)) begin
	    time_clka = $time;
	    #0 time_clkb_clka = time_clka - time_clkb;
	    change_clka = ~change_clka;
	end
    end
    
    always @(posedge clkb_int) begin
	if ((output_flag || display_flag)) begin
	    time_clkb = $time;
	    #0 time_clka_clkb = time_clkb - time_clka;
	    change_clkb = ~change_clkb;
	end
    end
    
    always @(change_clkb) begin
	if ((0 < time_clka_clkb) && (time_clka_clkb < SETUP_ALL))
	    setup_all_a_b = 1;
	if ((0 < time_clka_clkb) && (time_clka_clkb < SETUP_READ_FIRST))
	    setup_rf_a_b = 1;
    end

    always @(change_clka) begin
	if ((0 < time_clkb_clka) && (time_clkb_clka < SETUP_ALL))
	    setup_all_b_a = 1;
	if ((0 < time_clkb_clka) && (time_clkb_clka < SETUP_READ_FIRST))
	    setup_rf_b_a = 1;
    end

    always @(change_clkb or change_clka) begin
	if ((time_clkb_clka == 0) && (time_clka_clkb == 0))
	    setup_zero = 1;
    end

    always @(posedge setup_zero) begin
	if ((ena_int == 1) && (wea_int == 1) &&
	    (enb_int == 1) && (web_int == 1) &&
	    (data_addra_int[14:1] == data_addrb_int[14:1]))
	    memory_collision <= 1;
    end

    always @(posedge setup_all_a_b or posedge setup_rf_a_b) begin
	if ((ena_reg == 1) && (wea_reg == 1) &&
	    (enb_int == 1) && (web_int == 1) &&
	    (data_addra_reg[14:1] == data_addrb_int[14:1]))
	    memory_collision_a_b <= 1;
    end

    always @(posedge setup_all_b_a or posedge setup_rf_b_a) begin
	if ((ena_int == 1) && (wea_int == 1) &&
	    (enb_reg == 1) && (web_reg == 1) &&
	    (data_addra_int[14:1] == data_addrb_reg[14:1]))
	    memory_collision_b_a <= 1;
    end

    always @(posedge setup_all_a_b) begin
	if (data_addra_reg[14:1] == data_addrb_int[14:1]) begin
	if ((ena_reg == 1) && (enb_int == 1)) begin
	    case ({wr_mode_a, wr_mode_b, wea_reg, web_int})
		6'b000011 : begin data_collision_a_b <= 2'b11; display_wa_wb; end
		6'b000111 : begin data_collision_a_b <= 2'b11; display_wa_wb; end
		6'b001011 : begin data_collision_a_b <= 2'b10; display_wa_wb; end
//		6'b010011 : begin data_collision_a_b <= 2'b00; display_wa_wb; end
//		6'b010111 : begin data_collision_a_b <= 2'b00; display_wa_wb; end
//		6'b011011 : begin data_collision_a_b <= 2'b00; display_wa_wb; end
		6'b100011 : begin data_collision_a_b <= 2'b01; display_wa_wb; end
		6'b100111 : begin data_collision_a_b <= 2'b01; display_wa_wb; end
		6'b101011 : begin display_wa_wb; end
		6'b000001 : begin data_collision_a_b <= 2'b10; display_ra_wb; end
//		6'b000101 : begin data_collision_a_b <= 2'b00; display_ra_wb; end
		6'b001001 : begin data_collision_a_b <= 2'b10; display_ra_wb; end
		6'b010001 : begin data_collision_a_b <= 2'b10; display_ra_wb; end
//		6'b010101 : begin data_collision_a_b <= 2'b00; display_ra_wb; end
		6'b011001 : begin data_collision_a_b <= 2'b10; display_ra_wb; end
		6'b100001 : begin data_collision_a_b <= 2'b10; display_ra_wb; end
//		6'b100101 : begin data_collision_a_b <= 2'b00; display_ra_wb; end
		6'b101001 : begin data_collision_a_b <= 2'b10; display_ra_wb; end
		6'b000010 : begin data_collision_a_b <= 2'b01; display_wa_rb; end
		6'b000110 : begin data_collision_a_b <= 2'b01; display_wa_rb; end
		6'b001010 : begin data_collision_a_b <= 2'b01; display_wa_rb; end
//		6'b010010 : begin data_collision_a_b <= 2'b00; display_wa_rb; end
//		6'b010110 : begin data_collision_a_b <= 2'b00; display_wa_rb; end
//		6'b011010 : begin data_collision_a_b <= 2'b00; display_wa_rb; end
		6'b100010 : begin data_collision_a_b <= 2'b01; display_wa_rb; end
		6'b100110 : begin data_collision_a_b <= 2'b01; display_wa_rb; end
		6'b101010 : begin data_collision_a_b <= 2'b01; display_wa_rb; end
	    endcase
	end
	end
	setup_all_a_b <= 0;
    end


    always @(posedge setup_all_b_a) begin
	if (data_addra_int[14:1] == data_addrb_reg[14:1]) begin
	if ((ena_int == 1) && (enb_reg == 1)) begin
	    case ({wr_mode_a, wr_mode_b, wea_int, web_reg})
		6'b000011 : begin data_collision_b_a <= 2'b11; display_wa_wb; end
//		6'b000111 : begin data_collision_b_a <= 2'b00; display_wa_wb; end
		6'b001011 : begin data_collision_b_a <= 2'b10; display_wa_wb; end
		6'b010011 : begin data_collision_b_a <= 2'b11; display_wa_wb; end
//		6'b010111 : begin data_collision_b_a <= 2'b00; display_wa_wb; end
		6'b011011 : begin data_collision_b_a <= 2'b10; display_wa_wb; end
		6'b100011 : begin data_collision_b_a <= 2'b01; display_wa_wb; end
		6'b100111 : begin data_collision_b_a <= 2'b01; display_wa_wb; end
		6'b101011 : begin display_wa_wb; end
		6'b000001 : begin data_collision_b_a <= 2'b10; display_ra_wb; end
		6'b000101 : begin data_collision_b_a <= 2'b10; display_ra_wb; end
		6'b001001 : begin data_collision_b_a <= 2'b10; display_ra_wb; end
		6'b010001 : begin data_collision_b_a <= 2'b10; display_ra_wb; end
		6'b010101 : begin data_collision_b_a <= 2'b10; display_ra_wb; end
		6'b011001 : begin data_collision_b_a <= 2'b10; display_ra_wb; end
		6'b100001 : begin data_collision_b_a <= 2'b10; display_ra_wb; end
		6'b100101 : begin data_collision_b_a <= 2'b10; display_ra_wb; end
		6'b101001 : begin data_collision_b_a <= 2'b10; display_ra_wb; end
		6'b000010 : begin data_collision_b_a <= 2'b01; display_wa_rb; end
		6'b000110 : begin data_collision_b_a <= 2'b01; display_wa_rb; end
		6'b001010 : begin data_collision_b_a <= 2'b01; display_wa_rb; end
//		6'b010010 : begin data_collision_b_a <= 2'b00; display_wa_rb; end
//		6'b010110 : begin data_collision_b_a <= 2'b00; display_wa_rb; end
//		6'b011010 : begin data_collision_b_a <= 2'b00; display_wa_rb; end
		6'b100010 : begin data_collision_b_a <= 2'b01; display_wa_rb; end
		6'b100110 : begin data_collision_b_a <= 2'b01; display_wa_rb; end
		6'b101010 : begin data_collision_b_a <= 2'b01; display_wa_rb; end
	    endcase
	end
	end
	setup_all_b_a <= 0;
    end


    always @(posedge setup_zero) begin
	if (data_addra_int[14:1] == data_addrb_int[14:1]) begin
	if ((ena_int == 1) && (enb_int == 1)) begin
	    case ({wr_mode_a, wr_mode_b, wea_int, web_int})
		6'b000011 : begin data_collision <= 2'b11; display_wa_wb; end
		6'b000111 : begin data_collision <= 2'b11; display_wa_wb; end
		6'b001011 : begin data_collision <= 2'b10; display_wa_wb; end
		6'b010011 : begin data_collision <= 2'b11; display_wa_wb; end
		6'b010111 : begin data_collision <= 2'b11; display_wa_wb; end
		6'b011011 : begin data_collision <= 2'b10; display_wa_wb; end
		6'b100011 : begin data_collision <= 2'b01; display_wa_wb; end
		6'b100111 : begin data_collision <= 2'b01; display_wa_wb; end
		6'b101011 : begin display_wa_wb; end
		6'b000001 : begin data_collision <= 2'b10; display_ra_wb; end
//		6'b000101 : begin data_collision <= 2'b00; display_ra_wb; end
		6'b001001 : begin data_collision <= 2'b10; display_ra_wb; end
		6'b010001 : begin data_collision <= 2'b10; display_ra_wb; end
//		6'b010101 : begin data_collision <= 2'b00; display_ra_wb; end
		6'b011001 : begin data_collision <= 2'b10; display_ra_wb; end
		6'b100001 : begin data_collision <= 2'b10; display_ra_wb; end
//		6'b100101 : begin data_collision <= 2'b00; display_ra_wb; end
		6'b101001 : begin data_collision <= 2'b10; display_ra_wb; end
		6'b000010 : begin data_collision <= 2'b01; display_wa_rb; end
		6'b000110 : begin data_collision <= 2'b01; display_wa_rb; end
		6'b001010 : begin data_collision <= 2'b01; display_wa_rb; end
//		6'b010010 : begin data_collision <= 2'b00; display_wa_rb; end
//		6'b010110 : begin data_collision <= 2'b00; display_wa_rb; end
//		6'b011010 : begin data_collision <= 2'b00; display_wa_rb; end
		6'b100010 : begin data_collision <= 2'b01; display_wa_rb; end
		6'b100110 : begin data_collision <= 2'b01; display_wa_rb; end
		6'b101010 : begin data_collision <= 2'b01; display_wa_rb; end
	    endcase
	end
	end
	setup_zero <= 0;
    end

    task display_ra_wb;
    begin
	if (display_flag)
        $display("Memory Collision Error on RAMB16_S1_S2:%m at simulation time %.3f ns\nA read was performed on address %h (hex) of Port A while a write was requested to the same address on Port B. The write will be successful however the read value on Port A is unknown until the next CLKA cycle.", $time/1000.0, addra_int);
    end
    endtask

    task display_wa_rb;
    begin
	if (display_flag)
        $display("Memory Collision Error on RAMB16_S1_S2:%m at simulation time %.3f ns\nA read was performed on address %h (hex) of Port B while a write was requested to the same address on Port A. The write will be successful however the read value on Port B is unknown until the next CLKB cycle.", $time/1000.0, addrb_int);
    end
    endtask

    task display_wa_wb;
    begin
	if (display_flag)
        $display("Memory Collision Error on RAMB16_S1_S2:%m at simulation time %.3f ns\nA write was requested to the same address simultaneously at both Port A and Port B of the RAM. The contents written to the RAM at address location %h (hex) of Port A and address location %h (hex) of Port B are unknown.", $time/1000.0, addra_int, addrb_int);
    end
    endtask


    always @(posedge setup_rf_a_b) begin
	if (data_addra_reg[14:1] == data_addrb_int[14:1]) begin
	if ((ena_reg == 1) && (enb_int == 1)) begin
	    case ({wr_mode_a, wr_mode_b, wea_reg, web_int})
//		6'b000011 : begin data_collision_a_b <= 2'b00; display_wa_wb; end
//		6'b000111 : begin data_collision_a_b <= 2'b00; display_wa_wb; end
//		6'b001011 : begin data_collision_a_b <= 2'b00; display_wa_wb; end
		6'b010011 : begin data_collision_a_b <= 2'b11; display_wa_wb; end
		6'b010111 : begin data_collision_a_b <= 2'b11; display_wa_wb; end
		6'b011011 : begin data_collision_a_b <= 2'b10; display_wa_wb; end
//		6'b100011 : begin data_collision_a_b <= 2'b00; display_wa_wb; end
//		6'b100111 : begin data_collision_a_b <= 2'b00; display_wa_wb; end
//		6'b101011 : begin data_collision_a_b <= 2'b00; display_wa_wb; end
//		6'b000001 : begin data_collision_a_b <= 2'b00; display_ra_wb; end
//		6'b000101 : begin data_collision_a_b <= 2'b00; display_ra_wb; end
//		6'b001001 : begin data_collision_a_b <= 2'b00; display_ra_wb; end
//		6'b010001 : begin data_collision_a_b <= 2'b00; display_ra_wb; end
//		6'b010101 : begin data_collision_a_b <= 2'b00; display_ra_wb; end
//		6'b011001 : begin data_collision_a_b <= 2'b00; display_ra_wb; end
//		6'b100001 : begin data_collision_a_b <= 2'b00; display_ra_wb; end
//		6'b100101 : begin data_collision_a_b <= 2'b00; display_ra_wb; end
//		6'b101001 : begin data_collision_a_b <= 2'b00; display_ra_wb; end
//		6'b000010 : begin data_collision_a_b <= 2'b00; display_wa_rb; end
//		6'b000110 : begin data_collision_a_b <= 2'b00; display_wa_rb; end
//		6'b001010 : begin data_collision_a_b <= 2'b00; display_wa_rb; end
		6'b010010 : begin data_collision_a_b <= 2'b01; display_wa_rb; end
		6'b010110 : begin data_collision_a_b <= 2'b01; display_wa_rb; end
		6'b011010 : begin data_collision_a_b <= 2'b01; display_wa_rb; end
//		6'b100010 : begin data_collision_a_b <= 2'b00; display_wa_rb; end
//		6'b100110 : begin data_collision_a_b <= 2'b00; display_wa_rb; end
//		6'b101010 : begin data_collision_a_b <= 2'b00; display_wa_rb; end
	    endcase
	end
	end
	setup_rf_a_b <= 0;
    end


    always @(posedge setup_rf_b_a) begin
	if (data_addra_int[14:1] == data_addrb_reg[14:1]) begin
	if ((ena_int == 1) && (enb_reg == 1)) begin
	    case ({wr_mode_a, wr_mode_b, wea_int, web_reg})
//		6'b000011 : begin data_collision_b_a <= 2'b00; display_wa_wb; end
		6'b000111 : begin data_collision_b_a <= 2'b11; display_wa_wb; end
//		6'b001011 : begin data_collision_b_a <= 2'b00; display_wa_wb; end
//		6'b010011 : begin data_collision_b_a <= 2'b00; display_wa_wb; end
		6'b010111 : begin data_collision_b_a <= 2'b11; display_wa_wb; end
//		6'b011011 : begin data_collision_b_a <= 2'b00; display_wa_wb; end
//		6'b100011 : begin data_collision_b_a <= 2'b00; display_wa_wb; end
		6'b100111 : begin data_collision_b_a <= 2'b01; display_wa_wb; end
//		6'b101011 : begin data_collision_b_a <= 2'b00; display_wa_wb; end
//		6'b000001 : begin data_collision_b_a <= 2'b00; display_ra_wb; end
		6'b000101 : begin data_collision_b_a <= 2'b10; display_ra_wb; end
//		6'b001001 : begin data_collision_b_a <= 2'b00; display_ra_wb; end
//		6'b010001 : begin data_collision_b_a <= 2'b00; display_ra_wb; end
		6'b010101 : begin data_collision_b_a <= 2'b10; display_ra_wb; end
//		6'b011001 : begin data_collision_b_a <= 2'b00; display_ra_wb; end
//		6'b100001 : begin data_collision_b_a <= 2'b00; display_ra_wb; end
		6'b100101 : begin data_collision_b_a <= 2'b10; display_ra_wb; end
//		6'b101001 : begin data_collision_b_a <= 2'b00; display_ra_wb; end
//		6'b000010 : begin data_collision_b_a <= 2'b00; display_wa_rb; end
//		6'b000110 : begin data_collision_b_a <= 2'b00; display_wa_rb; end
//		6'b001010 : begin data_collision_b_a <= 2'b00; display_wa_rb; end
//		6'b010010 : begin data_collision_b_a <= 2'b00; display_wa_rb; end
//		6'b010110 : begin data_collision_b_a <= 2'b00; display_wa_rb; end
//		6'b011010 : begin data_collision_b_a <= 2'b00; display_wa_rb; end
//		6'b100010 : begin data_collision_b_a <= 2'b00; display_wa_rb; end
//		6'b100110 : begin data_collision_b_a <= 2'b00; display_wa_rb; end
//		6'b101010 : begin data_collision_b_a <= 2'b00; display_wa_rb; end
	    endcase
	end
	end
	setup_rf_b_a <= 0;
    end


    always @(posedge clka_int) begin
	if ((output_flag || display_flag)) begin
	    addra_reg <= addra_int;
	    ena_reg <= ena_int;
	    ssra_reg <= ssra_int;
	    wea_reg <= wea_int;
	end
    end
    
    always @(posedge clkb_int) begin
	if ((output_flag || display_flag)) begin
	    addrb_reg <= addrb_int;
	    enb_reg <= enb_int;
	    ssrb_reg <= ssrb_int;
	    web_reg <= web_int;
	end
    end
    
	    
    // Data
    always @(posedge memory_collision) begin
	if ((output_flag || display_flag)) begin
	    mem[addra_int[13:1]][addra_int[0:0] * 1 +: 1] <= 1'bx;
	    memory_collision <= 0;
	end
	
    end

    always @(posedge memory_collision_a_b) begin
	if ((output_flag || display_flag)) begin
	    mem[addra_reg[13:1]][addra_reg[0:0] * 1 +: 1] <= 1'bx;
	    memory_collision_a_b <= 0;
	end
    end
    
    always @(posedge memory_collision_b_a) begin
	if ((output_flag || display_flag)) begin
	    mem[addra_int[13:1]][addra_int[0:0] * 1 +: 1] <= 1'bx;
	    memory_collision_b_a <= 0;
	end
    end
    
    always @(posedge data_collision[1]) begin
	if (ssra_int == 0 && output_flag) begin
	    doa_out <= #100 1'bX;
	end
	data_collision[1] <= 0;
    end

    always @(posedge data_collision[0]) begin
	if (ssrb_int == 0 && output_flag) begin
	    dob_out[addra_int[0:0] * 1 +: 1] <= #100 1'bX;
	end
	data_collision[0] <= 0;
    end

    always @(posedge data_collision_a_b[1]) begin
	if (ssra_reg == 0 && output_flag) begin
	    doa_out <= #100 1'bX;
	end
	data_collision_a_b[1] <= 0;
    end

    always @(posedge data_collision_a_b[0]) begin
	if (ssrb_int == 0 && output_flag) begin
	    dob_out[addra_reg[0:0] * 1 +: 1] <= #100 1'bX;
	end
	data_collision_a_b[0] <= 0;
    end

    always @(posedge data_collision_b_a[1]) begin
	if (ssra_int == 0 && output_flag) begin
	    doa_out <= #100 1'bX;
	end
	data_collision_b_a[1] <= 0;
    end

    always @(posedge data_collision_b_a[0]) begin
	if (ssrb_reg == 0 && output_flag) begin
	    dob_out[addra_int[0:0] * 1 +: 1] <= #100 1'bX;
	end
	data_collision_b_a[0] <= 0;
    end


    initial begin
	case (WRITE_MODE_A)
	    "WRITE_FIRST" : wr_mode_a <= 2'b00;
	    "READ_FIRST"  : wr_mode_a <= 2'b01;
	    "NO_CHANGE"   : wr_mode_a <= 2'b10;
	    default       : begin
				$display("Attribute Syntax Error : The Attribute WRITE_MODE_A on RAMB16_S1_S2 instance %m is set to %s.  Legal values for this attribute are WRITE_FIRST, READ_FIRST or NO_CHANGE.", WRITE_MODE_A);
				$finish;
			    end
	endcase
    end

    initial begin
	case (WRITE_MODE_B)
	    "WRITE_FIRST" : wr_mode_b <= 2'b00;
	    "READ_FIRST"  : wr_mode_b <= 2'b01;
	    "NO_CHANGE"   : wr_mode_b <= 2'b10;
	    default       : begin
				$display("Attribute Syntax Error : The Attribute WRITE_MODE_B on RAMB16_S1_S2 instance %m is set to %s.  Legal values for this attribute are WRITE_FIRST, READ_FIRST or NO_CHANGE.", WRITE_MODE_B);
				$finish;
			    end
	endcase
    end


    // Port A
    always @(posedge clka_int) begin

	if (ena_int == 1'b1) begin

	    if (ssra_int == 1'b1) begin
		{doa_out} <= #100 SRVAL_A;
	    end
	    else begin
		if (wea_int == 1'b1) begin
		    if (wr_mode_a == 2'b00) begin
			doa_out <= #100 dia_int;
		    end
		    else if (wr_mode_a == 2'b01) begin

			doa_out <= #100 mem[addra_int[13:1]][addra_int[0:0] * 1 +: 1];

		    end
		end
		else begin

		    doa_out <= #100 mem[addra_int[13:1]][addra_int[0:0] * 1 +: 1];
		    
		end
	    end

	    // memory
	    if (wea_int == 1'b1) begin
		mem[addra_int[13:1]][addra_int[0:0] * 1 +: 1] <= dia_int;
	    end
	    
	end
    end


    // Port B
    always @(posedge clkb_int) begin

	if (enb_int == 1'b1) begin

	    if (ssrb_int == 1'b1) begin
		{dob_out} <= #100 SRVAL_B;
	    end
	    else begin
		if (web_int == 1'b1) begin
		    if (wr_mode_b == 2'b00) begin
			dob_out <= #100 dib_int;
		    end
		    else if (wr_mode_b == 2'b01) begin
			dob_out <= #100 mem[addrb_int];
		    end
		end
		else begin
		    dob_out <= #100 mem[addrb_int];
		end
	    end

	    // memory
	    if (web_int == 1'b1) begin
		mem[addrb_int] <= dib_int;
	    end

	end
    end


endmodule

`endif
