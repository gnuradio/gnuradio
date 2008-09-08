// $Id: aeMB_core.v,v 1.9 2007/11/23 14:06:41 sybreon Exp $
//
// AEMB 32'bit RISC MICROPROCESSOR CORE
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
// HISTORY
// $Log: aeMB_core.v,v $
// Revision 1.9  2007/11/23 14:06:41  sybreon
// Old version deprecated.
//
// Revision 1.8  2007/10/22 19:12:59  sybreon
// Made some changes to the interrupt control. In some cases, the interrupt logic waits forever and doesn't execute. Bug was discovered by M. Ettus.
//
// Revision 1.7  2007/05/30 18:44:30  sybreon
// Added interrupt support.
//
// Revision 1.6  2007/05/17 09:08:21  sybreon
// Removed asynchronous reset signal.
//
// Revision 1.5  2007/04/27 00:23:55  sybreon
// Added code documentation.
// Improved size & speed of rtl/verilog/aeMB_aslu.v
//
// Revision 1.4  2007/04/25 22:15:04  sybreon
// Added support for 8-bit and 16-bit data types.
//
// Revision 1.3  2007/04/11 04:30:43  sybreon
// Added pipeline stalling from incomplete bus cycles.
// Separated sync and async portions of code.
//
// Revision 1.2  2007/04/04 06:13:23  sybreon
// Removed unused signals
//
// Revision 1.1  2007/03/09 17:52:17  sybreon
// initial import
//


module aeMB_core (/*AUTOARG*/
   // Outputs
   iwb_stb_o, iwb_adr_o, fsl_wre_o, fsl_tag_o, fsl_stb_o, fsl_dat_o,
   fsl_adr_o, dwb_wre_o, dwb_stb_o, dwb_sel_o, dwb_dat_o, dwb_adr_o,
   // Inputs
   sys_rst_i, sys_int_i, sys_clk_i, iwb_dat_i, iwb_ack_i, fsl_dat_i,
   fsl_ack_i, dwb_dat_i, dwb_ack_i
   );
   // Instruction WB address space
   parameter ISIZ = 32;
   // Data WB address space
   parameter DSIZ = 32;
   // Multiplier
   parameter MUL = 1;
   // Barrel Shifter
   parameter BSF = 1;   

   /*AUTOOUTPUT*/
   // Beginning of automatic outputs (from unused autoinst outputs)
   output [DSIZ-1:2]	dwb_adr_o;		// From edk32 of aeMB_edk32.v
   output [31:0]	dwb_dat_o;		// From edk32 of aeMB_edk32.v
   output [3:0]		dwb_sel_o;		// From edk32 of aeMB_edk32.v
   output		dwb_stb_o;		// From edk32 of aeMB_edk32.v
   output		dwb_wre_o;		// From edk32 of aeMB_edk32.v
   output [6:2]		fsl_adr_o;		// From edk32 of aeMB_edk32.v
   output [31:0]	fsl_dat_o;		// From edk32 of aeMB_edk32.v
   output		fsl_stb_o;		// From edk32 of aeMB_edk32.v
   output [1:0]		fsl_tag_o;		// From edk32 of aeMB_edk32.v
   output		fsl_wre_o;		// From edk32 of aeMB_edk32.v
   output [ISIZ-1:2]	iwb_adr_o;		// From edk32 of aeMB_edk32.v
   output		iwb_stb_o;		// From edk32 of aeMB_edk32.v
   // End of automatics
   /*AUTOINPUT*/
   // Beginning of automatic inputs (from unused autoinst inputs)
   input		dwb_ack_i;		// To edk32 of aeMB_edk32.v
   input [31:0]		dwb_dat_i;		// To edk32 of aeMB_edk32.v
   input		fsl_ack_i;		// To edk32 of aeMB_edk32.v
   input [31:0]		fsl_dat_i;		// To edk32 of aeMB_edk32.v
   input		iwb_ack_i;		// To edk32 of aeMB_edk32.v
   input [31:0]		iwb_dat_i;		// To edk32 of aeMB_edk32.v
   input		sys_clk_i;		// To edk32 of aeMB_edk32.v
   input		sys_int_i;		// To edk32 of aeMB_edk32.v
   input		sys_rst_i;		// To edk32 of aeMB_edk32.v
   // End of automatics
   /*AUTOWIRE*/

   // INSTANTIATIONS /////////////////////////////////////////////////////////////////

   /* 
    aeMB_edk32 AUTO_TEMPLATE (
    .dwb_adr_o(dwb_adr_o[DSIZ-1:2]),
    .iwb_adr_o(iwb_adr_o[ISIZ-1:2]),
    );
    */
   
   aeMB_edk32 #(ISIZ, DSIZ, MUL, BSF)
   edk32 (/*AUTOINST*/
	  // Outputs
	  .dwb_adr_o			(dwb_adr_o[DSIZ-1:2]),	 // Templated
	  .dwb_dat_o			(dwb_dat_o[31:0]),
	  .dwb_sel_o			(dwb_sel_o[3:0]),
	  .dwb_stb_o			(dwb_stb_o),
	  .dwb_wre_o			(dwb_wre_o),
	  .fsl_adr_o			(fsl_adr_o[6:2]),
	  .fsl_dat_o			(fsl_dat_o[31:0]),
	  .fsl_stb_o			(fsl_stb_o),
	  .fsl_tag_o			(fsl_tag_o[1:0]),
	  .fsl_wre_o			(fsl_wre_o),
	  .iwb_adr_o			(iwb_adr_o[ISIZ-1:2]),	 // Templated
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
   
   
endmodule // aeMB_core
