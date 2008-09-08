
// Wrapper for aeMB core:
//    Drive wb_cyc_o  (just tied to wb_stb_o for now)
//    Make input reset active high (like the signal name makes it sound....)
// No longer needed
//    Make it big-endian like the standard MicroBlaze

module aeMB_core_BE
  #(parameter ISIZ=32, parameter DSIZ=32, 
    parameter MUL=0, parameter BSF=0)
    (input sys_clk_i,
     input sys_rst_i,

     output iwb_stb_o,
     output [ISIZ-1:0] iwb_adr_o,
     input [31:0] iwb_dat_i,
     input iwb_ack_i,

     output dwb_we_o,
     output dwb_stb_o,
     output [DSIZ-1:0] dwb_adr_o,
     output [31:0] dwb_dat_o,
     input [31:0] dwb_dat_i,
     input dwb_ack_i,
     output [3:0] dwb_sel_o,
     output dwb_cyc_o,
     
     input sys_int_i, 
     input sys_exc_i);

   assign  dwb_cyc_o = dwb_stb_o;

   aeMB_edk32 #(.IW(ISIZ),.DW(DSIZ),.MUL(MUL),.BSF(BSF))
     aeMB_edk32 (.sys_clk_i(sys_clk_i), 
		 .sys_rst_i(sys_rst_i),
		 
		 .iwb_stb_o(iwb_stb_o),
		 .iwb_adr_o(iwb_adr_o[ISIZ-1:2]),
		 .iwb_ack_i(iwb_ack_i),
		 .iwb_dat_i(iwb_dat_i),
		 
		 .dwb_wre_o(dwb_we_o),
		 .dwb_stb_o(dwb_stb_o),
		 .dwb_adr_o(dwb_adr_o[DSIZ-1:2]),
		 .dwb_ack_i(dwb_ack_i),
		 .dwb_sel_o(dwb_sel_o),
		 .dwb_dat_i(dwb_dat_i),
		 .dwb_dat_o(dwb_dat_o),
	
		 .fsl_wre_o(),
		 .fsl_tag_o(),
		 .fsl_stb_o(),
		 .fsl_dat_o(),
		 .fsl_adr_o(),
		 .fsl_dat_i(32'b0),
		 .fsl_ack_i(1'b0),
		 .sys_int_i(sys_int_i) );
   
   assign  iwb_adr_o[1:0] = 2'b0;
   assign  dwb_adr_o[1:0] = 2'b0;

endmodule // aeMB_core_BE
