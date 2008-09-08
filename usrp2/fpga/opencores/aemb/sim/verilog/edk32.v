/* $Id: edk32.v,v 1.12 2007/12/23 20:40:51 sybreon Exp $
**
** AEMB EDK 3.2 Compatible Core TEST
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

`define AEMB_SIMULATION_KERNEL   

module edk32 ();

`include "random.v"
  
   // INITIAL SETUP //////////////////////////////////////////////////////
   
   reg 	     sys_clk_i, sys_rst_i, sys_int_i, sys_exc_i;
   reg 	     svc;
   integer   inttime;
   integer   seed;   
   integer   theend;
   
   always #5 sys_clk_i = ~sys_clk_i;   

   initial begin
      //$dumpfile("dump.vcd");
      //$dumpvars(1,dut);
   end
   
   initial begin
      seed = randseed;
      theend = 0;      
      svc = 0;      
      sys_clk_i = $random(seed);
      sys_rst_i = 1;
      sys_int_i = 0;
      sys_exc_i = 0;      
      #50 sys_rst_i = 0;
   end

   initial fork
      //inttime $display("FSADFASDFSDAF");      
      //#10000 sys_int_i = 1;
      //#1100 sys_int_i = 0;
      //#100000 $displayh("\nTest Completed."); 
      //#4000 $finish;
   join   

   
   // FAKE MEMORY ////////////////////////////////////////////////////////

   wire        fsl_stb_o;
   wire        fsl_wre_o;
   wire [31:0] fsl_dat_o;
   wire [31:0] fsl_dat_i;   
   wire [6:2]  fsl_adr_o;
   
   wire [15:2] iwb_adr_o;
   wire        iwb_stb_o;
   wire        dwb_stb_o;
   reg [31:0]  rom [0:65535];
   wire [31:0] iwb_dat_i;
   reg 	       iwb_ack_i, dwb_ack_i, fsl_ack_i;

   reg [31:0]  ram[0:65535];
   wire [31:0] dwb_dat_i;
   reg [31:0]  dwblat;
   wire        dwb_we_o;
   reg [15:2]  dadr,iadr;
   wire [3:0]  dwb_sel_o; 
   wire [31:0] dwb_dat_o;
   wire [15:2] dwb_adr_o;
   wire [31:0] dwb_dat_t;

   initial begin
      dwb_ack_i = 0;
      iwb_ack_i = 0;
      fsl_ack_i = 0;      
   end
   
   assign      dwb_dat_t = ram[dwb_adr_o];   
   assign      iwb_dat_i = ram[iadr]; 
   assign      dwb_dat_i = ram[dadr];     
   assign      fsl_dat_i = fsl_adr_o;   

`ifdef POSEDGE
   
   always @(posedge sys_clk_i) 
     if (sys_rst_i) begin
	/*AUTORESET*/
	// Beginning of autoreset for uninitialized flops
	dwb_ack_i <= 1'h0;
	fsl_ack_i <= 1'h0;
	iwb_ack_i <= 1'h0;
	// End of automatics
     end else begin
	iwb_ack_i <= #1 iwb_stb_o ^ iwb_ack_i;      
	dwb_ack_i <= #1 dwb_stb_o ^ dwb_ack_i;
	fsl_ack_i <= #1 fsl_stb_o ^ fsl_ack_i;
     end // else: !if(sys_rst_i)
   
   always @(posedge sys_clk_i) begin
      iadr <= #1 iwb_adr_o;      
      dadr <= #1 dwb_adr_o;
      
      if (dwb_we_o & dwb_stb_o) begin
	 case (dwb_sel_o)
	   4'h1: ram[dwb_adr_o] <= {dwb_dat_t[31:8], dwb_dat_o[7:0]};
	   4'h2: ram[dwb_adr_o] <= {dwb_dat_t[31:16], dwb_dat_o[15:8], dwb_dat_t[7:0]};
	   4'h4: ram[dwb_adr_o] <= {dwb_dat_t[31:24], dwb_dat_o[23:16], dwb_dat_t[15:0]};
	   4'h8: ram[dwb_adr_o] <= {dwb_dat_o[31:24], dwb_dat_t[23:0]};
	   4'h3: ram[dwb_adr_o] <= {dwb_dat_t[31:16], dwb_dat_o[15:0]};
	   4'hC: ram[dwb_adr_o] <= {dwb_dat_o[31:16], dwb_dat_t[15:0]};
	   4'hF: ram[dwb_adr_o] <= {dwb_dat_o};
	 endcase // case (dwb_sel_o)
      end // if (dwb_we_o & dwb_stb_o)
   end // always @ (posedge sys_clk_i)

