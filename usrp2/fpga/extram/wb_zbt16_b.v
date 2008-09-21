
module wb_zbt16_b
  (input               clk,
   input               rst,
   // Wishbone bus A, highest priority, with prefetch
   input      [19:0]   wb_adr_i,
   input      [15:0]   wb_dat_i,
   output reg [15:0]   wb_dat_o,
   input      [ 1:0]   wb_sel_i,
   input               wb_cyc_i,
   input               wb_stb_i,
   output reg          wb_ack_o,
   input               wb_we_i,
   // Memory connection
   output              sram_clk,
   output     [18:0]   sram_a,
   inout      [15:0]   sram_d,
   output              sram_we,
   output     [ 1:0]   sram_bw,
   output              sram_adv,
   output              sram_ce,
   output              sram_oe,
   output              sram_mode,
   output              sram_zz
   );

   assign 	       sram_clk = ~clk;
   //assign 	       sram_oe = 1'b0;
   assign 	       sram_ce = 1'b0;
   assign 	       sram_adv = 1'b0;
   assign 	       sram_mode = 1'b0;
   assign 	       sram_zz = 1'b0;
   assign 	       sram_bw = 2'b0;
   
   // need to drive wb_dat_o, wb_ack_o,
   // sram_a, sram_d, sram_we
   wire 	       myOE;
   assign 	       sram_d = myOE ? wb_dat_i : 16'bzzzz;
   assign 	       sram_a = wb_adr_i[19:1];

   reg 		       read_d1, read_d2, read_d3, write_d1, write_d2, write_d3;
   wire 	       acc = wb_cyc_i & wb_stb_i;
   wire 	       read_acc = wb_cyc_i & wb_stb_i & ~wb_we_i & ~read_d1 & ~read_d2 & ~read_d3;
   wire 	       write_acc = wb_cyc_i & wb_stb_i & wb_we_i & ~write_d1 & ~write_d2 & ~write_d3;

   assign 	       sram_we = ~write_acc;
   assign 	       sram_oe = ~(read_d2 | read_d3);
   assign 	       myOE = write_d1 | write_d2;
   wire 	       latch_now = read_d2;

   always @(posedge clk)
     if(latch_now)
       wb_dat_o <= sram_d;

   always @(posedge clk) wb_ack_o <= read_d2 | write_d2;
   always @(posedge clk) read_d1 <= read_acc;
   always @(posedge clk) read_d2 <= read_d1;
   always @(posedge clk) read_d3 <= read_d2;
   always @(posedge clk) write_d1 <= write_acc;
   always @(posedge clk) write_d2 <= write_d1;
   always @(posedge clk) write_d3 <= write_d2;
endmodule // wb_zbt16_b

