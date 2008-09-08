// $Id: aeMB_ctrl.v,v 1.10 2007/11/30 16:44:40 sybreon Exp $
//
// AEMB CONTROL UNIT
// 
// Copyright (C) 2004-2007 Shawn Tan Ser Ngiap <shawn.tan@aeste.net>
//  
// This file is part of AEMB.
//
// AEMB is free software: you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// AEMB is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General
// Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with AEMB. If not, see <http://www.gnu.org/licenses/>.
//
// $Log: aeMB_ctrl.v,v $
// Revision 1.10  2007/11/30 16:44:40  sybreon
// Minor code cleanup.
//
// Revision 1.9  2007/11/15 09:26:43  sybreon
// Fixed minor typo causing synthesis failure.
//
// Revision 1.8  2007/11/14 23:19:24  sybreon
// Fixed minor typo.
//
// Revision 1.7  2007/11/14 22:14:34  sybreon
// Changed interrupt handling system (reported by M. Ettus).
//
// Revision 1.6  2007/11/10 16:39:38  sybreon
// Upgraded license to LGPLv3.
// Significant performance optimisations.
//
// Revision 1.5  2007/11/09 20:51:52  sybreon
// Added GET/PUT support through a FSL bus.
//
// Revision 1.4  2007/11/08 17:48:14  sybreon
// Fixed data WISHBONE arbitration problem (reported by J Lee).
//
// Revision 1.3  2007/11/08 14:17:47  sybreon
// Parameterised optional components.
//
// Revision 1.2  2007/11/02 19:20:58  sybreon
// Added better (beta) interrupt support.
// Changed MSR_IE to disabled at reset as per MB docs.
//
// Revision 1.1  2007/11/02 03:25:40  sybreon
// New EDK 3.2 compatible design with optional barrel-shifter and multiplier.
// Fixed various minor data hazard bugs.
// Code compatible with -O0/1/2/3/s generated code.
//

