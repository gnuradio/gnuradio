
module icache
  #(parameter AWIDTH=14,
    parameter CWIDTH=6)

    (input wb_clk_i,
     input wb_rst_i,
     input [AWIDTH-1:0] iwb_adr_i,
     input iwb_stb_i,
     output [31:0] iwb_dat_o,
     output iwb_ack_o,
     input [31:0] iram_dat_i,
     output [AWIDTH-1:0] iram_adr_o,
     output iram_en_o );

   localparam TAGWIDTH = AWIDTH-CWIDTH-2;
   reg 	      stb_d1, ack_d1, miss_d1;
   reg [AWIDTH-1:0] held_addr;
   reg [31:0] 	    idata [0:(1<<CWIDTH)-1];
   reg [TAGWIDTH-1:0] itags [0:(1<<CWIDTH)-1];
   reg 		      ivalid [0:(1<<CWIDTH)-1];
   
   wire [CWIDTH-1:0]  rd_line, wr_line;
   wire [TAGWIDTH-1:0] wr_tags;
   wire 	       store_in_cache;

   // /////////////////////////////////////
   // Write into cache
   integer 	      i;
   always @(posedge wb_clk_i)
     if(wb_rst_i)
       for(i=0;i<(1<<CWIDTH);i=i+1)
	 ivalid[i] <= 0;
     else
       if(store_in_cache)
	 ivalid[wr_line] <= 1'b1;
   
   always @(posedge wb_clk_i)
     if(store_in_cache)
       begin
	  idata[wr_line] <= iram_dat_i;
	  itags[wr_line] <= wr_tags;
       end
   
   // //////////////////////////////////////
   // Read from Cache
   wire [TAGWIDTH-1:0] tag_out = itags[rd_line];
   wire 	       valid_out = ivalid[rd_line];
   wire [31:0] 	       data_out	= idata[rd_line];
   wire 	       cache_hit = valid_out & (tag_out == iwb_adr_i[AWIDTH-1:CWIDTH+2]);
   wire 	       cache_miss = ~cache_hit;

   // //////////////////////////////////////
   // Handle 1-cycle delay of Block-RAM
   always @(posedge wb_clk_i)
     if(wb_rst_i)
       stb_d1 <= 0;
     else
       stb_d1 <= iwb_stb_i;
   
   always @(posedge wb_clk_i)
     if(wb_rst_i)
       held_addr <= 0;
     else
       held_addr <= iwb_adr_i;
   
   always @(posedge wb_clk_i) 
     if(wb_rst_i)
       ack_d1 <= 1'b0;
     else 
       ack_d1 <= iwb_ack_o;

   always @(posedge wb_clk_i) 
     if(wb_rst_i)
       miss_d1 <= 0;
     else 
       miss_d1 <= cache_miss;

//`define IC_NOCACHE
//`define IC_BASIC
//`define IC_FORWARDING_DP
`define IC_FORWARDING_SP
//`define IC_PREFETCH

`ifdef IC_NOCACHE
   assign 	       iwb_dat_o = iram_dat_i;
   assign 	       iwb_ack_o = iwb_stb_i & (stb_d1 & ~ack_d1);
   assign 	       iram_adr_o = iwb_adr_i;
   assign 	       iram_en_o = 1'b1;
   assign 	       rd_line = 0;
   assign 	       wr_line = 0;
   assign 	       wr_tags = 0;
   assign 	       store_in_cache = 0;
`endif
   
`ifdef IC_BASIC    // Very basic, no forwarding, 2 wait states on miss
   assign 	       iwb_dat_o = data_out;
   assign 	       iwb_ack_o = iwb_stb_i & cache_hit;
   assign 	       iram_adr_o = iwb_adr_i;
   assign 	       iram_en_o = 1'b1;
   assign 	       rd_line = iwb_adr_i[CWIDTH+1:2];
   assign 	       wr_line = rd_line;
   assign 	       wr_tags = iwb_adr_i[AWIDTH-1:CWIDTH+2];
   assign 	       store_in_cache = stb_d1 & miss_d1;
`endif
   
`ifdef IC_FORWARDING_DP   // Simple forwarding, 1 wait state on miss, dual-port ram
   assign 	       iwb_dat_o = cache_hit ? data_out : iram_dat_i;
   assign 	       iwb_ack_o = iwb_stb_i & (cache_hit | (stb_d1 & ~ack_d1));
   assign 	       iram_adr_o = iwb_adr_i;
   assign 	       iram_en_o = 1'b1;
   assign 	       rd_line = iwb_adr_i[CWIDTH+1:2];
   assign 	       wr_line = held_addr[CWIDTH+1:2];
   assign 	       wr_tags = held_addr[AWIDTH-1:CWIDTH+2];	       
   assign 	       store_in_cache = iwb_stb_i & stb_d1 & miss_d1 & ~ack_d1;
`endif

`ifdef IC_FORWARDING_SP   // Simple forwarding, 1 wait state on miss, single-port ram
   assign 	       iwb_dat_o = cache_hit ? data_out : iram_dat_i;
   assign 	       iwb_ack_o = iwb_stb_i & (cache_hit | (stb_d1 & ~ack_d1));
   assign 	       iram_adr_o = iwb_adr_i;
   assign 	       iram_en_o = 1'b1;
   assign 	       rd_line = iwb_adr_i[CWIDTH+1:2];
   assign 	       wr_line = rd_line;
   assign 	       wr_tags = iwb_adr_i[AWIDTH-1:CWIDTH+2];
   assign 	       store_in_cache = iwb_stb_i & stb_d1 & miss_d1 & ~ack_d1;
`endif

`ifdef IC_PREFETCH   // Forwarding plus prefetch

`endif
   
   
endmodule // icache
