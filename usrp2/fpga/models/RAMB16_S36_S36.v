// $Header: /devl/xcs/repo/env/Databases/CAEInterfaces/verunilibs/data/unisims/RAMB16_S36_S36.v,v 1.10 2007/02/22 01:58:06 wloo Exp $
///////////////////////////////////////////////////////////////////////////////
// Copyright (c) 1995/2005 Xilinx, Inc.
// All Right Reserved.
///////////////////////////////////////////////////////////////////////////////
//   ____  ____
//  /   /\/   /
// /___/  \  /    Vendor : Xilinx
// \   \   \/     Version : 10.1
//  \   \         Description : Xilinx Functional Simulation Library Component
//  /   /                  16K-Bit Data and 2K-Bit Parity Dual Port Block RAM
// /___/   /\     Filename : RAMB16_S36_S36.v
// \   \  /  \    Timestamp : Thu Mar 10 16:43:36 PST 2005
//  \___\/\___\
//
// Revision:
//    03/23/04 - Initial version.
// End Revision

`ifdef legacy_model

`timescale  1 ps / 1 ps

module RAMB16_S36_S36 (DOA, DOB, DOPA, DOPB, ADDRA, ADDRB, CLKA, CLKB, DIA, DIB, DIPA, DIPB, ENA, ENB, SSRA, SSRB, WEA, WEB);

    parameter INIT_A = 36'h0;
    parameter INIT_B = 36'h0;
    parameter SRVAL_A = 36'h0;
    parameter SRVAL_B = 36'h0;
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
    parameter INITP_00 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INITP_01 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INITP_02 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INITP_03 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INITP_04 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INITP_05 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INITP_06 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INITP_07 = 256'h0000000000000000000000000000000000000000000000000000000000000000;

    output [31:0] DOA;
    output [3:0] DOPA;
    reg [31:0] doa_out;
    reg [3:0] dopa_out;
    wire doa_out0, doa_out1, doa_out2, doa_out3, doa_out4, doa_out5, doa_out6, doa_out7, doa_out8, doa_out9, doa_out10, doa_out11, doa_out12, doa_out13, doa_out14, doa_out15, doa_out16, doa_out17, doa_out18, doa_out19, doa_out20, doa_out21, doa_out22, doa_out23, doa_out24, doa_out25, doa_out26, doa_out27, doa_out28, doa_out29, doa_out30, doa_out31;
    wire dopa0_out, dopa1_out, dopa2_out, dopa3_out;

    input [8:0] ADDRA;
    input [31:0] DIA;
    input [3:0] DIPA;
    input ENA, CLKA, WEA, SSRA;

    output [31:0] DOB;
    output [3:0] DOPB;
    reg [31:0] dob_out;
    reg [3:0] dopb_out;
    wire dob_out0, dob_out1, dob_out2, dob_out3, dob_out4, dob_out5, dob_out6, dob_out7, dob_out8, dob_out9, dob_out10, dob_out11, dob_out12, dob_out13, dob_out14, dob_out15, dob_out16, dob_out17, dob_out18, dob_out19, dob_out20, dob_out21, dob_out22, dob_out23, dob_out24, dob_out25, dob_out26, dob_out27, dob_out28, dob_out29, dob_out30, dob_out31;
    wire dopb0_out, dopb1_out, dopb2_out, dopb3_out;

    input [8:0] ADDRB;
    input [31:0] DIB;
    input [3:0] DIPB;
    input ENB, CLKB, WEB, SSRB;

    reg [18431:0] mem;
    reg [8:0] count;
    reg [1:0] wr_mode_a, wr_mode_b;

    reg [5:0] dmi, dbi;
    reg [5:0] pmi, pbi;

    wire [8:0] addra_int;
    reg [8:0] addra_reg;
    wire [31:0] dia_int;
    wire [3:0] dipa_int;
    wire ena_int, clka_int, wea_int, ssra_int;
    reg ena_reg, wea_reg, ssra_reg;
    wire [8:0] addrb_int;
    reg [8:0] addrb_reg;
    wire [31:0] dib_int;
    wire [3:0] dipb_int;
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
	    assign doa_out = INIT_A[31:0];
	    assign dopa_out = INIT_A[35:32];
	    assign dob_out = INIT_B[31:0];
	    assign dopb_out = INIT_B[35:32];
	end
	else begin
	    deassign doa_out;
	    deassign dopa_out;
	    deassign dob_out;
	    deassign dopb_out;
	end

    buf b_doa_out0 (doa_out0, doa_out[0]);
    buf b_doa_out1 (doa_out1, doa_out[1]);
    buf b_doa_out2 (doa_out2, doa_out[2]);
    buf b_doa_out3 (doa_out3, doa_out[3]);
    buf b_doa_out4 (doa_out4, doa_out[4]);
    buf b_doa_out5 (doa_out5, doa_out[5]);
    buf b_doa_out6 (doa_out6, doa_out[6]);
    buf b_doa_out7 (doa_out7, doa_out[7]);
    buf b_doa_out8 (doa_out8, doa_out[8]);
    buf b_doa_out9 (doa_out9, doa_out[9]);
    buf b_doa_out10 (doa_out10, doa_out[10]);
    buf b_doa_out11 (doa_out11, doa_out[11]);
    buf b_doa_out12 (doa_out12, doa_out[12]);
    buf b_doa_out13 (doa_out13, doa_out[13]);
    buf b_doa_out14 (doa_out14, doa_out[14]);
    buf b_doa_out15 (doa_out15, doa_out[15]);
    buf b_doa_out16 (doa_out16, doa_out[16]);
    buf b_doa_out17 (doa_out17, doa_out[17]);
    buf b_doa_out18 (doa_out18, doa_out[18]);
    buf b_doa_out19 (doa_out19, doa_out[19]);
    buf b_doa_out20 (doa_out20, doa_out[20]);
    buf b_doa_out21 (doa_out21, doa_out[21]);
    buf b_doa_out22 (doa_out22, doa_out[22]);
    buf b_doa_out23 (doa_out23, doa_out[23]);
    buf b_doa_out24 (doa_out24, doa_out[24]);
    buf b_doa_out25 (doa_out25, doa_out[25]);
    buf b_doa_out26 (doa_out26, doa_out[26]);
    buf b_doa_out27 (doa_out27, doa_out[27]);
    buf b_doa_out28 (doa_out28, doa_out[28]);
    buf b_doa_out29 (doa_out29, doa_out[29]);
    buf b_doa_out30 (doa_out30, doa_out[30]);
    buf b_doa_out31 (doa_out31, doa_out[31]);
    buf b_dopa_out0 (dopa_out0, dopa_out[0]);
    buf b_dopa_out1 (dopa_out1, dopa_out[1]);
    buf b_dopa_out2 (dopa_out2, dopa_out[2]);
    buf b_dopa_out3 (dopa_out3, dopa_out[3]);
    buf b_dob_out0 (dob_out0, dob_out[0]);
    buf b_dob_out1 (dob_out1, dob_out[1]);
    buf b_dob_out2 (dob_out2, dob_out[2]);
    buf b_dob_out3 (dob_out3, dob_out[3]);
    buf b_dob_out4 (dob_out4, dob_out[4]);
    buf b_dob_out5 (dob_out5, dob_out[5]);
    buf b_dob_out6 (dob_out6, dob_out[6]);
    buf b_dob_out7 (dob_out7, dob_out[7]);
    buf b_dob_out8 (dob_out8, dob_out[8]);
    buf b_dob_out9 (dob_out9, dob_out[9]);
    buf b_dob_out10 (dob_out10, dob_out[10]);
    buf b_dob_out11 (dob_out11, dob_out[11]);
    buf b_dob_out12 (dob_out12, dob_out[12]);
    buf b_dob_out13 (dob_out13, dob_out[13]);
    buf b_dob_out14 (dob_out14, dob_out[14]);
    buf b_dob_out15 (dob_out15, dob_out[15]);
    buf b_dob_out16 (dob_out16, dob_out[16]);
    buf b_dob_out17 (dob_out17, dob_out[17]);
    buf b_dob_out18 (dob_out18, dob_out[18]);
    buf b_dob_out19 (dob_out19, dob_out[19]);
    buf b_dob_out20 (dob_out20, dob_out[20]);
    buf b_dob_out21 (dob_out21, dob_out[21]);
    buf b_dob_out22 (dob_out22, dob_out[22]);
    buf b_dob_out23 (dob_out23, dob_out[23]);
    buf b_dob_out24 (dob_out24, dob_out[24]);
    buf b_dob_out25 (dob_out25, dob_out[25]);
    buf b_dob_out26 (dob_out26, dob_out[26]);
    buf b_dob_out27 (dob_out27, dob_out[27]);
    buf b_dob_out28 (dob_out28, dob_out[28]);
    buf b_dob_out29 (dob_out29, dob_out[29]);
    buf b_dob_out30 (dob_out30, dob_out[30]);
    buf b_dob_out31 (dob_out31, dob_out[31]);
    buf b_dopb_out0 (dopb_out0, dopb_out[0]);
    buf b_dopb_out1 (dopb_out1, dopb_out[1]);
    buf b_dopb_out2 (dopb_out2, dopb_out[2]);
    buf b_dopb_out3 (dopb_out3, dopb_out[3]);

    buf b_doa0 (DOA[0], doa_out0);
    buf b_doa1 (DOA[1], doa_out1);
    buf b_doa2 (DOA[2], doa_out2);
    buf b_doa3 (DOA[3], doa_out3);
    buf b_doa4 (DOA[4], doa_out4);
    buf b_doa5 (DOA[5], doa_out5);
    buf b_doa6 (DOA[6], doa_out6);
    buf b_doa7 (DOA[7], doa_out7);
    buf b_doa8 (DOA[8], doa_out8);
    buf b_doa9 (DOA[9], doa_out9);
    buf b_doa10 (DOA[10], doa_out10);
    buf b_doa11 (DOA[11], doa_out11);
    buf b_doa12 (DOA[12], doa_out12);
    buf b_doa13 (DOA[13], doa_out13);
    buf b_doa14 (DOA[14], doa_out14);
    buf b_doa15 (DOA[15], doa_out15);
    buf b_doa16 (DOA[16], doa_out16);
    buf b_doa17 (DOA[17], doa_out17);
    buf b_doa18 (DOA[18], doa_out18);
    buf b_doa19 (DOA[19], doa_out19);
    buf b_doa20 (DOA[20], doa_out20);
    buf b_doa21 (DOA[21], doa_out21);
    buf b_doa22 (DOA[22], doa_out22);
    buf b_doa23 (DOA[23], doa_out23);
    buf b_doa24 (DOA[24], doa_out24);
    buf b_doa25 (DOA[25], doa_out25);
    buf b_doa26 (DOA[26], doa_out26);
    buf b_doa27 (DOA[27], doa_out27);
    buf b_doa28 (DOA[28], doa_out28);
    buf b_doa29 (DOA[29], doa_out29);
    buf b_doa30 (DOA[30], doa_out30);
    buf b_doa31 (DOA[31], doa_out31);
    buf b_dopa0 (DOPA[0], dopa_out0);
    buf b_dopa1 (DOPA[1], dopa_out1);
    buf b_dopa2 (DOPA[2], dopa_out2);
    buf b_dopa3 (DOPA[3], dopa_out3);
    buf b_dob0 (DOB[0], dob_out0);
    buf b_dob1 (DOB[1], dob_out1);
    buf b_dob2 (DOB[2], dob_out2);
    buf b_dob3 (DOB[3], dob_out3);
    buf b_dob4 (DOB[4], dob_out4);
    buf b_dob5 (DOB[5], dob_out5);
    buf b_dob6 (DOB[6], dob_out6);
    buf b_dob7 (DOB[7], dob_out7);
    buf b_dob8 (DOB[8], dob_out8);
    buf b_dob9 (DOB[9], dob_out9);
    buf b_dob10 (DOB[10], dob_out10);
    buf b_dob11 (DOB[11], dob_out11);
    buf b_dob12 (DOB[12], dob_out12);
    buf b_dob13 (DOB[13], dob_out13);
    buf b_dob14 (DOB[14], dob_out14);
    buf b_dob15 (DOB[15], dob_out15);
    buf b_dob16 (DOB[16], dob_out16);
    buf b_dob17 (DOB[17], dob_out17);
    buf b_dob18 (DOB[18], dob_out18);
    buf b_dob19 (DOB[19], dob_out19);
    buf b_dob20 (DOB[20], dob_out20);
    buf b_dob21 (DOB[21], dob_out21);
    buf b_dob22 (DOB[22], dob_out22);
    buf b_dob23 (DOB[23], dob_out23);
    buf b_dob24 (DOB[24], dob_out24);
    buf b_dob25 (DOB[25], dob_out25);
    buf b_dob26 (DOB[26], dob_out26);
    buf b_dob27 (DOB[27], dob_out27);
    buf b_dob28 (DOB[28], dob_out28);
    buf b_dob29 (DOB[29], dob_out29);
    buf b_dob30 (DOB[30], dob_out30);
    buf b_dob31 (DOB[31], dob_out31);
    buf b_dopb0 (DOPB[0], dopb_out0);
    buf b_dopb1 (DOPB[1], dopb_out1);
    buf b_dopb2 (DOPB[2], dopb_out2);
    buf b_dopb3 (DOPB[3], dopb_out3);

    buf b_addra_0 (addra_int[0], ADDRA[0]);
    buf b_addra_1 (addra_int[1], ADDRA[1]);
    buf b_addra_2 (addra_int[2], ADDRA[2]);
    buf b_addra_3 (addra_int[3], ADDRA[3]);
    buf b_addra_4 (addra_int[4], ADDRA[4]);
    buf b_addra_5 (addra_int[5], ADDRA[5]);
    buf b_addra_6 (addra_int[6], ADDRA[6]);
    buf b_addra_7 (addra_int[7], ADDRA[7]);
    buf b_addra_8 (addra_int[8], ADDRA[8]);
    buf b_dia_0 (dia_int[0], DIA[0]);
    buf b_dia_1 (dia_int[1], DIA[1]);
    buf b_dia_2 (dia_int[2], DIA[2]);
    buf b_dia_3 (dia_int[3], DIA[3]);
    buf b_dia_4 (dia_int[4], DIA[4]);
    buf b_dia_5 (dia_int[5], DIA[5]);
    buf b_dia_6 (dia_int[6], DIA[6]);
    buf b_dia_7 (dia_int[7], DIA[7]);
    buf b_dia_8 (dia_int[8], DIA[8]);
    buf b_dia_9 (dia_int[9], DIA[9]);
    buf b_dia_10 (dia_int[10], DIA[10]);
    buf b_dia_11 (dia_int[11], DIA[11]);
    buf b_dia_12 (dia_int[12], DIA[12]);
    buf b_dia_13 (dia_int[13], DIA[13]);
    buf b_dia_14 (dia_int[14], DIA[14]);
    buf b_dia_15 (dia_int[15], DIA[15]);
    buf b_dia_16 (dia_int[16], DIA[16]);
    buf b_dia_17 (dia_int[17], DIA[17]);
    buf b_dia_18 (dia_int[18], DIA[18]);
    buf b_dia_19 (dia_int[19], DIA[19]);
    buf b_dia_20 (dia_int[20], DIA[20]);
    buf b_dia_21 (dia_int[21], DIA[21]);
    buf b_dia_22 (dia_int[22], DIA[22]);
    buf b_dia_23 (dia_int[23], DIA[23]);
    buf b_dia_24 (dia_int[24], DIA[24]);
    buf b_dia_25 (dia_int[25], DIA[25]);
    buf b_dia_26 (dia_int[26], DIA[26]);
    buf b_dia_27 (dia_int[27], DIA[27]);
    buf b_dia_28 (dia_int[28], DIA[28]);
    buf b_dia_29 (dia_int[29], DIA[29]);
    buf b_dia_30 (dia_int[30], DIA[30]);
    buf b_dia_31 (dia_int[31], DIA[31]);
    buf b_dipa_0 (dipa_int[0], DIPA[0]);
    buf b_dipa_1 (dipa_int[1], DIPA[1]);
    buf b_dipa_2 (dipa_int[2], DIPA[2]);
    buf b_dipa_3 (dipa_int[3], DIPA[3]);
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
    buf b_dib_0 (dib_int[0], DIB[0]);
    buf b_dib_1 (dib_int[1], DIB[1]);
    buf b_dib_2 (dib_int[2], DIB[2]);
    buf b_dib_3 (dib_int[3], DIB[3]);
    buf b_dib_4 (dib_int[4], DIB[4]);
    buf b_dib_5 (dib_int[5], DIB[5]);
    buf b_dib_6 (dib_int[6], DIB[6]);
    buf b_dib_7 (dib_int[7], DIB[7]);
    buf b_dib_8 (dib_int[8], DIB[8]);
    buf b_dib_9 (dib_int[9], DIB[9]);
    buf b_dib_10 (dib_int[10], DIB[10]);
    buf b_dib_11 (dib_int[11], DIB[11]);
    buf b_dib_12 (dib_int[12], DIB[12]);
    buf b_dib_13 (dib_int[13], DIB[13]);
    buf b_dib_14 (dib_int[14], DIB[14]);
    buf b_dib_15 (dib_int[15], DIB[15]);
    buf b_dib_16 (dib_int[16], DIB[16]);
    buf b_dib_17 (dib_int[17], DIB[17]);
    buf b_dib_18 (dib_int[18], DIB[18]);
    buf b_dib_19 (dib_int[19], DIB[19]);
    buf b_dib_20 (dib_int[20], DIB[20]);
    buf b_dib_21 (dib_int[21], DIB[21]);
    buf b_dib_22 (dib_int[22], DIB[22]);
    buf b_dib_23 (dib_int[23], DIB[23]);
    buf b_dib_24 (dib_int[24], DIB[24]);
    buf b_dib_25 (dib_int[25], DIB[25]);
    buf b_dib_26 (dib_int[26], DIB[26]);
    buf b_dib_27 (dib_int[27], DIB[27]);
    buf b_dib_28 (dib_int[28], DIB[28]);
    buf b_dib_29 (dib_int[29], DIB[29]);
    buf b_dib_30 (dib_int[30], DIB[30]);
    buf b_dib_31 (dib_int[31], DIB[31]);
    buf b_dipb_0 (dipb_int[0], DIPB[0]);
    buf b_dipb_1 (dipb_int[1], DIPB[1]);
    buf b_dipb_2 (dipb_int[2], DIPB[2]);
    buf b_dipb_3 (dipb_int[3], DIPB[3]);
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
	    mem[256 * 64 + count] <= INITP_00[count];
	    mem[256 * 65 + count] <= INITP_01[count];
	    mem[256 * 66 + count] <= INITP_02[count];
	    mem[256 * 67 + count] <= INITP_03[count];
	    mem[256 * 68 + count] <= INITP_04[count];
	    mem[256 * 69 + count] <= INITP_05[count];
	    mem[256 * 70 + count] <= INITP_06[count];
	    mem[256 * 71 + count] <= INITP_07[count];
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

    assign data_addra_int = addra_int * 32;
    assign data_addra_reg = addra_reg * 32;
    assign data_addrb_int = addrb_int * 32;
    assign data_addrb_reg = addrb_reg * 32;
    assign parity_addra_int = 16384 + addra_int * 4;
    assign parity_addra_reg = 16384 + addra_reg * 4;
    assign parity_addrb_int = 16384 + addrb_int * 4;
    assign parity_addrb_reg = 16384 + addrb_reg * 4;


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
		          $display("Attribute Syntax Error : The Attribute SIM_COLLISION_CHECK on RAMB16_S36_S36 instance %m is set to %s.  Legal values for this attribute are ALL, NONE, WARNING_ONLY or GENERATE_X_ONLY.", SIM_COLLISION_CHECK);
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
	    (data_addra_int[14:5] == data_addrb_int[14:5]))
	    memory_collision <= 1;
    end

    always @(posedge setup_all_a_b or posedge setup_rf_a_b) begin
	if ((ena_reg == 1) && (wea_reg == 1) &&
	    (enb_int == 1) && (web_int == 1) &&
	    (data_addra_reg[14:5] == data_addrb_int[14:5]))
	    memory_collision_a_b <= 1;
    end

    always @(posedge setup_all_b_a or posedge setup_rf_b_a) begin
	if ((ena_int == 1) && (wea_int == 1) &&
	    (enb_reg == 1) && (web_reg == 1) &&
	    (data_addra_int[14:5] == data_addrb_reg[14:5]))
	    memory_collision_b_a <= 1;
    end

    always @(posedge setup_all_a_b) begin
	if (data_addra_reg[14:5] == data_addrb_int[14:5]) begin
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
	if (data_addra_int[14:5] == data_addrb_reg[14:5]) begin
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
	if (data_addra_int[14:5] == data_addrb_int[14:5]) begin
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
        $display("Memory Collision Error on RAMB16_S36_S36:%m at simulation time %.3f ns\nA read was performed on address %h (hex) of Port A while a write was requested to the same address on Port B. The write will be successful however the read value on Port A is unknown until the next CLKA cycle.", $time/1000.0, addra_int);
    end
    endtask

    task display_wa_rb;
    begin
	if (display_flag)
        $display("Memory Collision Error on RAMB16_S36_S36:%m at simulation time %.3f ns\nA read was performed on address %h (hex) of Port B while a write was requested to the same address on Port A. The write will be successful however the read value on Port B is unknown until the next CLKB cycle.", $time/1000.0, addrb_int);
    end
    endtask

    task display_wa_wb;
    begin
	if (display_flag)
        $display("Memory Collision Error on RAMB16_S36_S36:%m at simulation time %.3f ns\nA write was requested to the same address simultaneously at both Port A and Port B of the RAM. The contents written to the RAM at address location %h (hex) of Port A and address location %h (hex) of Port B are unknown.", $time/1000.0, addra_int, addrb_int);
    end
    endtask


    always @(posedge setup_rf_a_b) begin
	if (data_addra_reg[14:5] == data_addrb_int[14:5]) begin
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
	if (data_addra_int[14:5] == data_addrb_reg[14:5]) begin
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
	for (dmi = 0; dmi < 32; dmi = dmi + 1) begin
	    mem[data_addra_int + dmi] <= 1'bX;
	end
	memory_collision <= 0;
    end

    always @(posedge memory_collision_a_b) begin
	for (dmi = 0; dmi < 32; dmi = dmi + 1) begin
	    mem[data_addra_reg + dmi] <= 1'bX;
	end
	memory_collision_a_b <= 0;
    end

    always @(posedge memory_collision_b_a) begin
	for (dmi = 0; dmi < 32; dmi = dmi + 1) begin
	    mem[data_addra_int + dmi] <= 1'bX;
	end
	memory_collision_b_a <= 0;
    end

    always @(posedge data_collision[1]) begin
	if (ssra_int == 0) begin
	    doa_out <= 32'bX;
	end
	data_collision[1] <= 0;
    end

    always @(posedge data_collision[0]) begin
	if (ssrb_int == 0) begin
	    dob_out <= 32'bX;
	end
	data_collision[0] <= 0;
    end

    always @(posedge data_collision_a_b[1]) begin
	if (ssra_reg == 0) begin
	    doa_out <= 32'bX;
	end
	data_collision_a_b[1] <= 0;
    end

    always @(posedge data_collision_a_b[0]) begin
	if (ssrb_int == 0) begin
	    dob_out <= 32'bX;
	end
	data_collision_a_b[0] <= 0;
    end

    always @(posedge data_collision_b_a[1]) begin
	if (ssra_int == 0) begin
	    doa_out <= 32'bX;
	end
	data_collision_b_a[1] <= 0;
    end

    always @(posedge data_collision_b_a[0]) begin
	if (ssrb_reg == 0) begin
	dob_out <= 32'bX;
	end
	data_collision_b_a[0] <= 0;
    end


    // Parity
    always @(posedge memory_collision) begin
	for (pmi = 0; pmi < 4; pmi = pmi + 1) begin
	    mem[parity_addra_int + pmi] <= 1'bX;
	end
    end

    always @(posedge memory_collision_a_b) begin
	for (pmi = 0; pmi < 4; pmi = pmi + 1) begin
	    mem[parity_addra_reg + pmi] <= 1'bX;
	end
    end

    always @(posedge memory_collision_b_a) begin
	for (pmi = 0; pmi < 4; pmi = pmi + 1) begin
	    mem[parity_addra_int + pmi] <= 1'bX;
	end
    end

    always @(posedge data_collision[1]) begin
	if (ssra_int == 0) begin
	    dopa_out <= 4'bX;
	end
    end

    always @(posedge data_collision[0]) begin
	if (ssrb_int == 0) begin
	    dopb_out <= 4'bX;
	end
    end

    always @(posedge data_collision_a_b[1]) begin
	if (ssra_reg == 0) begin
	    dopa_out <= 4'bX;
	end
    end

    always @(posedge data_collision_a_b[0]) begin
	if (ssrb_int == 0) begin
	dopb_out <= 4'bX;
	end
    end

    always @(posedge data_collision_b_a[1]) begin
	if (ssra_int == 0) begin
	    dopa_out <= 4'bX;
	end
    end

    always @(posedge data_collision_b_a[0]) begin
	if (ssrb_reg == 0) begin
	    dopb_out <= 4'bX;
	end
    end


    initial begin
	case (WRITE_MODE_A)
	    "WRITE_FIRST" : wr_mode_a <= 2'b00;
	    "READ_FIRST"  : wr_mode_a <= 2'b01;
	    "NO_CHANGE"   : wr_mode_a <= 2'b10;
	    default       : begin
				$display("Attribute Syntax Error : The Attribute WRITE_MODE_A on RAMB16_S36_S36 instance %m is set to %s.  Legal values for this attribute are WRITE_FIRST, READ_FIRST or NO_CHANGE.", WRITE_MODE_A);
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
				$display("Attribute Syntax Error : The Attribute WRITE_MODE_B on RAMB16_S36_S36 instance %m is set to %s.  Legal values for this attribute are WRITE_FIRST, READ_FIRST or NO_CHANGE.", WRITE_MODE_B);
				$finish;
			    end
	endcase
    end

    // Port A
    always @(posedge clka_int) begin
	if (ena_int == 1'b1) begin
	    if (ssra_int == 1'b1) begin
		doa_out[0] <= SRVAL_A[0];
		doa_out[1] <= SRVAL_A[1];
		doa_out[2] <= SRVAL_A[2];
		doa_out[3] <= SRVAL_A[3];
		doa_out[4] <= SRVAL_A[4];
		doa_out[5] <= SRVAL_A[5];
		doa_out[6] <= SRVAL_A[6];
		doa_out[7] <= SRVAL_A[7];
		doa_out[8] <= SRVAL_A[8];
		doa_out[9] <= SRVAL_A[9];
		doa_out[10] <= SRVAL_A[10];
		doa_out[11] <= SRVAL_A[11];
		doa_out[12] <= SRVAL_A[12];
		doa_out[13] <= SRVAL_A[13];
		doa_out[14] <= SRVAL_A[14];
		doa_out[15] <= SRVAL_A[15];
		doa_out[16] <= SRVAL_A[16];
		doa_out[17] <= SRVAL_A[17];
		doa_out[18] <= SRVAL_A[18];
		doa_out[19] <= SRVAL_A[19];
		doa_out[20] <= SRVAL_A[20];
		doa_out[21] <= SRVAL_A[21];
		doa_out[22] <= SRVAL_A[22];
		doa_out[23] <= SRVAL_A[23];
		doa_out[24] <= SRVAL_A[24];
		doa_out[25] <= SRVAL_A[25];
		doa_out[26] <= SRVAL_A[26];
		doa_out[27] <= SRVAL_A[27];
		doa_out[28] <= SRVAL_A[28];
		doa_out[29] <= SRVAL_A[29];
		doa_out[30] <= SRVAL_A[30];
		doa_out[31] <= SRVAL_A[31];
		dopa_out[0] <= SRVAL_A[32];
		dopa_out[1] <= SRVAL_A[33];
		dopa_out[2] <= SRVAL_A[34];
		dopa_out[3] <= SRVAL_A[35];
	    end
	    else begin
		if (wea_int == 1'b1) begin
		    if (wr_mode_a == 2'b00) begin
			doa_out <= dia_int;
			dopa_out <= dipa_int;
		    end
		    else if (wr_mode_a == 2'b01) begin
			doa_out[0] <= mem[data_addra_int + 0];
			doa_out[1] <= mem[data_addra_int + 1];
			doa_out[2] <= mem[data_addra_int + 2];
			doa_out[3] <= mem[data_addra_int + 3];
			doa_out[4] <= mem[data_addra_int + 4];
			doa_out[5] <= mem[data_addra_int + 5];
			doa_out[6] <= mem[data_addra_int + 6];
			doa_out[7] <= mem[data_addra_int + 7];
			doa_out[8] <= mem[data_addra_int + 8];
			doa_out[9] <= mem[data_addra_int + 9];
			doa_out[10] <= mem[data_addra_int + 10];
			doa_out[11] <= mem[data_addra_int + 11];
			doa_out[12] <= mem[data_addra_int + 12];
			doa_out[13] <= mem[data_addra_int + 13];
			doa_out[14] <= mem[data_addra_int + 14];
			doa_out[15] <= mem[data_addra_int + 15];
			doa_out[16] <= mem[data_addra_int + 16];
			doa_out[17] <= mem[data_addra_int + 17];
			doa_out[18] <= mem[data_addra_int + 18];
			doa_out[19] <= mem[data_addra_int + 19];
			doa_out[20] <= mem[data_addra_int + 20];
			doa_out[21] <= mem[data_addra_int + 21];
			doa_out[22] <= mem[data_addra_int + 22];
			doa_out[23] <= mem[data_addra_int + 23];
			doa_out[24] <= mem[data_addra_int + 24];
			doa_out[25] <= mem[data_addra_int + 25];
			doa_out[26] <= mem[data_addra_int + 26];
			doa_out[27] <= mem[data_addra_int + 27];
			doa_out[28] <= mem[data_addra_int + 28];
			doa_out[29] <= mem[data_addra_int + 29];
			doa_out[30] <= mem[data_addra_int + 30];
			doa_out[31] <= mem[data_addra_int + 31];
			dopa_out[0] <= mem[parity_addra_int + 0];
			dopa_out[1] <= mem[parity_addra_int + 1];
			dopa_out[2] <= mem[parity_addra_int + 2];
			dopa_out[3] <= mem[parity_addra_int + 3];
		    end
		end
		else begin
		    doa_out[0] <= mem[data_addra_int + 0];
		    doa_out[1] <= mem[data_addra_int + 1];
		    doa_out[2] <= mem[data_addra_int + 2];
		    doa_out[3] <= mem[data_addra_int + 3];
		    doa_out[4] <= mem[data_addra_int + 4];
		    doa_out[5] <= mem[data_addra_int + 5];
		    doa_out[6] <= mem[data_addra_int + 6];
		    doa_out[7] <= mem[data_addra_int + 7];
		    doa_out[8] <= mem[data_addra_int + 8];
		    doa_out[9] <= mem[data_addra_int + 9];
		    doa_out[10] <= mem[data_addra_int + 10];
		    doa_out[11] <= mem[data_addra_int + 11];
		    doa_out[12] <= mem[data_addra_int + 12];
		    doa_out[13] <= mem[data_addra_int + 13];
		    doa_out[14] <= mem[data_addra_int + 14];
		    doa_out[15] <= mem[data_addra_int + 15];
		    doa_out[16] <= mem[data_addra_int + 16];
		    doa_out[17] <= mem[data_addra_int + 17];
		    doa_out[18] <= mem[data_addra_int + 18];
		    doa_out[19] <= mem[data_addra_int + 19];
		    doa_out[20] <= mem[data_addra_int + 20];
		    doa_out[21] <= mem[data_addra_int + 21];
		    doa_out[22] <= mem[data_addra_int + 22];
		    doa_out[23] <= mem[data_addra_int + 23];
		    doa_out[24] <= mem[data_addra_int + 24];
		    doa_out[25] <= mem[data_addra_int + 25];
		    doa_out[26] <= mem[data_addra_int + 26];
		    doa_out[27] <= mem[data_addra_int + 27];
		    doa_out[28] <= mem[data_addra_int + 28];
		    doa_out[29] <= mem[data_addra_int + 29];
		    doa_out[30] <= mem[data_addra_int + 30];
		    doa_out[31] <= mem[data_addra_int + 31];
		    dopa_out[0] <= mem[parity_addra_int + 0];
		    dopa_out[1] <= mem[parity_addra_int + 1];
		    dopa_out[2] <= mem[parity_addra_int + 2];
		    dopa_out[3] <= mem[parity_addra_int + 3];
		end
	    end
	end
    end

    always @(posedge clka_int) begin
	if (ena_int == 1'b1 && wea_int == 1'b1) begin
	    mem[data_addra_int + 0] <= dia_int[0];
	    mem[data_addra_int + 1] <= dia_int[1];
	    mem[data_addra_int + 2] <= dia_int[2];
	    mem[data_addra_int + 3] <= dia_int[3];
	    mem[data_addra_int + 4] <= dia_int[4];
	    mem[data_addra_int + 5] <= dia_int[5];
	    mem[data_addra_int + 6] <= dia_int[6];
	    mem[data_addra_int + 7] <= dia_int[7];
	    mem[data_addra_int + 8] <= dia_int[8];
	    mem[data_addra_int + 9] <= dia_int[9];
	    mem[data_addra_int + 10] <= dia_int[10];
	    mem[data_addra_int + 11] <= dia_int[11];
	    mem[data_addra_int + 12] <= dia_int[12];
	    mem[data_addra_int + 13] <= dia_int[13];
	    mem[data_addra_int + 14] <= dia_int[14];
	    mem[data_addra_int + 15] <= dia_int[15];
	    mem[data_addra_int + 16] <= dia_int[16];
	    mem[data_addra_int + 17] <= dia_int[17];
	    mem[data_addra_int + 18] <= dia_int[18];
	    mem[data_addra_int + 19] <= dia_int[19];
	    mem[data_addra_int + 20] <= dia_int[20];
	    mem[data_addra_int + 21] <= dia_int[21];
	    mem[data_addra_int + 22] <= dia_int[22];
	    mem[data_addra_int + 23] <= dia_int[23];
	    mem[data_addra_int + 24] <= dia_int[24];
	    mem[data_addra_int + 25] <= dia_int[25];
	    mem[data_addra_int + 26] <= dia_int[26];
	    mem[data_addra_int + 27] <= dia_int[27];
	    mem[data_addra_int + 28] <= dia_int[28];
	    mem[data_addra_int + 29] <= dia_int[29];
	    mem[data_addra_int + 30] <= dia_int[30];
	    mem[data_addra_int + 31] <= dia_int[31];
	    mem[parity_addra_int + 0] <= dipa_int[0];
	    mem[parity_addra_int + 1] <= dipa_int[1];
	    mem[parity_addra_int + 2] <= dipa_int[2];
	    mem[parity_addra_int + 3] <= dipa_int[3];
	end
    end

    // Port B
    always @(posedge clkb_int) begin
	if (enb_int == 1'b1) begin
	    if (ssrb_int == 1'b1) begin
		dob_out[0] <= SRVAL_B[0];
		dob_out[1] <= SRVAL_B[1];
		dob_out[2] <= SRVAL_B[2];
		dob_out[3] <= SRVAL_B[3];
		dob_out[4] <= SRVAL_B[4];
		dob_out[5] <= SRVAL_B[5];
		dob_out[6] <= SRVAL_B[6];
		dob_out[7] <= SRVAL_B[7];
		dob_out[8] <= SRVAL_B[8];
		dob_out[9] <= SRVAL_B[9];
		dob_out[10] <= SRVAL_B[10];
		dob_out[11] <= SRVAL_B[11];
		dob_out[12] <= SRVAL_B[12];
		dob_out[13] <= SRVAL_B[13];
		dob_out[14] <= SRVAL_B[14];
		dob_out[15] <= SRVAL_B[15];
		dob_out[16] <= SRVAL_B[16];
		dob_out[17] <= SRVAL_B[17];
		dob_out[18] <= SRVAL_B[18];
		dob_out[19] <= SRVAL_B[19];
		dob_out[20] <= SRVAL_B[20];
		dob_out[21] <= SRVAL_B[21];
		dob_out[22] <= SRVAL_B[22];
		dob_out[23] <= SRVAL_B[23];
		dob_out[24] <= SRVAL_B[24];
		dob_out[25] <= SRVAL_B[25];
		dob_out[26] <= SRVAL_B[26];
		dob_out[27] <= SRVAL_B[27];
		dob_out[28] <= SRVAL_B[28];
		dob_out[29] <= SRVAL_B[29];
		dob_out[30] <= SRVAL_B[30];
		dob_out[31] <= SRVAL_B[31];
		dopb_out[0] <= SRVAL_B[32];
		dopb_out[1] <= SRVAL_B[33];
		dopb_out[2] <= SRVAL_B[34];
		dopb_out[3] <= SRVAL_B[35];
	    end
	    else begin
		if (web_int == 1'b1) begin
		    if (wr_mode_b == 2'b00) begin
			dob_out <= dib_int;
			dopb_out <= dipb_int;
		    end
		    else if (wr_mode_b == 2'b01) begin
			dob_out[0] <= mem[data_addrb_int + 0];
			dob_out[1] <= mem[data_addrb_int + 1];
			dob_out[2] <= mem[data_addrb_int + 2];
			dob_out[3] <= mem[data_addrb_int + 3];
			dob_out[4] <= mem[data_addrb_int + 4];
			dob_out[5] <= mem[data_addrb_int + 5];
			dob_out[6] <= mem[data_addrb_int + 6];
			dob_out[7] <= mem[data_addrb_int + 7];
			dob_out[8] <= mem[data_addrb_int + 8];
			dob_out[9] <= mem[data_addrb_int + 9];
			dob_out[10] <= mem[data_addrb_int + 10];
			dob_out[11] <= mem[data_addrb_int + 11];
			dob_out[12] <= mem[data_addrb_int + 12];
			dob_out[13] <= mem[data_addrb_int + 13];
			dob_out[14] <= mem[data_addrb_int + 14];
			dob_out[15] <= mem[data_addrb_int + 15];
			dob_out[16] <= mem[data_addrb_int + 16];
			dob_out[17] <= mem[data_addrb_int + 17];
			dob_out[18] <= mem[data_addrb_int + 18];
			dob_out[19] <= mem[data_addrb_int + 19];
			dob_out[20] <= mem[data_addrb_int + 20];
			dob_out[21] <= mem[data_addrb_int + 21];
			dob_out[22] <= mem[data_addrb_int + 22];
			dob_out[23] <= mem[data_addrb_int + 23];
			dob_out[24] <= mem[data_addrb_int + 24];
			dob_out[25] <= mem[data_addrb_int + 25];
			dob_out[26] <= mem[data_addrb_int + 26];
			dob_out[27] <= mem[data_addrb_int + 27];
			dob_out[28] <= mem[data_addrb_int + 28];
			dob_out[29] <= mem[data_addrb_int + 29];
			dob_out[30] <= mem[data_addrb_int + 30];
			dob_out[31] <= mem[data_addrb_int + 31];
			dopb_out[0] <= mem[parity_addrb_int + 0];
			dopb_out[1] <= mem[parity_addrb_int + 1];
			dopb_out[2] <= mem[parity_addrb_int + 2];
			dopb_out[3] <= mem[parity_addrb_int + 3];
		    end
		end
		else begin
		    dob_out[0] <= mem[data_addrb_int + 0];
		    dob_out[1] <= mem[data_addrb_int + 1];
		    dob_out[2] <= mem[data_addrb_int + 2];
		    dob_out[3] <= mem[data_addrb_int + 3];
		    dob_out[4] <= mem[data_addrb_int + 4];
		    dob_out[5] <= mem[data_addrb_int + 5];
		    dob_out[6] <= mem[data_addrb_int + 6];
		    dob_out[7] <= mem[data_addrb_int + 7];
		    dob_out[8] <= mem[data_addrb_int + 8];
		    dob_out[9] <= mem[data_addrb_int + 9];
		    dob_out[10] <= mem[data_addrb_int + 10];
		    dob_out[11] <= mem[data_addrb_int + 11];
		    dob_out[12] <= mem[data_addrb_int + 12];
		    dob_out[13] <= mem[data_addrb_int + 13];
		    dob_out[14] <= mem[data_addrb_int + 14];
		    dob_out[15] <= mem[data_addrb_int + 15];
		    dob_out[16] <= mem[data_addrb_int + 16];
		    dob_out[17] <= mem[data_addrb_int + 17];
		    dob_out[18] <= mem[data_addrb_int + 18];
		    dob_out[19] <= mem[data_addrb_int + 19];
		    dob_out[20] <= mem[data_addrb_int + 20];
		    dob_out[21] <= mem[data_addrb_int + 21];
		    dob_out[22] <= mem[data_addrb_int + 22];
		    dob_out[23] <= mem[data_addrb_int + 23];
		    dob_out[24] <= mem[data_addrb_int + 24];
		    dob_out[25] <= mem[data_addrb_int + 25];
		    dob_out[26] <= mem[data_addrb_int + 26];
		    dob_out[27] <= mem[data_addrb_int + 27];
		    dob_out[28] <= mem[data_addrb_int + 28];
		    dob_out[29] <= mem[data_addrb_int + 29];
		    dob_out[30] <= mem[data_addrb_int + 30];
		    dob_out[31] <= mem[data_addrb_int + 31];
		    dopb_out[0] <= mem[parity_addrb_int + 0];
		    dopb_out[1] <= mem[parity_addrb_int + 1];
		    dopb_out[2] <= mem[parity_addrb_int + 2];
		    dopb_out[3] <= mem[parity_addrb_int + 3];
		end
	    end
	end
    end

    always @(posedge clkb_int) begin
	if (enb_int == 1'b1 && web_int == 1'b1) begin
	    mem[data_addrb_int + 0] <= dib_int[0];
	    mem[data_addrb_int + 1] <= dib_int[1];
	    mem[data_addrb_int + 2] <= dib_int[2];
	    mem[data_addrb_int + 3] <= dib_int[3];
	    mem[data_addrb_int + 4] <= dib_int[4];
	    mem[data_addrb_int + 5] <= dib_int[5];
	    mem[data_addrb_int + 6] <= dib_int[6];
	    mem[data_addrb_int + 7] <= dib_int[7];
	    mem[data_addrb_int + 8] <= dib_int[8];
	    mem[data_addrb_int + 9] <= dib_int[9];
	    mem[data_addrb_int + 10] <= dib_int[10];
	    mem[data_addrb_int + 11] <= dib_int[11];
	    mem[data_addrb_int + 12] <= dib_int[12];
	    mem[data_addrb_int + 13] <= dib_int[13];
	    mem[data_addrb_int + 14] <= dib_int[14];
	    mem[data_addrb_int + 15] <= dib_int[15];
	    mem[data_addrb_int + 16] <= dib_int[16];
	    mem[data_addrb_int + 17] <= dib_int[17];
	    mem[data_addrb_int + 18] <= dib_int[18];
	    mem[data_addrb_int + 19] <= dib_int[19];
	    mem[data_addrb_int + 20] <= dib_int[20];
	    mem[data_addrb_int + 21] <= dib_int[21];
	    mem[data_addrb_int + 22] <= dib_int[22];
	    mem[data_addrb_int + 23] <= dib_int[23];
	    mem[data_addrb_int + 24] <= dib_int[24];
	    mem[data_addrb_int + 25] <= dib_int[25];
	    mem[data_addrb_int + 26] <= dib_int[26];
	    mem[data_addrb_int + 27] <= dib_int[27];
	    mem[data_addrb_int + 28] <= dib_int[28];
	    mem[data_addrb_int + 29] <= dib_int[29];
	    mem[data_addrb_int + 30] <= dib_int[30];
	    mem[data_addrb_int + 31] <= dib_int[31];
	    mem[parity_addrb_int + 0] <= dipb_int[0];
	    mem[parity_addrb_int + 1] <= dipb_int[1];
	    mem[parity_addrb_int + 2] <= dipb_int[2];
	    mem[parity_addrb_int + 3] <= dipb_int[3];
	end
    end

    specify
	(CLKA *> DOA) = (100, 100);
	(CLKA *> DOPA) = (100, 100);
	(CLKB *> DOB) = (100, 100);
	(CLKB *> DOPB) = (100, 100);
    endspecify

endmodule

`else

// $Header: /devl/xcs/repo/env/Databases/CAEInterfaces/verunilibs/data/unisims/RAMB16_S36_S36.v,v 1.10 2007/02/22 01:58:06 wloo Exp $
///////////////////////////////////////////////////////////////////////////////
// Copyright (c) 1995/2005 Xilinx, Inc.
// All Right Reserved.
///////////////////////////////////////////////////////////////////////////////
//   ____  ____
//  /   /\/   /
// /___/  \  /    Vendor : Xilinx
// \   \   \/     Version : 10.1
//  \   \         Description : Xilinx Timing Simulation Library Component
//  /   /                  16K-Bit Data and 2K-Bit Parity Dual Port Block RAM
// /___/   /\     Filename : RAMB16_S36_S36.v
// \   \  /  \    Timestamp : Thu Mar 10 16:44:01 PST 2005
//  \___\/\___\
//
// Revision:
//    03/23/04 - Initial version.
//    03/10/05 - Initialized outputs.
//    02/21/07 - Fixed parameter SIM_COLLISION_CHECK (CR 433281).
// End Revision

`timescale 1 ps/1 ps

module RAMB16_S36_S36 (DOA, DOB, DOPA, DOPB, ADDRA, ADDRB, CLKA, CLKB, DIA, DIB, DIPA, DIPB, ENA, ENB, SSRA, SSRB, WEA, WEB);

    parameter INIT_A = 36'h0;
    parameter INIT_B = 36'h0;
    parameter SRVAL_A = 36'h0;
    parameter SRVAL_B = 36'h0;
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
    parameter INITP_00 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INITP_01 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INITP_02 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INITP_03 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INITP_04 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INITP_05 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INITP_06 = 256'h0000000000000000000000000000000000000000000000000000000000000000;
    parameter INITP_07 = 256'h0000000000000000000000000000000000000000000000000000000000000000;

    output [31:0] DOA;
    output [3:0] DOPA;
    output [31:0] DOB;
    output [3:0] DOPB;

    input [8:0] ADDRA;
    input [31:0] DIA;
    input [3:0] DIPA;
    input ENA, CLKA, WEA, SSRA;
    input [8:0] ADDRB;
    input [31:0] DIB;
    input [3:0] DIPB;
    input ENB, CLKB, WEB, SSRB;

    reg [31:0] doa_out = INIT_A[31:0];
    reg [3:0] dopa_out = INIT_A[35:32];
    reg [31:0] dob_out = INIT_B[31:0];
    reg [3:0] dopb_out = INIT_B[35:32];
    
    reg [31:0] mem [511:0];
    reg [3:0] memp [511:0];
    
    reg [8:0] count, countp;
    reg [1:0] wr_mode_a, wr_mode_b;

    reg [5:0] dmi, dbi;
    reg [5:0] pmi, pbi;

    wire [8:0] addra_int;
    reg [8:0] addra_reg;
    wire [31:0] dia_int;
    wire [3:0] dipa_int;
    wire ena_int, clka_int, wea_int, ssra_int;
    reg ena_reg, wea_reg, ssra_reg;
    wire [8:0] addrb_int;
    reg [8:0] addrb_reg;
    wire [31:0] dib_int;
    wire [3:0] dipb_int;
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

    buf b_doa [31:0] (DOA, doa_out);
    buf b_dopa [3:0] (DOPA, dopa_out);
    buf b_addra [8:0] (addra_int, ADDRA);
    buf b_dia [31:0] (dia_int, DIA);
    buf b_dipa [3:0] (dipa_int, DIPA);
    buf b_ena (ena_int, ENA);
    buf b_clka (clka_int, CLKA);
    buf b_ssra (ssra_int, SSRA);
    buf b_wea (wea_int, WEA);

    buf b_dob [31:0] (DOB, dob_out);
    buf b_dopb [3:0] (DOPB, dopb_out);
    buf b_addrb [8:0] (addrb_int, ADDRB);
    buf b_dib [31:0] (dib_int, DIB);
    buf b_dipb [3:0] (dipb_int, DIPB);
    buf b_enb (enb_int, ENB);
    buf b_clkb (clkb_int, CLKB);
    buf b_ssrb (ssrb_int, SSRB);
    buf b_web (web_int, WEB);

    
    always @(gsr_int)
	if (gsr_int) begin
	    assign {dopa_out, doa_out} = INIT_A;
	    assign {dopb_out, dob_out} = INIT_B;
	end
	else begin
	    deassign doa_out;
	    deassign dopa_out;
	    deassign dob_out;
	    deassign dopb_out;
	end

    
    initial begin

	for (count = 0; count < 8; count = count + 1) begin
	    mem[count]          = INIT_00[(count * 32) +: 32];
	    mem[8 * 1 + count]  = INIT_01[(count * 32) +: 32];
	    mem[8 * 2 + count]  = INIT_02[(count * 32) +: 32];
	    mem[8 * 3 + count]  = INIT_03[(count * 32) +: 32];
	    mem[8 * 4 + count]  = INIT_04[(count * 32) +: 32];
	    mem[8 * 5 + count]  = INIT_05[(count * 32) +: 32];
	    mem[8 * 6 + count]  = INIT_06[(count * 32) +: 32];
	    mem[8 * 7 + count]  = INIT_07[(count * 32) +: 32];
	    mem[8 * 8 + count]  = INIT_08[(count * 32) +: 32];
	    mem[8 * 9 + count]  = INIT_09[(count * 32) +: 32];
	    mem[8 * 10 + count] = INIT_0A[(count * 32) +: 32];
	    mem[8 * 11 + count] = INIT_0B[(count * 32) +: 32];
	    mem[8 * 12 + count] = INIT_0C[(count * 32) +: 32];
	    mem[8 * 13 + count] = INIT_0D[(count * 32) +: 32];
	    mem[8 * 14 + count] = INIT_0E[(count * 32) +: 32];
	    mem[8 * 15 + count] = INIT_0F[(count * 32) +: 32];
	    mem[8 * 16 + count] = INIT_10[(count * 32) +: 32];
	    mem[8 * 17 + count] = INIT_11[(count * 32) +: 32];
	    mem[8 * 18 + count] = INIT_12[(count * 32) +: 32];
	    mem[8 * 19 + count] = INIT_13[(count * 32) +: 32];
	    mem[8 * 20 + count] = INIT_14[(count * 32) +: 32];
	    mem[8 * 21 + count] = INIT_15[(count * 32) +: 32];
	    mem[8 * 22 + count] = INIT_16[(count * 32) +: 32];
	    mem[8 * 23 + count] = INIT_17[(count * 32) +: 32];
	    mem[8 * 24 + count] = INIT_18[(count * 32) +: 32];
	    mem[8 * 25 + count] = INIT_19[(count * 32) +: 32];
	    mem[8 * 26 + count] = INIT_1A[(count * 32) +: 32];
	    mem[8 * 27 + count] = INIT_1B[(count * 32) +: 32];
	    mem[8 * 28 + count] = INIT_1C[(count * 32) +: 32];
	    mem[8 * 29 + count] = INIT_1D[(count * 32) +: 32];
	    mem[8 * 30 + count] = INIT_1E[(count * 32) +: 32];
	    mem[8 * 31 + count] = INIT_1F[(count * 32) +: 32];
	    mem[8 * 32 + count] = INIT_20[(count * 32) +: 32];
	    mem[8 * 33 + count] = INIT_21[(count * 32) +: 32];
	    mem[8 * 34 + count] = INIT_22[(count * 32) +: 32];
	    mem[8 * 35 + count] = INIT_23[(count * 32) +: 32];
	    mem[8 * 36 + count] = INIT_24[(count * 32) +: 32];
	    mem[8 * 37 + count] = INIT_25[(count * 32) +: 32];
	    mem[8 * 38 + count] = INIT_26[(count * 32) +: 32];
	    mem[8 * 39 + count] = INIT_27[(count * 32) +: 32];
	    mem[8 * 40 + count] = INIT_28[(count * 32) +: 32];
	    mem[8 * 41 + count] = INIT_29[(count * 32) +: 32];
	    mem[8 * 42 + count] = INIT_2A[(count * 32) +: 32];
	    mem[8 * 43 + count] = INIT_2B[(count * 32) +: 32];
	    mem[8 * 44 + count] = INIT_2C[(count * 32) +: 32];
	    mem[8 * 45 + count] = INIT_2D[(count * 32) +: 32];
	    mem[8 * 46 + count] = INIT_2E[(count * 32) +: 32];
	    mem[8 * 47 + count] = INIT_2F[(count * 32) +: 32];
	    mem[8 * 48 + count] = INIT_30[(count * 32) +: 32];
	    mem[8 * 49 + count] = INIT_31[(count * 32) +: 32];
	    mem[8 * 50 + count] = INIT_32[(count * 32) +: 32];
	    mem[8 * 51 + count] = INIT_33[(count * 32) +: 32];
	    mem[8 * 52 + count] = INIT_34[(count * 32) +: 32];
	    mem[8 * 53 + count] = INIT_35[(count * 32) +: 32];
	    mem[8 * 54 + count] = INIT_36[(count * 32) +: 32];
	    mem[8 * 55 + count] = INIT_37[(count * 32) +: 32];
	    mem[8 * 56 + count] = INIT_38[(count * 32) +: 32];
	    mem[8 * 57 + count] = INIT_39[(count * 32) +: 32];
	    mem[8 * 58 + count] = INIT_3A[(count * 32) +: 32];
	    mem[8 * 59 + count] = INIT_3B[(count * 32) +: 32];
	    mem[8 * 60 + count] = INIT_3C[(count * 32) +: 32];
	    mem[8 * 61 + count] = INIT_3D[(count * 32) +: 32];
	    mem[8 * 62 + count] = INIT_3E[(count * 32) +: 32];
	    mem[8 * 63 + count] = INIT_3F[(count * 32) +: 32];
	end

// initiate parity start
	for (countp = 0; countp < 64; countp = countp + 1) begin
	    memp[countp]          = INITP_00[(countp * 4) +: 4];
	    memp[64 * 1 + countp] = INITP_01[(countp * 4) +: 4];
	    memp[64 * 2 + countp] = INITP_02[(countp * 4) +: 4];
	    memp[64 * 3 + countp] = INITP_03[(countp * 4) +: 4];
	    memp[64 * 4 + countp] = INITP_04[(countp * 4) +: 4];
	    memp[64 * 5 + countp] = INITP_05[(countp * 4) +: 4];
	    memp[64 * 6 + countp] = INITP_06[(countp * 4) +: 4];
	    memp[64 * 7 + countp] = INITP_07[(countp * 4) +: 4];
	end
// initiate parity end
	
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

    assign data_addra_int = addra_int * 32;
    assign data_addra_reg = addra_reg * 32;
    assign data_addrb_int = addrb_int * 32;
    assign data_addrb_reg = addrb_reg * 32;


    initial begin

	display_flag = 1;
	output_flag = 1;
	
	case (SIM_COLLISION_CHECK)

	    "NONE" : begin
		         output_flag = 0;
	                 display_flag = 0;
	             end
	    "WARNING_ONLY" : output_flag = 0;
	    "GENERATE_X_ONLY" : display_flag = 0;
	    "ALL" : ;

	    default : begin
		          $display("Attribute Syntax Error : The Attribute SIM_COLLISION_CHECK on RAMB16_S36_S36 instance %m is set to %s.  Legal values for this attribute are ALL, NONE, WARNING_ONLY or GENERATE_X_ONLY.", SIM_COLLISION_CHECK);
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
	    (data_addra_int[14:5] == data_addrb_int[14:5]))
	    memory_collision <= 1;
    end

    always @(posedge setup_all_a_b or posedge setup_rf_a_b) begin
	if ((ena_reg == 1) && (wea_reg == 1) &&
	    (enb_int == 1) && (web_int == 1) &&
	    (data_addra_reg[14:5] == data_addrb_int[14:5]))
	    memory_collision_a_b <= 1;
    end

    always @(posedge setup_all_b_a or posedge setup_rf_b_a) begin
	if ((ena_int == 1) && (wea_int == 1) &&
	    (enb_reg == 1) && (web_reg == 1) &&
	    (data_addra_int[14:5] == data_addrb_reg[14:5]))
	    memory_collision_b_a <= 1;
    end

    always @(posedge setup_all_a_b) begin
	if (data_addra_reg[14:5] == data_addrb_int[14:5]) begin
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
	if (data_addra_int[14:5] == data_addrb_reg[14:5]) begin
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
	if (data_addra_int[14:5] == data_addrb_int[14:5]) begin
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
        $display("Memory Collision Error on RAMB16_S36_S36:%m at simulation time %.3f ns\nA read was performed on address %h (hex) of Port A while a write was requested to the same address on Port B. The write will be successful however the read value on Port A is unknown until the next CLKA cycle.", $time/1000.0, addra_int);
    end
    endtask

    task display_wa_rb;
    begin
	if (display_flag)
        $display("Memory Collision Error on RAMB16_S36_S36:%m at simulation time %.3f ns\nA read was performed on address %h (hex) of Port B while a write was requested to the same address on Port A. The write will be successful however the read value on Port B is unknown until the next CLKB cycle.", $time/1000.0, addrb_int);
    end
    endtask

    task display_wa_wb;
    begin
	if (display_flag)
        $display("Memory Collision Error on RAMB16_S36_S36:%m at simulation time %.3f ns\nA write was requested to the same address simultaneously at both Port A and Port B of the RAM. The contents written to the RAM at address location %h (hex) of Port A and address location %h (hex) of Port B are unknown.", $time/1000.0, addra_int, addrb_int);
    end
    endtask


    always @(posedge setup_rf_a_b) begin
	if (data_addra_reg[14:5] == data_addrb_int[14:5]) begin
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
	if (data_addra_int[14:5] == data_addrb_reg[14:5]) begin
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
	    mem[addra_int] <= 32'bx;
	    memory_collision <= 0;
	end
	
    end

    always @(posedge memory_collision_a_b) begin
	if ((output_flag || display_flag)) begin
	    mem[addra_reg] <= 32'bx;
	    memory_collision_a_b <= 0;
	end
    end
    
    always @(posedge memory_collision_b_a) begin
	if ((output_flag || display_flag)) begin
	    mem[addra_int] <= 32'bx;
	    memory_collision_b_a <= 0;
	end
    end
    
    always @(posedge data_collision[1]) begin
	if (ssra_int == 0 && output_flag) begin
	    doa_out <= #100 32'bX;
	end
	data_collision[1] <= 0;
    end

    always @(posedge data_collision[0]) begin
	if (ssrb_int == 0 && output_flag) begin
	    dob_out <= #100 32'bX;
	end
	data_collision[0] <= 0;
    end

    always @(posedge data_collision_a_b[1]) begin
	if (ssra_reg == 0 && output_flag) begin
	    doa_out <= #100 32'bX;
	end
	data_collision_a_b[1] <= 0;
    end

    always @(posedge data_collision_a_b[0]) begin
	if (ssrb_int == 0 && output_flag) begin
	    dob_out <= #100 32'bX;
	end
	data_collision_a_b[0] <= 0;
    end

    always @(posedge data_collision_b_a[1]) begin
	if (ssra_int == 0 && output_flag) begin
	    doa_out <= #100 32'bX;
	end
	data_collision_b_a[1] <= 0;
    end

    always @(posedge data_collision_b_a[0]) begin
	if (ssrb_reg == 0 && output_flag) begin
	    dob_out <= #100 32'bX;
	end
	data_collision_b_a[0] <= 0;
    end

// x parity start
    always @(posedge memory_collision) begin
	if ((output_flag || display_flag))
	    memp[addra_int] <= 4'bx;
    end

    always @(posedge memory_collision_a_b) begin
	if ((output_flag || display_flag))
	    memp[addra_reg] <= 4'bx;
    end

    always @(posedge memory_collision_b_a) begin
	if ((output_flag || display_flag))
	    memp[addra_int] <= 4'bx;
    end
    
    always @(posedge data_collision[1]) begin
	if (ssra_int == 0 && output_flag) begin
	    dopa_out <= #100 4'bX;
	end
    end

    always @(posedge data_collision_a_b[1]) begin
	if (ssra_reg == 0 && output_flag) begin
	    dopa_out <= #100 4'bX;
	end
    end

    
    always @(posedge data_collision_b_a[1]) begin
	if (ssra_int == 0 && output_flag) begin
	    dopa_out <= #100 4'bX;
	end
    end

    always @(posedge data_collision[0]) begin
	if (ssrb_int == 0 && output_flag) begin
	    dopb_out <= #100 4'bx;
	end
    end

    always @(posedge data_collision_a_b[0]) begin
	if (ssrb_int == 0 && output_flag) begin
	    dopb_out <= #100 4'bx;
	end
    end

    always @(posedge data_collision_b_a[0]) begin
	if (ssrb_reg == 0 && output_flag) begin
	    dopb_out <= #100 4'bx;
	end
    end
// x parity end

    initial begin
	case (WRITE_MODE_A)
	    "WRITE_FIRST" : wr_mode_a <= 2'b00;
	    "READ_FIRST"  : wr_mode_a <= 2'b01;
	    "NO_CHANGE"   : wr_mode_a <= 2'b10;
	    default       : begin
				$display("Attribute Syntax Error : The Attribute WRITE_MODE_A on RAMB16_S36_S36 instance %m is set to %s.  Legal values for this attribute are WRITE_FIRST, READ_FIRST or NO_CHANGE.", WRITE_MODE_A);
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
				$display("Attribute Syntax Error : The Attribute WRITE_MODE_B on RAMB16_S36_S36 instance %m is set to %s.  Legal values for this attribute are WRITE_FIRST, READ_FIRST or NO_CHANGE.", WRITE_MODE_B);
				$finish;
			    end
	endcase
    end


    // Port A
    always @(posedge clka_int) begin

	if (ena_int == 1'b1) begin

	    if (ssra_int == 1'b1) begin
		{dopa_out, doa_out} <= #100 SRVAL_A;
	    end
	    else begin
		if (wea_int == 1'b1) begin
		    if (wr_mode_a == 2'b00) begin
			doa_out <= #100 dia_int;
			dopa_out <= #100 dipa_int;
		    end
		    else if (wr_mode_a == 2'b01) begin

			doa_out <= #100 mem[addra_int];
			dopa_out <= #100 memp[addra_int];

		    end
		end
		else begin

		    doa_out <= #100 mem[addra_int];
		    dopa_out <= #100 memp[addra_int];
		    
		end
	    end

	    // memory
	    if (wea_int == 1'b1) begin
		mem[addra_int] <= dia_int;
		memp[addra_int] <= dipa_int;
	    end
	    
	end
    end


    // Port B
    always @(posedge clkb_int) begin

	if (enb_int == 1'b1) begin

	    if (ssrb_int == 1'b1) begin
		{dopb_out, dob_out} <= #100 SRVAL_B;
	    end
	    else begin
		if (web_int == 1'b1) begin
		    if (wr_mode_b == 2'b00) begin
			dob_out <= #100 dib_int;
			dopb_out <= #100 dipb_int;
		    end
		    else if (wr_mode_b == 2'b01) begin
			dob_out <= #100 mem[addrb_int];
			dopb_out <= #100 memp[addrb_int];
		    end
		end
		else begin
		    dob_out <= #100 mem[addrb_int];
		    dopb_out <= #100 memp[addrb_int];
		end
	    end

	    // memory
	    if (web_int == 1'b1) begin
		mem[addrb_int] <= dib_int;
		memp[addrb_int] <= dipb_int;
	    end

	end
    end


endmodule

`endif
