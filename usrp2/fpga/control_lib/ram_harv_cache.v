

// Dual ported, Harvard architecture, cached ram

module ram_harv_cache
  #(parameter AWIDTH=15,parameter RAM_SIZE=16384,parameter ICWIDTH=6,parameter DCWIDTH=6)
    (input wb_clk_i, input wb_rst_i,
     
     input [AWIDTH-1:0] ram_loader_adr_i,
     input [31:0] ram_loader_dat_i,
     input ram_loader_stb_i,
     input [3:0] ram_loader_sel_i,
     input ram_loader_we_i,
     output ram_loader_ack_o,
     input ram_loader_done_i,
     
     input [AWIDTH-1:0] iwb_adr_i,
     input iwb_stb_i,
     output [31:0] iwb_dat_o,
     output iwb_ack_o,
     
     input [AWIDTH-1:0] dwb_adr_i,
     input [31:0] dwb_dat_i, 
     output [31:0] dwb_dat_o,
     input dwb_we_i,
     output dwb_ack_o,
     input dwb_stb_i,
     input [3:0] dwb_sel_i );

   wire [31:0] 	 iram_dat, dram_dat_i, dram_dat_o;
   wire [AWIDTH-1:0] iram_adr, dram_adr;
   wire 	     iram_en, dram_en, dram_we;
   wire [3:0] 	     dram_sel;
   
   dpram32 #(.AWIDTH(AWIDTH),.RAM_SIZE(RAM_SIZE)) sys_ram
     (.clk(wb_clk_i),

      .adr1_i(ram_loader_done_i ? iram_adr : ram_loader_adr_i),
      .dat1_i(ram_loader_dat_i),
      .dat1_o(iram_dat),
      .we1_i(ram_loader_done_i ? 1'b0 : ram_loader_we_i),
      .en1_i(ram_loader_done_i ? iram_en : ram_loader_stb_i),
      .sel1_i(ram_loader_done_i ? 4'hF : ram_loader_sel_i),

      .adr2_i(dram_adr),.dat2_i(dram_dat_i),.dat2_o(dram_dat_o),
      .we2_i(dram_we),.en2_i(dram_en),.sel2_i(dram_sel) );

   // Data bus side
   dcache #(.AWIDTH(AWIDTH),.CWIDTH(DCWIDTH))
     dcache(.wb_clk_i(wb_clk_i),.wb_rst_i(wb_rst_i),
	    .dwb_adr_i(dwb_adr_i),.dwb_stb_i(dwb_stb_i),
	    .dwb_we_i(dwb_we_i),.dwb_sel_i(dwb_sel_i),
	    .dwb_dat_i(dwb_dat_i),.dwb_dat_o(dwb_dat_o),
	    .dwb_ack_o(dwb_ack_o),
	    .dram_dat_i(dram_dat_o),.dram_dat_o(dram_dat_i),.dram_adr_o(dram_adr),
	    .dram_we_o(dram_we),.dram_en_o(dram_en), .dram_sel_o(dram_sel) );
	    
   // Instruction bus side
   icache #(.AWIDTH(AWIDTH),.CWIDTH(ICWIDTH))
     icache(.wb_clk_i(wb_clk_i),.wb_rst_i(wb_rst_i),
	    .iwb_adr_i(iwb_adr_i),.iwb_stb_i(iwb_stb_i),
	    .iwb_dat_o(iwb_dat_o),.iwb_ack_o(iwb_ack_o),
	    .iram_dat_i(iram_dat),.iram_adr_o(iram_adr),.iram_en_o(iram_en) );

   // RAM loader
   assign 	 ram_loader_ack_o = ram_loader_stb_i;

   // Performance Monitoring
   wire 	 i_wait = iwb_stb_i & ~iwb_ack_o;
   wire 	 d_wait = dwb_stb_i & ~dwb_ack_o;
   
endmodule // ram_harv_cache
