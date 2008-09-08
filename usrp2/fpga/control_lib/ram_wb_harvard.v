
// Dual ported RAM for Harvard architecture processors
//    Does no forwarding
//    Addresses are byte-oriented, so botton 2 address bits are ignored.  FIXME
//    AWIDTH of 13 gives 8K bytes.  For Spartan 3, if the total RAM size is not a
//    multiple of 8K then BRAM space is wasted

module ram_wb_harvard #(parameter AWIDTH=13)
  (input wb_clk_i,
   input wb_rst_i,
   
   input [AWIDTH-1:0] iwb_adr_i,
   input [31:0] iwb_dat_i,
   output reg [31:0] iwb_dat_o,
   input iwb_we_i,
   output reg iwb_ack_o,
   input iwb_stb_i,
   input [3:0] iwb_sel_i,
   
   input [AWIDTH-1:0] dwb_adr_i,
   input [31:0] dwb_dat_i,
   output reg [31:0] dwb_dat_o,
   input dwb_we_i,
   output reg dwb_ack_o,
   input dwb_stb_i,
   input [3:0] dwb_sel_i);
   
   reg [7:0] 	 ram0 [0:(1<<(AWIDTH-2))-1];
   reg [7:0] 	 ram1 [0:(1<<(AWIDTH-2))-1];
   reg [7:0] 	 ram2 [0:(1<<(AWIDTH-2))-1];
   reg [7:0] 	 ram3 [0:(1<<(AWIDTH-2))-1];
   
   // Instruction Read Port
   always @(posedge wb_clk_i)
     iwb_ack_o <= iwb_stb_i & ~iwb_ack_o;
   
   always @(posedge wb_clk_i)
     iwb_dat_o[31:24] <= ram3[iwb_adr_i[AWIDTH-1:2]];
   always @(posedge wb_clk_i)
     iwb_dat_o[23:16] <= ram2[iwb_adr_i[AWIDTH-1:2]];
   always @(posedge wb_clk_i)
     iwb_dat_o[15:8] <= ram1[iwb_adr_i[AWIDTH-1:2]];
   always @(posedge wb_clk_i)
     iwb_dat_o[7:0] <= ram0[iwb_adr_i[AWIDTH-1:2]];
   
   always @(posedge wb_clk_i)
     if(iwb_we_i & iwb_stb_i & iwb_sel_i[3])
       ram3[iwb_adr_i[AWIDTH-1:2]] <= iwb_dat_i[31:24];
   always @(posedge wb_clk_i)
     if(iwb_we_i & iwb_stb_i & iwb_sel_i[2])
       ram2[iwb_adr_i[AWIDTH-1:2]] <= iwb_dat_i[23:16];
   always @(posedge wb_clk_i)
     if(iwb_we_i & iwb_stb_i & iwb_sel_i[1])
       ram1[iwb_adr_i[AWIDTH-1:2]] <= iwb_dat_i[15:8];
   always @(posedge wb_clk_i)
     if(iwb_we_i & iwb_stb_i & iwb_sel_i[0])
       ram0[iwb_adr_i[AWIDTH-1:2]] <= iwb_dat_i[7:0];
   
   // Data Port
   always @(posedge wb_clk_i)
     dwb_ack_o <= dwb_stb_i & ~dwb_ack_o;
   
   always @(posedge wb_clk_i)
     dwb_dat_o[31:24] <= ram3[dwb_adr_i[AWIDTH-1:2]];
   always @(posedge wb_clk_i)
     dwb_dat_o[23:16] <= ram2[dwb_adr_i[AWIDTH-1:2]];
   always @(posedge wb_clk_i)
     dwb_dat_o[15:8] <= ram1[dwb_adr_i[AWIDTH-1:2]];
   always @(posedge wb_clk_i)
     dwb_dat_o[7:0]  <= ram0[dwb_adr_i[AWIDTH-1:2]];
   
   always @(posedge wb_clk_i)
     if(dwb_we_i & dwb_stb_i & dwb_sel_i[3])
       ram3[dwb_adr_i[AWIDTH-1:2]] <= dwb_dat_i[31:24];
   always @(posedge wb_clk_i)
     if(dwb_we_i & dwb_stb_i & dwb_sel_i[2])
       ram2[dwb_adr_i[AWIDTH-1:2]] <= dwb_dat_i[23:16];
   always @(posedge wb_clk_i)
     if(dwb_we_i & dwb_stb_i & dwb_sel_i[1])
       ram1[dwb_adr_i[AWIDTH-1:2]] <= dwb_dat_i[15:8];
   always @(posedge wb_clk_i)
     if(dwb_we_i & dwb_stb_i & dwb_sel_i[0])
       ram0[dwb_adr_i[AWIDTH-1:2]] <= dwb_dat_i[7:0];
   
endmodule // ram_wb_harvard