`else // !`ifdef POSEDGE
   
   always @(negedge sys_clk_i) 
     if (sys_rst_i) begin
	/*AUTORESET*/
	// Beginning of autoreset for uninitialized flops
	dwb_ack_i <= 1'h0;
	fsl_ack_i <= 1'h0;
	iwb_ack_i <= 1'h0;
	// End of automatics
     end else begin
	iwb_ack_i <= #1 iwb_stb_o;      
	dwb_ack_i <= #1 dwb_stb_o;
	fsl_ack_i <= #1 fsl_stb_o;
     end // else: !if(sys_rst_i)
   
   always @(negedge sys_clk_i) begin
      iadr <= #1 iwb_adr_o;      
      dadr <= #1 dwb_adr_o;
      
      if (dwb_we_o & dwb_stb_o) begin
	 case (dwb_sel_o)
	   4'h1: ram[dwb_adr_o] <= {dwb_dat_t[31:8], dwb_dat_o[7:0]};
	   4'h2: ram[dwb_adr_o] <= {dwb_dat_t[31:16], dwb_dat_o[15:8], dwb_dat_t[7:0]};
	   4'h4: ram[dwb_adr_o] <= {dwb_dat_t[31:24], dwb_dat_o[23:16], dwb_dat_t[15:0]};
	   4'h8: ram[dwb_adr_o] <= {dwb_dat_o[31:24], dwb_dat_t[23:0]};
	   4'h3: ram[dwb_adr_o] <= {dwb_dat_t[31:16], dwb_dat_o[15:0]};
	   4'hC: ram[dwb_adr_o] <= {dwb_dat_o[31:16], dwb_dat_t[15:0]};
	   4'hF: ram[dwb_adr_o] <= {dwb_dat_o};
	 endcase // case (dwb_sel_o)
      end // if (dwb_we_o & dwb_stb_o)
   end // always @ (negedge sys_clk_i)
   
`endif // !`ifdef POSEDGE
   

   integer i;   
   initial begin
      for (i=0;i<65535;i=i+1) begin
	 ram[i] <= $random;
      end
      #1 $readmemh("dump.vmem",ram);
   end

   // DISPLAY OUTPUTS ///////////////////////////////////////////////////

   integer rnd;
   
   always @(posedge sys_clk_i) begin

      // Interrupt Monitors
      if (!dut.cpu.rMSR_IE) begin
	 rnd = $random % 30;	 
	 inttime = $stime + 1000 + (rnd*rnd * 10);
      end
      if ($stime > inttime) begin
	 sys_int_i = 1;
	 svc = 0;	 
      end
      if (($stime > inttime + 500) && !svc) begin
	 $display("\n\t*** INTERRUPT TIMEOUT ***", inttime);	 
	 $finish;	 
      end
      if (dwb_we_o & (dwb_dat_o == "RTNI")) sys_int_i = 0;	 
      if (dut.cpu.regf.fRDWE && (dut.cpu.rRD == 5'h0e) && !svc && dut.cpu.gena) begin 
	 svc = 1;
	 //$display("\nLATENCY: ", ($stime - inttime)/10);	 
      end
      
      // Pass/Fail Monitors
      if (dwb_we_o & (dwb_dat_o == "FAIL")) begin
	 $display("\n\tFAIL");	 
	 $finish;
      end
      
      if (iwb_dat_i == 32'hb8000000) begin
	 theend = theend + 1;	 
      end

      if (theend == 5) begin
	 $display("\n\t*** PASSED ALL TESTS ***");
	 $finish;	 
      end
   end // always @ (posedge sys_clk_i)
   
   // INTERNAL WIRING ////////////////////////////////////////////////////
   
   aeMB_sim #(16,16)
     dut (
	  .sys_int_i(sys_int_i),
	  .dwb_ack_i(dwb_ack_i),
	  .dwb_stb_o(dwb_stb_o),
	  .dwb_adr_o(dwb_adr_o),
	  .dwb_dat_o(dwb_dat_o),
	  .dwb_dat_i(dwb_dat_i),
	  .dwb_wre_o(dwb_we_o),
	  .dwb_sel_o(dwb_sel_o),

	  .fsl_ack_i(fsl_ack_i),
	  .fsl_stb_o(fsl_stb_o),
	  .fsl_adr_o(fsl_adr_o),
	  .fsl_dat_o(fsl_dat_o),
	  .fsl_dat_i(fsl_dat_i),
	  .fsl_wre_o(fsl_we_o),

	  .iwb_adr_o(iwb_adr_o),
	  .iwb_dat_i(iwb_dat_i),
	  .iwb_stb_o(iwb_stb_o),
	  .iwb_ack_i(iwb_ack_i),
	  .sys_clk_i(sys_clk_i),
	  .sys_rst_i(sys_rst_i)
	  );

endmodule // edk32

/*
 $Log: edk32.v,v $
 Revision 1.12  2007/12/23 20:40:51  sybreon
 Abstracted simulation kernel (aeMB_sim) to split simulation models from synthesis models.

 Revision 1.11  2007/12/11 00:44:31  sybreon
 Modified for AEMB2

 Revision 1.10  2007/11/30 17:08:30  sybreon
 Moved simulation kernel into code.

 Revision 1.9  2007/11/20 18:36:00  sybreon
 Removed unnecessary byte acrobatics with VMEM data.

 Revision 1.8  2007/11/18 19:41:45  sybreon
 Minor simulation fixes.

 Revision 1.7  2007/11/14 22:11:41  sybreon
 Added posedge/negedge bus interface.
 Modified interrupt test system.

 Revision 1.6  2007/11/13 23:37:28  sybreon
 Updated simulation to also check BRI 0x00 instruction.

 Revision 1.5  2007/11/09 20:51:53  sybreon
 Added GET/PUT support through a FSL bus.

 Revision 1.4  2007/11/08 14:18:00  sybreon
 Parameterised optional components.

 Revision 1.3  2007/11/05 10:59:31  sybreon
 Added random seed for simulation.

 Revision 1.2  2007/11/02 19:16:10  sybreon
 Added interrupt simulation.
 Changed "human readable" simulation output.

 Revision 1.1  2007/11/02 03:25:45  sybreon
 New EDK 3.2 compatible design with optional barrel-shifter and multiplier.
 Fixed various minor data hazard bugs.
 Code compatible with -O0/1/2/3/s generated code.
 */