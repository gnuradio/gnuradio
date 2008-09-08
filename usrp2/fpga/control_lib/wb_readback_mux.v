

// Note -- clocks must be synchronous (derived from the same source)
// Assumes alt_clk is running at a multiple of wb_clk

module wb_readback_mux
  (input wb_clk_i,
   input wb_rst_i,
   input wb_stb_i,
   input [15:0] wb_adr_i,
   output reg [31:0] wb_dat_o,
   output reg wb_ack_o,
   
   input [31:0] word00,
   input [31:0] word01,
   input [31:0] word02,
   input [31:0] word03,
   input [31:0] word04,
   input [31:0] word05,
   input [31:0] word06,
   input [31:0] word07,
   input [31:0] word08,
   input [31:0] word09,
   input [31:0] word10,
   input [31:0] word11,
   input [31:0] word12,
   input [31:0] word13,
   input [31:0] word14,
   input [31:0] word15
   );

   always @(posedge wb_clk_i)
     if(wb_rst_i)
       wb_ack_o <= 0;
     else
       wb_ack_o <= wb_stb_i & ~wb_ack_o;
   
   always @(posedge wb_clk_i)
     case(wb_adr_i[5:2])
       0 : wb_dat_o <= word00;
       1 : wb_dat_o <= word01;
       2 : wb_dat_o <= word02;
       3 : wb_dat_o <= word03;
       4 : wb_dat_o <= word04;
       5 : wb_dat_o <= word05;
       6 : wb_dat_o <= word06;
       7 : wb_dat_o <= word07;
       8 : wb_dat_o <= word08;
       9 : wb_dat_o <= word09;
       10: wb_dat_o <= word10;
       11: wb_dat_o <= word11;
       12: wb_dat_o <= word12;
       13: wb_dat_o <= word13;
       14: wb_dat_o <= word14;
       15: wb_dat_o <= word15;
     endcase // case(addr_reg[3:0])
      
endmodule // wb_readback_mux


