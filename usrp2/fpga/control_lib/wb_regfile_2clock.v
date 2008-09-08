
module wb_regfile_2clock
  (input wb_clk_i,
   input wb_rst_i,
   input wb_stb_i,
   input wb_we_i,
   input [15:0] wb_adr_i,
   input [3:0] wb_sel_i,
   input [31:0] wb_dat_i,
   output [31:0] wb_dat_o,
   output wb_ack_o,
   input alt_clk,
   input alt_rst,
   
   output reg [31:0] reg00,
   output reg [31:0] reg01,
   output reg [31:0] reg02,
   output reg [31:0] reg03,
   output reg [31:0] reg04,
   output reg [31:0] reg05,
   output reg [31:0] reg06,
   output reg [31:0] reg07
   );

   reg [15:0] 	 addr_reg;
   reg [3:0] 	 sel_reg;
   reg [31:0] 	 dat_reg;
   reg 		 wr_ret1, wr_ret2, we_reg, stb_reg;
   
   always @(posedge wb_clk_i)
     if(wb_rst_i)
       begin
	  addr_reg <= 0;
	  sel_reg <= 0;
	  dat_reg <= 0;
       end
     else if(wb_stb_i & wb_we_i)
       begin
	  addr_reg <= wb_adr_i;
	  sel_reg <= wb_sel_i;
	  dat_reg <= wb_dat_i;
       end
   
   always @(posedge wb_clk_i)
     if(wb_rst_i)
       {we_reg,stb_reg} <= 2'b0;
     else
       {we_reg,stb_reg} <= {wb_we_i,wb_stb_i};

   assign wb_ack_o = stb_reg;
   
   always @(posedge alt_clk)
     if(alt_rst)
       {wr_ret2, wr_ret1} <= 2'b0;
     else
       {wr_ret2, wr_ret1} <= {wr_ret1, we_reg & stb_reg};
   
   always @(posedge alt_clk)
     if(alt_rst)
       begin
	  reg00 <= 0;
	  reg01 <= 0;
	  reg02 <= 0;
	  reg03 <= 0;
	  reg04 <= 0;
	  reg05 <= 0;
	  reg06 <= 0;
	  reg07 <= 0;
       end // if (alt_rst)
     else if(wr_ret2)
       case(addr_reg[4:2])
	 3'd0: reg00 <= { {sel_reg[3] ? dat_reg[31:24] : reg00[31:24]},
			  {sel_reg[2] ? dat_reg[23:16] : reg00[23:16]},
			  {sel_reg[1] ? dat_reg[15:8] : reg00[15:8]},
			  {sel_reg[0] ? dat_reg[7:0] : reg00[7:0]}};
	 3'd1: reg01 <= { {sel_reg[3] ? dat_reg[31:24] : reg01[31:24]},
			  {sel_reg[2] ? dat_reg[23:16] : reg01[23:16]},
			  {sel_reg[1] ? dat_reg[15:8] : reg01[15:8]},
			  {sel_reg[0] ? dat_reg[7:0] : reg01[7:0]}};
	 3'd2: reg02 <= { {sel_reg[3] ? dat_reg[31:24] : reg02[31:24]},
			  {sel_reg[2] ? dat_reg[23:16] : reg02[23:16]},
			  {sel_reg[1] ? dat_reg[15:8] : reg02[15:8]},
			  {sel_reg[0] ? dat_reg[7:0] : reg02[7:0]}};
	 3'd3: reg03 <= { {sel_reg[3] ? dat_reg[31:24] : reg03[31:24]},
			  {sel_reg[2] ? dat_reg[23:16] : reg03[23:16]},
			  {sel_reg[1] ? dat_reg[15:8] : reg03[15:8]},
			  {sel_reg[0] ? dat_reg[7:0] : reg03[7:0]}};
	 3'd4: reg04 <= { {sel_reg[3] ? dat_reg[31:24] : reg04[31:24]},
			  {sel_reg[2] ? dat_reg[23:16] : reg04[23:16]},
			  {sel_reg[1] ? dat_reg[15:8] : reg04[15:8]},
			  {sel_reg[0] ? dat_reg[7:0] : reg04[7:0]}};
	 3'd5: reg05 <= { {sel_reg[3] ? dat_reg[31:24] : reg05[31:24]},
			  {sel_reg[2] ? dat_reg[23:16] : reg05[23:16]},
			  {sel_reg[1] ? dat_reg[15:8] : reg05[15:8]},
			  {sel_reg[0] ? dat_reg[7:0] : reg05[7:0]}};
	 3'd6: reg06 <= { {sel_reg[3] ? dat_reg[31:24] : reg06[31:24]},
			  {sel_reg[2] ? dat_reg[23:16] : reg06[23:16]},
			  {sel_reg[1] ? dat_reg[15:8] : reg06[15:8]},
			  {sel_reg[0] ? dat_reg[7:0] : reg06[7:0]}};
	 3'd7: reg07 <= { {sel_reg[3] ? dat_reg[31:24] : reg07[31:24]},
			  {sel_reg[2] ? dat_reg[23:16] : reg07[23:16]},
			  {sel_reg[1] ? dat_reg[15:8] : reg07[15:8]},
			  {sel_reg[0] ? dat_reg[7:0] : reg07[7:0]}};
       endcase // case(addr_reg[2:0])
   
endmodule // wb_regfile_2clock

