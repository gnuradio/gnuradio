/* $Id: aeMB_sim.v,v 1.2 2008/06/06 09:36:02 sybreon Exp $
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

module aeMB_sim (/*AUTOARG*/
   // Outputs
   iwb_stb_o, iwb_adr_o, fsl_wre_o, fsl_tag_o, fsl_stb_o, fsl_dat_o,
   fsl_adr_o, dwb_wre_o, dwb_stb_o, dwb_sel_o, dwb_dat_o, dwb_adr_o,
   // Inputs
   sys_rst_i, sys_int_i, sys_clk_i, iwb_dat_i, iwb_ack_i, fsl_dat_i,
   fsl_ack_i, dwb_dat_i, dwb_ack_i
   );
   // Bus widths
   parameter IW = 32; /// Instruction bus address width
   parameter DW = 32; /// Data bus address width

   // Optional functions
   parameter MUL = 1; // Multiplier
   parameter BSF = 1; // Barrel Shifter
      
   /*AUTOOUTPUT*/
   // Beginning of automatic outputs (from unused autoinst outputs)
   output [DW-1:2]	dwb_adr_o;		// From cpu of aeMB_edk32.v
   output [31:0]	dwb_dat_o;		// From cpu of aeMB_edk32.v
   output [3:0]		dwb_sel_o;		// From cpu of aeMB_edk32.v
   output		dwb_stb_o;		// From cpu of aeMB_edk32.v
   output		dwb_wre_o;		// From cpu of aeMB_edk32.v
   output [6:2]		fsl_adr_o;		// From cpu of aeMB_edk32.v
   output [31:0]	fsl_dat_o;		// From cpu of aeMB_edk32.v
   output		fsl_stb_o;		// From cpu of aeMB_edk32.v
   output [1:0]		fsl_tag_o;		// From cpu of aeMB_edk32.v
   output		fsl_wre_o;		// From cpu of aeMB_edk32.v
   output [IW-1:2]	iwb_adr_o;		// From cpu of aeMB_edk32.v
   output		iwb_stb_o;		// From cpu of aeMB_edk32.v
   // End of automatics
   /*AUTOINPUT*/
   // Beginning of automatic inputs (from unused autoinst inputs)
   input		dwb_ack_i;		// To cpu of aeMB_edk32.v
   input [31:0]		dwb_dat_i;		// To cpu of aeMB_edk32.v
   input		fsl_ack_i;		// To cpu of aeMB_edk32.v
   input [31:0]		fsl_dat_i;		// To cpu of aeMB_edk32.v
   input		iwb_ack_i;		// To cpu of aeMB_edk32.v
   input [31:0]		iwb_dat_i;		// To cpu of aeMB_edk32.v
   input		sys_clk_i;		// To cpu of aeMB_edk32.v
   input		sys_int_i;		// To cpu of aeMB_edk32.v
   input		sys_rst_i;		// To cpu of aeMB_edk32.v
   // End of automatics
   /*AUTOWIRE*/

   aeMB_edk32
     #(/*AUTOINSTPARAM*/
       // Parameters
       .IW				(IW),
       .DW				(DW),
       .MUL				(MUL),
       .BSF				(BSF))
   cpu
     (/*AUTOINST*/
      // Outputs
      .dwb_adr_o			(dwb_adr_o[DW-1:2]),
      .dwb_dat_o			(dwb_dat_o[31:0]),
      .dwb_sel_o			(dwb_sel_o[3:0]),
      .dwb_stb_o			(dwb_stb_o),
      .dwb_wre_o			(dwb_wre_o),
      .fsl_adr_o			(fsl_adr_o[6:2]),
      .fsl_dat_o			(fsl_dat_o[31:0]),
      .fsl_stb_o			(fsl_stb_o),
      .fsl_tag_o			(fsl_tag_o[1:0]),
      .fsl_wre_o			(fsl_wre_o),
      .iwb_adr_o			(iwb_adr_o[IW-1:2]),
      .iwb_stb_o			(iwb_stb_o),
      // Inputs
      .dwb_ack_i			(dwb_ack_i),
      .dwb_dat_i			(dwb_dat_i[31:0]),
      .fsl_ack_i			(fsl_ack_i),
      .fsl_dat_i			(fsl_dat_i[31:0]),
      .iwb_ack_i			(iwb_ack_i),
      .iwb_dat_i			(iwb_dat_i[31:0]),
      .sys_int_i			(sys_int_i),
      .sys_clk_i			(sys_clk_i),
      .sys_rst_i			(sys_rst_i));
   
   // --- SIMULATION KERNEL ----------------------------------
   // synopsys translate_off
   
   wire [IW-1:0] 	iwb_adr = {iwb_adr_o, 2'd0};
   wire [DW-1:0] 	dwb_adr = {dwb_adr_o,2'd0};   
   wire [1:0] 		wBRA = {cpu.rBRA, cpu.rDLY};   
   wire [3:0] 		wMSR = {cpu.xecu.rMSR_BIP, cpu.xecu.rMSR_C, cpu.xecu.rMSR_IE, cpu.xecu.rMSR_BE};


   `ifdef AEMB_SIM_KERNEL
   always @(posedge cpu.gclk) begin
      if (cpu.gena) begin
	 
	 $write ("\n", ($stime/10));
	 $writeh (" PC=", iwb_adr );
	 $writeh ("\t");
	 
	 case (wBRA)
	   2'b00: $write(" ");
	   2'b01: $write(".");	
	   2'b10: $write("-");
	   2'b11: $write("+");	
	 endcase // case (cpu.wBRA)
      
	 case (cpu.rOPC)
	   6'o00: if (cpu.rRD == 0) $write("   "); else $write("ADD");
	   6'o01: $write("RSUB");	
	   6'o02: $write("ADDC");	
	   6'o03: $write("RSUBC");	
	   6'o04: $write("ADDK");	
	   6'o05: case (cpu.rIMM[1:0])
		    2'o0: $write("RSUBK");	
		    2'o1: $write("CMP");	
		    2'o3: $write("CMPU");	
		    default: $write("XXX");
		  endcase // case (cpu.rIMM[1:0])
	   6'o06: $write("ADDKC");	
	   6'o07: $write("RSUBKC");	
	   
	   6'o10: $write("ADDI");	
	   6'o11: $write("RSUBI");	
	   6'o12: $write("ADDIC");	
	   6'o13: $write("RSUBIC");	
	   6'o14: $write("ADDIK");	
	   6'o15: $write("RSUBIK");	
	   6'o16: $write("ADDIKC");	
	   6'o17: $write("RSUBIKC");	
	   
	   6'o20: $write("MUL");	
	   6'o21: case (cpu.rALT[10:9])
		    2'o0: $write("BSRL");		 
		    2'o1: $write("BSRA");		 
		    2'o2: $write("BSLL");		 
		    default: $write("XXX");		 
		  endcase // case (cpu.rALT[10:9])
	   6'o22: $write("IDIV");	
	   
	   6'o30: $write("MULI");	
	   6'o31: case (cpu.rALT[10:9])
		    2'o0: $write("BSRLI");		 
		    2'o1: $write("BSRAI");		 
		    2'o2: $write("BSLLI");		 
		    default: $write("XXX");		 
		  endcase // case (cpu.rALT[10:9])
	   6'o33: case (cpu.rRB[4:2])
		    3'o0: $write("GET");
		    3'o4: $write("PUT");		 
		    3'o2: $write("NGET");
		    3'o6: $write("NPUT");		 
		    3'o1: $write("CGET");
		    3'o5: $write("CPUT");		 
		    3'o3: $write("NCGET");
		    3'o7: $write("NCPUT");		 
		  endcase // case (cpu.rRB[4:2])
	   
	   6'o40: $write("OR");
	   6'o41: $write("AND");	
	   6'o42: if (cpu.rRD == 0) $write("   "); else $write("XOR");
	   6'o43: $write("ANDN");	
	   6'o44: case (cpu.rIMM[6:5])
		    2'o0: $write("SRA");
		    2'o1: $write("SRC");
		    2'o2: $write("SRL");
		    2'o3: if (cpu.rIMM[0]) $write("SEXT16"); else $write("SEXT8");		 
		  endcase // case (cpu.rIMM[6:5])
	   
	   6'o45: $write("MOV");	
	   6'o46: case (cpu.rRA[3:2])
		    3'o0: $write("BR");		 
		    3'o1: $write("BRL");		 
		    3'o2: $write("BRA");		 
		    3'o3: $write("BRAL");		 
		  endcase // case (cpu.rRA[3:2])
	   
	   6'o47: case (cpu.rRD[2:0])
		    3'o0: $write("BEQ");	
		    3'o1: $write("BNE");	
		    3'o2: $write("BLT");	
		    3'o3: $write("BLE");	
		    3'o4: $write("BGT");	
		    3'o5: $write("BGE");
		    default: $write("XXX");		 
		  endcase // case (cpu.rRD[2:0])
	   
	   6'o50: $write("ORI");	
	   6'o51: $write("ANDI");	
	   6'o52: $write("XORI");	
	   6'o53: $write("ANDNI");	
	   6'o54: $write("IMMI");	
	   6'o55: case (cpu.rRD[1:0])
		    2'o0: $write("RTSD");
		    2'o1: $write("RTID");
		    2'o2: $write("RTBD");
		    default: $write("XXX");		 
		  endcase // case (cpu.rRD[1:0])
	   6'o56: case (cpu.rRA[3:2])
		    3'o0: $write("BRI");		 
		    3'o1: $write("BRLI");		 
		    3'o2: $write("BRAI");		 
		    3'o3: $write("BRALI");		 
		  endcase // case (cpu.rRA[3:2])
	   6'o57: case (cpu.rRD[2:0])
		    3'o0: $write("BEQI");	
		    3'o1: $write("BNEI");	
		    3'o2: $write("BLTI");	
		    3'o3: $write("BLEI");	
		    3'o4: $write("BGTI");	
		    3'o5: $write("BGEI");	
		    default: $write("XXX");		 
		  endcase // case (cpu.rRD[2:0])
	   
	   6'o60: $write("LBU");	
	   6'o61: $write("LHU");	
	   6'o62: $write("LW");	
	   6'o64: $write("SB");	
	   6'o65: $write("SH");	
	   6'o66: $write("SW");	
	   
	   6'o70: $write("LBUI");	
	   6'o71: $write("LHUI");	
	   6'o72: $write("LWI");	
	   6'o74: $write("SBI");	
	   6'o75: $write("SHI");	
	   6'o76: $write("SWI");
	   
	   default: $write("XXX");	
	 endcase // case (cpu.rOPC)
	 
	 case (cpu.rOPC[3])
	   1'b1: $writeh("\tr",cpu.rRD,", r",cpu.rRA,", h",cpu.rIMM);
	   1'b0: $writeh("\tr",cpu.rRD,", r",cpu.rRA,", r",cpu.rRB,"  ");	
	 endcase // case (cpu.rOPC[3])
	 
	 
	 // ALU
	 $write("\t");
	 $writeh(" A=",cpu.xecu.rOPA);
	 $writeh(" B=",cpu.xecu.rOPB);
	 
	 case (cpu.rMXALU)
	   3'o0: $write(" ADD");
	   3'o1: $write(" LOG");
	   3'o2: $write(" SFT");
	   3'o3: $write(" MOV");
	   3'o4: $write(" MUL");
	   3'o5: $write(" BSF");
	   default: $write(" XXX");
	 endcase // case (cpu.rMXALU)
	 $writeh("=h",cpu.xecu.xRESULT);
	 
	 // WRITEBACK
	 $writeh("\tSR=", wMSR," ");
	 
	 if (cpu.regf.fRDWE) begin
	    case (cpu.rMXDST)
	      2'o2: begin
		 if (dwb_stb_o) $writeh("R",cpu.rRW,"=RAM(h",cpu.regf.xWDAT,")");
		 if (fsl_stb_o) $writeh("R",cpu.rRW,"=FSL(h",cpu.regf.xWDAT,")");
	      end
	      2'o1: $writeh("R",cpu.rRW,"=LNK(h",cpu.regf.xWDAT,")");
	      2'o0: $writeh("R",cpu.rRW,"=ALU(h",cpu.regf.xWDAT,")");
	    endcase // case (cpu.rMXDST)
	 end
	 
	 // STORE
	 if (dwb_stb_o & dwb_wre_o) begin
	    $writeh("RAM(", dwb_adr ,")=", dwb_dat_o);
	    case (dwb_sel_o)
	      4'hF: $write(":L");
	      4'h3,4'hC: $write(":W");
	      4'h1,4'h2,4'h4,4'h8: $write(":B");
	    endcase // case (dwb_sel_o)
	    
	 end
	 
      end // if (cpu.gena)
      
   end // always @ (posedge cpu.gclk)
   `endif //  `ifdef AEMB_SIM_KERNEL
   
   // synopsys translate_on
   
endmodule // aeMB_sim

/* 
 $Log: aeMB_sim.v,v $
 Revision 1.2  2008/06/06 09:36:02  sybreon
 single thread design

 Revision 1.1  2007/12/23 20:40:45  sybreon
 Abstracted simulation kernel (aeMB_sim) to split simulation models from synthesis models.
 
 */