module aeMB_ctrl (/*AUTOARG*/
   // Outputs
   rMXDST, rMXSRC, rMXTGT, rMXALT, rMXALU, rRW, dwb_stb_o, dwb_wre_o,
   fsl_stb_o, fsl_wre_o,
   // Inputs
   rDLY, rIMM, rALT, rOPC, rRD, rRA, rRB, rPC, rBRA, rMSR_IE, xIREG,
   dwb_ack_i, iwb_ack_i, fsl_ack_i, gclk, grst, gena
   );
   // INTERNAL   
   //output [31:2] rPCLNK;
   output [1:0]  rMXDST;
   output [1:0]  rMXSRC, rMXTGT, rMXALT;
   output [2:0]  rMXALU;   
   output [4:0]  rRW;
   
   input 	 rDLY;
   input [15:0]  rIMM;
   input [10:0]  rALT;
   input [5:0] 	 rOPC;
   input [4:0] 	 rRD, rRA, rRB;
   input [31:2]  rPC;
   input 	 rBRA;
   input 	 rMSR_IE;
   input [31:0]  xIREG;   
   
   // DATA WISHBONE
   output 	 dwb_stb_o;
   output 	 dwb_wre_o;
   input 	 dwb_ack_i;

   // INST WISHBONE
   input 	 iwb_ack_i;
   
   // FSL WISHBONE
   output 	 fsl_stb_o;
   output 	 fsl_wre_o;
   input 	 fsl_ack_i;   
   
   // SYSTEM
   input 	 gclk, grst, gena;

   // --- DECODE INSTRUCTIONS
   // TODO: Simplify

   wire [5:0] 	 wOPC;
   wire [4:0] 	 wRD, wRA, wRB;
   wire [10:0] 	 wALT;   
   
   assign 	 {wOPC, wRD, wRA, wRB, wALT} = xIREG; // FIXME: Endian

   wire 	 fSFT = (rOPC == 6'o44);
   wire 	 fLOG = ({rOPC[5:4],rOPC[2]} == 3'o4);   

   wire 	 fMUL = (rOPC == 6'o20) | (rOPC == 6'o30);
   wire 	 fBSF = (rOPC == 6'o21) | (rOPC == 6'o31);
   wire 	 fDIV = (rOPC == 6'o22);   
   
   wire 	 fRTD = (rOPC == 6'o55);
   wire 	 fBCC = (rOPC == 6'o47) | (rOPC == 6'o57);
   wire 	 fBRU = (rOPC == 6'o46) | (rOPC == 6'o56);
   wire 	 fBRA = fBRU & rRA[3];   

   wire 	 fIMM = (rOPC == 6'o54);
   wire 	 fMOV = (rOPC == 6'o45);   
   
   wire 	 fLOD = ({rOPC[5:4],rOPC[2]} == 3'o6);
   wire 	 fSTR = ({rOPC[5:4],rOPC[2]} == 3'o7);
   wire 	 fLDST = (&rOPC[5:4]);   

   wire          fPUT = (rOPC == 6'o33) & rRB[4];
   wire 	 fGET = (rOPC == 6'o33) & !rRB[4];   


   wire 	 wSFT = (wOPC == 6'o44);
   wire 	 wLOG = ({wOPC[5:4],wOPC[2]} == 3'o4);   

   wire 	 wMUL = (wOPC == 6'o20) | (wOPC == 6'o30);
   wire 	 wBSF = (wOPC == 6'o21) | (wOPC == 6'o31);
   wire 	 wDIV = (wOPC == 6'o22);   
   
   wire 	 wRTD = (wOPC == 6'o55);
   wire 	 wBCC = (wOPC == 6'o47) | (wOPC == 6'o57);
   wire 	 wBRU = (wOPC == 6'o46) | (wOPC == 6'o56);
   wire 	 wBRA = wBRU & wRA[3];   

   wire 	 wIMM = (wOPC == 6'o54);
   wire 	 wMOV = (wOPC == 6'o45);   
   
   wire 	 wLOD = ({wOPC[5:4],wOPC[2]} == 3'o6);
   wire 	 wSTR = ({wOPC[5:4],wOPC[2]} == 3'o7);
   wire 	 wLDST = (&wOPC[5:4]);   

   wire          wPUT = (wOPC == 6'o33) & wRB[4];
   wire 	 wGET = (wOPC == 6'o33) & !wRB[4];   

   
   // --- BRANCH SLOT REGISTERS ---------------------------

   reg [31:2] 	 rPCLNK, xPCLNK;
   reg [1:0] 	 rMXDST, xMXDST;
   reg [4:0] 	 rRW, xRW;   

   reg [1:0] 	 rMXSRC, xMXSRC;
   reg [1:0] 	 rMXTGT, xMXTGT;
   reg [1:0] 	 rMXALT, xMXALT;
   
   
   // --- OPERAND SELECTOR ---------------------------------

   wire 	 wRDWE = |xRW;
   wire 	 wAFWD_M = (xRW == wRA) & (xMXDST == 2'o2) & wRDWE;
   wire 	 wBFWD_M = (xRW == wRB) & (xMXDST == 2'o2) & wRDWE;
   wire 	 wAFWD_R = (xRW == wRA) & (xMXDST == 2'o0) & wRDWE;   
   wire 	 wBFWD_R = (xRW == wRB) & (xMXDST == 2'o0) & wRDWE;

   always @(/*AUTOSENSE*/rBRA or wAFWD_M or wAFWD_R or wBCC or wBFWD_M
	    or wBFWD_R or wBRU or wOPC) 
     //if (rBRA | |rXCE) begin
     if (rBRA) begin
	/*AUTORESET*/
	// Beginning of autoreset for uninitialized flops
	xMXALT <= 2'h0;
	xMXSRC <= 2'h0;
	xMXTGT <= 2'h0;
	// End of automatics
     end else begin
	xMXSRC <= (wBRU | wBCC) ? 2'o3 : // PC
		  (wAFWD_M) ? 2'o2 : // RAM
		  (wAFWD_R) ? 2'o1 : // FWD
		  2'o0; // REG
	xMXTGT <= (wOPC[3]) ? 2'o3 : // IMM
		  (wBFWD_M) ? 2'o2 : // RAM
		  (wBFWD_R) ? 2'o1 : // FWD
		  2'o0; // REG
	xMXALT <= (wAFWD_M) ? 2'o2 : // RAM
		  (wAFWD_R) ? 2'o1 : // FWD
		  2'o0; // REG	
     end // else: !if(rBRA)
   
   // --- ALU CONTROL ---------------------------------------

   reg [2:0]     rMXALU, xMXALU;

   always @(/*AUTOSENSE*/rBRA or wBRA or wBSF or wDIV or wLOG or wMOV
	    or wMUL or wSFT)
     //if (rBRA | |rXCE) begin
     if (rBRA) begin
	/*AUTORESET*/
	// Beginning of autoreset for uninitialized flops
	xMXALU <= 3'h0;
	// End of automatics
     end else begin
	xMXALU <= (wBRA | wMOV) ? 3'o3 :
		  (wSFT) ? 3'o2 :
		  (wLOG) ? 3'o1 :
		  (wMUL) ? 3'o4 :
		  (wBSF) ? 3'o5 :
		  (wDIV) ? 3'o6 :
		  3'o0;      	
     end // else: !if(rBRA)
   
   // --- DELAY SLOT REGISTERS ------------------------------
   
   wire 	 fSKIP = (rBRA & !rDLY);
   
   always @(/*AUTOSENSE*/fBCC or fBRU or fGET or fLOD or fRTD or fSKIP
	    or fSTR or rRD)
     if (fSKIP) begin
	/*AUTORESET*/
	// Beginning of autoreset for uninitialized flops
	xMXDST <= 2'h0;
	xRW <= 5'h0;
	// End of automatics
     end else begin
	xMXDST <= (fSTR | fRTD | fBCC) ? 2'o3 :
		  (fLOD | fGET) ? 2'o2 :
		  (fBRU) ? 2'o1 :
		  2'o0;
	xRW <= rRD;
     end // else: !if(fSKIP)


   // --- DATA WISHBONE ----------------------------------

   wire 	 fDACK = !(dwb_stb_o ^ dwb_ack_i);
   
   reg 		 rDWBSTB, xDWBSTB;
   reg 		 rDWBWRE, xDWBWRE;

   assign 	 dwb_stb_o = rDWBSTB;
   assign 	 dwb_wre_o = rDWBWRE;
   
   
   always @(/*AUTOSENSE*/fLOD or fSKIP or fSTR or iwb_ack_i)
     //if (fSKIP | |rXCE) begin
     if (fSKIP) begin
	/*AUTORESET*/
	// Beginning of autoreset for uninitialized flops
	xDWBSTB <= 1'h0;
	xDWBWRE <= 1'h0;
	// End of automatics
     end else begin
	xDWBSTB <= (fLOD | fSTR) & iwb_ack_i;
	xDWBWRE <= fSTR & iwb_ack_i;	
     end
   
   always @(posedge gclk)
     if (grst) begin
	/*AUTORESET*/
	// Beginning of autoreset for uninitialized flops
	rDWBSTB <= 1'h0;
	rDWBWRE <= 1'h0;
	// End of automatics
     end else if (fDACK) begin
	rDWBSTB <= #1 xDWBSTB;
	rDWBWRE <= #1 xDWBWRE;	
     end
   

   // --- FSL WISHBONE -----------------------------------

   wire 	 fFACK = !(fsl_stb_o ^ fsl_ack_i);   
	 
   reg 		 rFSLSTB, xFSLSTB;
   reg 		 rFSLWRE, xFSLWRE;

   assign 	 fsl_stb_o = rFSLSTB;
   assign 	 fsl_wre_o = rFSLWRE;   

   always @(/*AUTOSENSE*/fGET or fPUT or fSKIP or iwb_ack_i) 
     //if (fSKIP | |rXCE) begin
     if (fSKIP) begin
	/*AUTORESET*/
	// Beginning of autoreset for uninitialized flops
	xFSLSTB <= 1'h0;
	xFSLWRE <= 1'h0;
	// End of automatics
     end else begin
	xFSLSTB <= (fPUT | fGET) & iwb_ack_i;
	xFSLWRE <= fPUT & iwb_ack_i;	
     end

   always @(posedge gclk)
     if (grst) begin
	/*AUTORESET*/
	// Beginning of autoreset for uninitialized flops
	rFSLSTB <= 1'h0;
	rFSLWRE <= 1'h0;
	// End of automatics
     end else if (fFACK) begin
	rFSLSTB <= #1 xFSLSTB;
	rFSLWRE <= #1 xFSLWRE;	
     end
   
   // --- PIPELINE CONTROL DELAY ----------------------------

   always @(posedge gclk)
     if (grst) begin
	/*AUTORESET*/
	// Beginning of autoreset for uninitialized flops
	rMXALT <= 2'h0;
	rMXALU <= 3'h0;
	rMXDST <= 2'h0;
	rMXSRC <= 2'h0;
	rMXTGT <= 2'h0;
	rRW <= 5'h0;
	// End of automatics
     end else if (gena) begin // if (grst)
	//rPCLNK <= #1 xPCLNK;
	rMXDST <= #1 xMXDST;
	rRW <= #1 xRW;
	rMXSRC <= #1 xMXSRC;
	rMXTGT <= #1 xMXTGT;
	rMXALT <= #1 xMXALT;	
	rMXALU <= #1 xMXALU;	
     end

   
endmodule // aeMB_ctrl
