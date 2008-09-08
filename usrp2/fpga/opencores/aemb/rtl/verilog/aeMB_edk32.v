/* $Id: aeMB_edk32.v,v 1.14 2008/01/19 16:01:22 sybreon Exp $
**
** AEMB EDK 3.2 Compatible Core
** Copyright (C) 2004-2007 Shawn Tan Ser Ngiap <shawn.tan@aeste.net>
**  
** This file is part of AEMB.
**
** AEMB is free software: you can redistribute it and/or modify it
** under the terms of the GNU Lesser General Public License as
** published by the Free Software Foundation, either version 3 of the
** License, or (at your option) any later version.
**
** AEMB is distributed in the hope that it will be useful, but WITHOUT
** ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
** or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General
** Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public
** License along with AEMB. If not, see <http://www.gnu.org/licenses/>.
*/

module aeMB_edk32 (/*AUTOARG*/
   // Outputs
   iwb_stb_o, iwb_adr_o, fsl_wre_o, fsl_tag_o, fsl_stb_o, fsl_dat_o,
   fsl_adr_o, dwb_wre_o, dwb_stb_o, dwb_sel_o, dwb_dat_o, dwb_adr_o,
   // Inputs
   sys_int_i, iwb_dat_i, iwb_ack_i, fsl_dat_i, fsl_ack_i, dwb_dat_i,
   dwb_ack_i, sys_clk_i, sys_rst_i
   );
   // Bus widths
   parameter IW = 32; /// Instruction bus address width
   parameter DW = 32; /// Data bus address width

   // Optional functions
   parameter MUL = 0; // Multiplier
   parameter BSF = 1; // Barrel Shifter
   
   /*AUTOOUTPUT*/
   // Beginning of automatic outputs (from unused autoinst outputs)
   output [DW-1:2]	dwb_adr_o;		// From xecu of aeMB_xecu.v
   output [31:0]	dwb_dat_o;		// From regf of aeMB_regf.v
   output [3:0]		dwb_sel_o;		// From xecu of aeMB_xecu.v
   output		dwb_stb_o;		// From ctrl of aeMB_ctrl.v
   output		dwb_wre_o;		// From ctrl of aeMB_ctrl.v
   output [6:2]		fsl_adr_o;		// From xecu of aeMB_xecu.v
   output [31:0]	fsl_dat_o;		// From regf of aeMB_regf.v
   output		fsl_stb_o;		// From ctrl of aeMB_ctrl.v
   output [1:0]		fsl_tag_o;		// From xecu of aeMB_xecu.v
   output		fsl_wre_o;		// From ctrl of aeMB_ctrl.v
   output [IW-1:2]	iwb_adr_o;		// From bpcu of aeMB_bpcu.v
   output		iwb_stb_o;		// From ibuf of aeMB_ibuf.v
   // End of automatics
   /*AUTOINPUT*/
   // Beginning of automatic inputs (from unused autoinst inputs)
   input		dwb_ack_i;		// To ctrl of aeMB_ctrl.v
   input [31:0]		dwb_dat_i;		// To regf of aeMB_regf.v
   input		fsl_ack_i;		// To ctrl of aeMB_ctrl.v
   input [31:0]		fsl_dat_i;		// To regf of aeMB_regf.v
   input		iwb_ack_i;		// To ibuf of aeMB_ibuf.v, ...
   input [31:0]		iwb_dat_i;		// To ibuf of aeMB_ibuf.v
   input		sys_int_i;		// To ibuf of aeMB_ibuf.v
   // End of automatics
   /*AUTOWIRE*/
   // Beginning of automatic wires (for undeclared instantiated-module outputs)
   wire [10:0]		rALT;			// From ibuf of aeMB_ibuf.v
   wire			rBRA;			// From bpcu of aeMB_bpcu.v
   wire			rDLY;			// From bpcu of aeMB_bpcu.v
   wire [31:0]		rDWBDI;			// From regf of aeMB_regf.v
   wire [3:0]		rDWBSEL;		// From xecu of aeMB_xecu.v
   wire [15:0]		rIMM;			// From ibuf of aeMB_ibuf.v
   wire			rMSR_BIP;		// From xecu of aeMB_xecu.v
   wire			rMSR_IE;		// From xecu of aeMB_xecu.v
   wire [1:0]		rMXALT;			// From ctrl of aeMB_ctrl.v
   wire [2:0]		rMXALU;			// From ctrl of aeMB_ctrl.v
   wire [1:0]		rMXDST;			// From ctrl of aeMB_ctrl.v
   wire [1:0]		rMXSRC;			// From ctrl of aeMB_ctrl.v
   wire [1:0]		rMXTGT;			// From ctrl of aeMB_ctrl.v
   wire [5:0]		rOPC;			// From ibuf of aeMB_ibuf.v
   wire [31:2]		rPC;			// From bpcu of aeMB_bpcu.v
   wire [31:2]		rPCLNK;			// From bpcu of aeMB_bpcu.v
   wire [4:0]		rRA;			// From ibuf of aeMB_ibuf.v
   wire [4:0]		rRB;			// From ibuf of aeMB_ibuf.v
   wire [4:0]		rRD;			// From ibuf of aeMB_ibuf.v
   wire [31:0]		rREGA;			// From regf of aeMB_regf.v
   wire [31:0]		rREGB;			// From regf of aeMB_regf.v
   wire [31:0]		rRESULT;		// From xecu of aeMB_xecu.v
   wire [4:0]		rRW;			// From ctrl of aeMB_ctrl.v
   wire [31:0]		rSIMM;			// From ibuf of aeMB_ibuf.v
   wire			rSTALL;			// From ibuf of aeMB_ibuf.v
   wire [31:0]		xIREG;			// From ibuf of aeMB_ibuf.v
   // End of automatics

   input 		sys_clk_i;
   input 		sys_rst_i;

   wire 		grst = sys_rst_i;
   wire 		gclk = sys_clk_i;
   wire 		gena = !((dwb_stb_o ^ dwb_ack_i) | (fsl_stb_o ^ fsl_ack_i) | !iwb_ack_i) & !rSTALL;   
   wire 		oena = ((dwb_stb_o ^ dwb_ack_i) | (fsl_stb_o ^ fsl_ack_i) | !iwb_ack_i);   
   
   // --- INSTANTIATIONS -------------------------------------
          
   aeMB_ibuf
     ibuf (/*AUTOINST*/
	   // Outputs
	   .rIMM			(rIMM[15:0]),
	   .rRA				(rRA[4:0]),
	   .rRD				(rRD[4:0]),
	   .rRB				(rRB[4:0]),
	   .rALT			(rALT[10:0]),
	   .rOPC			(rOPC[5:0]),
	   .rSIMM			(rSIMM[31:0]),
	   .xIREG			(xIREG[31:0]),
	   .rSTALL			(rSTALL),
	   .iwb_stb_o			(iwb_stb_o),
	   // Inputs
	   .rBRA			(rBRA),
	   .rMSR_IE			(rMSR_IE),
	   .rMSR_BIP			(rMSR_BIP),
	   .iwb_dat_i			(iwb_dat_i[31:0]),
	   .iwb_ack_i			(iwb_ack_i),
	   .sys_int_i			(sys_int_i),
	   .gclk			(gclk),
	   .grst			(grst),
	   .gena			(gena),
	   .oena			(oena));   
   
   aeMB_ctrl
     ctrl (/*AUTOINST*/
	   // Outputs
	   .rMXDST			(rMXDST[1:0]),
	   .rMXSRC			(rMXSRC[1:0]),
	   .rMXTGT			(rMXTGT[1:0]),
	   .rMXALT			(rMXALT[1:0]),
	   .rMXALU			(rMXALU[2:0]),
	   .rRW				(rRW[4:0]),
	   .dwb_stb_o			(dwb_stb_o),
	   .dwb_wre_o			(dwb_wre_o),
	   .fsl_stb_o			(fsl_stb_o),
	   .fsl_wre_o			(fsl_wre_o),
	   // Inputs
	   .rDLY			(rDLY),
	   .rIMM			(rIMM[15:0]),
	   .rALT			(rALT[10:0]),
	   .rOPC			(rOPC[5:0]),
	   .rRD				(rRD[4:0]),
	   .rRA				(rRA[4:0]),
	   .rRB				(rRB[4:0]),
	   .rPC				(rPC[31:2]),
	   .rBRA			(rBRA),
	   .rMSR_IE			(rMSR_IE),
	   .xIREG			(xIREG[31:0]),
	   .dwb_ack_i			(dwb_ack_i),
	   .iwb_ack_i			(iwb_ack_i),
	   .fsl_ack_i			(fsl_ack_i),
	   .gclk			(gclk),
	   .grst			(grst),
	   .gena			(gena));

   aeMB_bpcu #(IW)
     bpcu (/*AUTOINST*/
	   // Outputs
	   .iwb_adr_o			(iwb_adr_o[IW-1:2]),
	   .rPC				(rPC[31:2]),
	   .rPCLNK			(rPCLNK[31:2]),
	   .rBRA			(rBRA),
	   .rDLY			(rDLY),
	   // Inputs
	   .rMXALT			(rMXALT[1:0]),
	   .rOPC			(rOPC[5:0]),
	   .rRD				(rRD[4:0]),
	   .rRA				(rRA[4:0]),
	   .rRESULT			(rRESULT[31:0]),
	   .rDWBDI			(rDWBDI[31:0]),
	   .rREGA			(rREGA[31:0]),
	   .gclk			(gclk),
	   .grst			(grst),
	   .gena			(gena));

   aeMB_regf
     regf (/*AUTOINST*/
	   // Outputs
	   .rREGA			(rREGA[31:0]),
	   .rREGB			(rREGB[31:0]),
	   .rDWBDI			(rDWBDI[31:0]),
	   .dwb_dat_o			(dwb_dat_o[31:0]),
	   .fsl_dat_o			(fsl_dat_o[31:0]),
	   // Inputs
	   .rOPC			(rOPC[5:0]),
	   .rRA				(rRA[4:0]),
	   .rRB				(rRB[4:0]),
	   .rRW				(rRW[4:0]),
	   .rRD				(rRD[4:0]),
	   .rMXDST			(rMXDST[1:0]),
	   .rPCLNK			(rPCLNK[31:2]),
	   .rRESULT			(rRESULT[31:0]),
	   .rDWBSEL			(rDWBSEL[3:0]),
	   .rBRA			(rBRA),
	   .rDLY			(rDLY),
	   .dwb_dat_i			(dwb_dat_i[31:0]),
	   .fsl_dat_i			(fsl_dat_i[31:0]),
	   .gclk			(gclk),
	   .grst			(grst),
	   .gena			(gena));   

   aeMB_xecu #(DW, MUL, BSF)
     xecu (/*AUTOINST*/
	   // Outputs
	   .dwb_adr_o			(dwb_adr_o[DW-1:2]),
	   .dwb_sel_o			(dwb_sel_o[3:0]),
	   .fsl_adr_o			(fsl_adr_o[6:2]),
	   .fsl_tag_o			(fsl_tag_o[1:0]),
	   .rRESULT			(rRESULT[31:0]),
	   .rDWBSEL			(rDWBSEL[3:0]),
	   .rMSR_IE			(rMSR_IE),
	   .rMSR_BIP			(rMSR_BIP),
	   // Inputs
	   .rREGA			(rREGA[31:0]),
	   .rREGB			(rREGB[31:0]),
	   .rMXSRC			(rMXSRC[1:0]),
	   .rMXTGT			(rMXTGT[1:0]),
	   .rRA				(rRA[4:0]),
	   .rRB				(rRB[4:0]),
	   .rMXALU			(rMXALU[2:0]),
	   .rBRA			(rBRA),
	   .rDLY			(rDLY),
	   .rALT			(rALT[10:0]),
	   .rSTALL			(rSTALL),
	   .rSIMM			(rSIMM[31:0]),
	   .rIMM			(rIMM[15:0]),
	   .rOPC			(rOPC[5:0]),
	   .rRD				(rRD[4:0]),
	   .rDWBDI			(rDWBDI[31:0]),
	   .rPC				(rPC[31:2]),
	   .gclk			(gclk),
	   .grst			(grst),
	   .gena			(gena));
   
      
