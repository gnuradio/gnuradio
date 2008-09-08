/* $Id: aemb2.v,v 1.3 2007/12/28 21:44:50 sybreon Exp $
**
** AEMB2 TEST BENCH
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
** License along with AEMB. If not, see <http:**www.gnu.org/licenses/>.
*/

module aemb2 ();
   parameter IWB=16;
   parameter DWB=16;

   parameter TXE = 0; ///< thread execution enable
   
   parameter MUL = 1; ///< enable hardware multiplier
   parameter BSF = 1; ///< enable barrel shifter
   parameter FSL = 1; ///< enable FSL bus
   parameter DIV = 0; ///< enable hardware divider   
   
`include "random.v"
   
   /*AUTOWIRE*/
   // Beginning of automatic wires (for undeclared instantiated-module outputs)
   wire [6:2]		cwb_adr_o;		// From dut of aeMB2_sim.v
   wire [31:0]		cwb_dat_o;		// From dut of aeMB2_sim.v
   wire [3:0]		cwb_sel_o;		// From dut of aeMB2_sim.v
   wire			cwb_stb_o;		// From dut of aeMB2_sim.v
   wire [1:0]		cwb_tga_o;		// From dut of aeMB2_sim.v
   wire			cwb_wre_o;		// From dut of aeMB2_sim.v
   wire [DWB-1:2]	dwb_adr_o;		// From dut of aeMB2_sim.v
   wire			dwb_cyc_o;		// From dut of aeMB2_sim.v
   wire [31:0]		dwb_dat_o;		// From dut of aeMB2_sim.v
   wire [3:0]		dwb_sel_o;		// From dut of aeMB2_sim.v
   wire			dwb_stb_o;		// From dut of aeMB2_sim.v
   wire			dwb_tga_o;		// From dut of aeMB2_sim.v
   wire			dwb_wre_o;		// From dut of aeMB2_sim.v
   wire [IWB-1:2]	iwb_adr_o;		// From dut of aeMB2_sim.v
   wire			iwb_stb_o;		// From dut of aeMB2_sim.v
   wire			iwb_tga_o;		// From dut of aeMB2_sim.v
   wire			iwb_wre_o;		// From dut of aeMB2_sim.v
   // End of automatics
   /*AUTOREGINPUT*/
   // Beginning of automatic reg inputs (for undeclared instantiated-module inputs)
   reg			cwb_ack_i;		// To dut of aeMB2_sim.v
   reg			dwb_ack_i;		// To dut of aeMB2_sim.v
   reg			iwb_ack_i;		// To dut of aeMB2_sim.v
   reg			sys_clk_i;		// To dut of aeMB2_sim.v
   reg			sys_int_i;		// To dut of aeMB2_sim.v
   reg			sys_rst_i;		// To dut of aeMB2_sim.v
   // End of automatics
  
   // INITIAL SETUP //////////////////////////////////////////////////////
   
   //reg 			sys_clk_i, sys_rst_i, sys_int_i, sys_exc_i;
   reg 	     svc;
   integer   inttime;
   integer   seed;   
   integer   theend;
   
   always #5 sys_clk_i = ~sys_clk_i;   

   initial begin
      //$dumpfile("dump.vcd");
      //$dumpvars(1,dut, dut.bpcu);
   end
   
   initial begin
      seed = randseed;
      theend = 0;      
      svc = 0;      
      sys_clk_i = $random(seed);
      sys_rst_i = 1;
      sys_int_i = 0;
      #50 sys_rst_i = 0;
      #3500000 $finish;      
   end
   
   // FAKE MEMORY ////////////////////////////////////////////////////////

   reg [31:0]  rom [0:65535];
   reg [31:0]  ram[0:65535];
   reg [31:0]  dwblat;
   reg [15:2]  dadr, iadr;

   wire [31:0] dwb_dat_t = ram[dwb_adr_o];   
   wire [31:0] iwb_dat_i = rom[iadr]; 
   wire [31:0] dwb_dat_i = ram[dadr];     
   wire [31:0] cwb_dat_i = cwb_adr_o;   

`ifdef POSEDGE  
`else // !`ifdef POSEDGE
   
   always @(negedge sys_clk_i) 
     if (sys_rst_i) begin
	/*AUTORESET*/
	// Beginning of autoreset for uninitialized flops
	cwb_ack_i <= 1'h0;
	dwb_ack_i <= 1'h0;
	iwb_ack_i <= 1'h0;
	// End of automatics
     end else begin
	iwb_ack_i <= #1 iwb_stb_o;      
	dwb_ack_i <= #1 dwb_stb_o;
	cwb_ack_i <= #1 cwb_stb_o;
     end // else: !if(sys_rst_i)
   
   always @(negedge sys_clk_i) begin
      iadr <= #1 iwb_adr_o;      
      dadr <= #1 dwb_adr_o;
      
      if (dwb_wre_o & dwb_stb_o) begin
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
      #1 $readmemh("dump.vmem",rom);
      #1 $readmemh("dump.vmem",ram);
   end

   // DISPLAY OUTPUTS ///////////////////////////////////////////////////

   integer rnd;
   
   always @(posedge sys_clk_i) begin
	      
      // Interrupt Monitors
      if (!dut.sim.rMSR_IE) begin
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
      if (dwb_wre_o & (dwb_dat_o == "RTNI")) sys_int_i = 0;
      /*
      if (dut.regf.fRDWE && (dut.rRD == 5'h0e) && !svc && dut.gena) begin 
	 svc = 1;
	 //$display("\nLATENCY: ", ($stime - inttime)/10);	 
      end
       */
      
      // Pass/Fail Monitors
      if (dwb_wre_o & (dwb_dat_o == "FAIL")) begin
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
   
   aeMB2_sim
     #(/*AUTOINSTPARAM*/
       // Parameters
       .IWB				(IWB),
       .DWB				(DWB),
       .TXE				(TXE),
       .MUL				(MUL),
       .BSF				(BSF),
       .FSL				(FSL),
       .DIV				(DIV))
   dut (/*AUTOINST*/
	// Outputs
	.cwb_adr_o			(cwb_adr_o[6:2]),
	.cwb_dat_o			(cwb_dat_o[31:0]),
	.cwb_sel_o			(cwb_sel_o[3:0]),
	.cwb_stb_o			(cwb_stb_o),
	.cwb_tga_o			(cwb_tga_o[1:0]),
	.cwb_wre_o			(cwb_wre_o),
	.dwb_adr_o			(dwb_adr_o[DWB-1:2]),
	.dwb_cyc_o			(dwb_cyc_o),
	.dwb_dat_o			(dwb_dat_o[31:0]),
	.dwb_sel_o			(dwb_sel_o[3:0]),
	.dwb_stb_o			(dwb_stb_o),
	.dwb_tga_o			(dwb_tga_o),
	.dwb_wre_o			(dwb_wre_o),
	.iwb_adr_o			(iwb_adr_o[IWB-1:2]),
	.iwb_stb_o			(iwb_stb_o),
	.iwb_tga_o			(iwb_tga_o),
	.iwb_wre_o			(iwb_wre_o),
	// Inputs
	.cwb_ack_i			(cwb_ack_i),
	.cwb_dat_i			(cwb_dat_i[31:0]),
	.dwb_ack_i			(dwb_ack_i),
	.dwb_dat_i			(dwb_dat_i[31:0]),
	.iwb_ack_i			(iwb_ack_i),
	.iwb_dat_i			(iwb_dat_i[31:0]),
	.sys_clk_i			(sys_clk_i),
	.sys_int_i			(sys_int_i),
	.sys_rst_i			(sys_rst_i));

endmodule // edk32

/* $Log $ */

// Local Variables:
// verilog-library-directories:("." "../../rtl/verilog/")
// verilog-library-files:("")
// End:
