// $Id: aeMB_bpcu.v,v 1.4 2007/11/14 22:14:34 sybreon Exp $
//
// AEMB BRANCH PROGRAMME COUNTER UNIT
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
// $Log: aeMB_bpcu.v,v $
// Revision 1.4  2007/11/14 22:14:34  sybreon
// Changed interrupt handling system (reported by M. Ettus).
//
// Revision 1.3  2007/11/10 16:39:38  sybreon
// Upgraded license to LGPLv3.
// Significant performance optimisations.
//
// Revision 1.2  2007/11/02 19:20:58  sybreon
// Added better (beta) interrupt support.
// Changed MSR_IE to disabled at reset as per MB docs.
//
// Revision 1.1  2007/11/02 03:25:39  sybreon
// New EDK 3.2 compatible design with optional barrel-shifter and multiplier.
// Fixed various minor data hazard bugs.
// Code compatible with -O0/1/2/3/s generated code.
//

module aeMB_bpcu (/*AUTOARG*/
   // Outputs
   iwb_adr_o, rPC, rPCLNK, rBRA, rDLY,
   // Inputs
   rMXALT, rOPC, rRD, rRA, rRESULT, rDWBDI, rREGA, gclk, grst, gena
   );
   parameter IW = 24;

   // INST WISHBONE
   output [IW-1:2] iwb_adr_o;

   // INTERNAL
   output [31:2]   rPC, rPCLNK;
   output 	   rBRA;
   output 	   rDLY;
   //output [1:0]    rATOM;
   //output [1:0]    xATOM;
   
   input [1:0] 	   rMXALT;   
   input [5:0] 	   rOPC;
   input [4:0] 	   rRD, rRA;  
   input [31:0]    rRESULT; // ALU
   input [31:0]    rDWBDI; // RAM
   input [31:0]    rREGA;
   //input [1:0] 	   rXCE;   
   
   // SYSTEM
   input 	   gclk, grst, gena;

   // --- BRANCH CONTROL --------------------------------------------
   // Controls the branch and delay flags
   
   wire 	   fRTD = (rOPC == 6'o55);
   wire 	   fBCC = (rOPC == 6'o47) | (rOPC == 6'o57);
   wire 	   fBRU = (rOPC == 6'o46) | (rOPC == 6'o56);

   wire [31:0] 	   wREGA;
   assign 	   wREGA = (rMXALT == 2'o2) ? rDWBDI :
			   (rMXALT == 2'o1) ? rRESULT :
			   rREGA;   
   
   wire 	   wBEQ = (wREGA == 32'd0);
   wire 	   wBNE = ~wBEQ;
   wire 	   wBLT = wREGA[31];
   wire 	   wBLE = wBLT | wBEQ;   
   wire 	   wBGE = ~wBLT;
   wire 	   wBGT = ~wBLE;   

   reg 		   xXCC;
   always @(/*AUTOSENSE*/rRD or wBEQ or wBGE or wBGT or wBLE or wBLT
	    or wBNE)
     case (rRD[2:0])
       3'o0: xXCC <= wBEQ;
       3'o1: xXCC <= wBNE;
       3'o2: xXCC <= wBLT;
       3'o3: xXCC <= wBLE;
       3'o4: xXCC <= wBGT;
       3'o5: xXCC <= wBGE;
       default: xXCC <= 1'bX;
     endcase // case (rRD[2:0])

   reg 		   rBRA, xBRA;
   reg 		   rDLY, xDLY;
   wire 	   fSKIP = rBRA & !rDLY;   
   
   always @(/*AUTOSENSE*/fBCC or fBRU or fRTD or rBRA or rRA or rRD
	    or xXCC)
     //if (rBRA | |rXCE) begin
     if (rBRA) begin
	/*AUTORESET*/
	// Beginning of autoreset for uninitialized flops
	xBRA <= 1'h0;
	xDLY <= 1'h0;
	// End of automatics
     end else begin
	xDLY <= (fBRU & rRA[4]) | (fBCC & rRD[4]) | fRTD;      
	xBRA <= (fRTD | fBRU) ? 1'b1 :
		(fBCC) ? xXCC :
		1'b0;
     end

   // --- PC PIPELINE ------------------------------------------------
   // PC and related changes
   
   reg [31:2] 	   rIPC, xIPC;
   reg [31:2] 	   rPC, xPC;
   reg [31:2] 	   rPCLNK, xPCLNK;
   
   assign 	   iwb_adr_o = rIPC[IW-1:2];
   
   always @(/*AUTOSENSE*/rBRA or rIPC or rPC or rRESULT) begin
      //xPCLNK <= (^rATOM) ? rPC : rPC;
      xPCLNK <= rPC;
      //xPC <= (^rATOM) ? rIPC : rRESULT[31:2];	
      xPC <= rIPC;
      //xIPC <= (rBRA) ? rRESULT[31:2] : (rIPC + 1);
      /*
     case (rXCE)
       2'o1: xIPC <= 30'h2;       
       2'o2: xIPC <= 30'h4;       
       2'o3: xIPC <= 30'h6;       
       default: xIPC <= (rBRA) ? rRESULT[31:2] : (rIPC + 1);
     endcase // case (rXCE)      
       */
      xIPC <= (rBRA) ? rRESULT[31:2] : (rIPC + 1);
   end   			   

   // --- ATOMIC CONTROL ---------------------------------------------
   // This is used to indicate 'safe' instruction borders.
   
   wire 	wIMM = (rOPC == 6'o54) & !fSKIP;
   wire 	wRTD = (rOPC == 6'o55) & !fSKIP;
   wire 	wBCC = xXCC & ((rOPC == 6'o47) | (rOPC == 6'o57)) & !fSKIP;
   wire 	wBRU = ((rOPC == 6'o46) | (rOPC == 6'o56)) & !fSKIP;   
   
   wire 	fATOM = ~(wIMM | wRTD | wBCC | wBRU | rBRA);   
   reg [1:0] 	rATOM, xATOM;

   always @(/*AUTOSENSE*/fATOM or rATOM)
     xATOM <= {rATOM[0], (rATOM[0] ^ fATOM)};   
     
   
   // --- SYNC PIPELINE ----------------------------------------------
    
   always @(posedge gclk)
     if (grst) begin
	/*AUTORESET*/
	// Beginning of autoreset for uninitialized flops
	rATOM <= 2'h0;
	rBRA <= 1'h0;
	rDLY <= 1'h0;
	rIPC <= 30'h0;
	rPC <= 30'h0;
	rPCLNK <= 30'h0;
	// End of automatics
     end else if (gena) begin
	rIPC <= #1 xIPC;
	rBRA <= #1 xBRA;
	rPC <= #1 xPC;
	rPCLNK <= #1 xPCLNK;
	rDLY <= #1 xDLY;
	rATOM <= #1 xATOM;	
     end
      
endmodule // aeMB_bpcu