endmodule // aeMB_edk32

/*
 $Log: aeMB_edk32.v,v $
 Revision 1.14  2008/01/19 16:01:22  sybreon
 Patched problem where memory access followed by dual cycle instructions were not stalling correctly (submitted by M. Ettus)

 Revision 1.13  2007/12/25 22:15:09  sybreon
 Stalls pipeline on MUL/BSF instructions results in minor speed improvements.

 Revision 1.12  2007/12/23 20:40:44  sybreon
 Abstracted simulation kernel (aeMB_sim) to split simulation models from synthesis models.

 Revision 1.11  2007/11/30 17:08:29  sybreon
 Moved simulation kernel into code.
 
 Revision 1.10  2007/11/16 21:52:03  sybreon
 Added fsl_tag_o to FSL bus (tag either address or data).

 Revision 1.9  2007/11/14 23:19:24  sybreon
 Fixed minor typo.

 Revision 1.8  2007/11/14 22:14:34  sybreon
 Changed interrupt handling system (reported by M. Ettus).

 Revision 1.7  2007/11/10 16:39:38  sybreon
 Upgraded license to LGPLv3.
 Significant performance optimisations.

 Revision 1.6  2007/11/09 20:51:52  sybreon
 Added GET/PUT support through a FSL bus.

 Revision 1.5  2007/11/08 17:48:14  sybreon
 Fixed data WISHBONE arbitration problem (reported by J Lee).

 Revision 1.4  2007/11/08 14:17:47  sybreon
 Parameterised optional components.

 Revision 1.3  2007/11/03 08:34:55  sybreon
 Minor code cleanup.

 Revision 1.2  2007/11/02 19:20:58  sybreon
 Added better (beta) interrupt support.
 Changed MSR_IE to disabled at reset as per MB docs.

 Revision 1.1  2007/11/02 03:25:40  sybreon
 New EDK 3.2 compatible design with optional barrel-shifter and multiplier.
 Fixed various minor data hazard bugs.
 Code compatible with -O0/1/2/3/s generated code.
*/ 