

module timer
  (input wb_clk_i, input rst_i,
   input cyc_i, input stb_i, input [2:0] adr_i,
   input we_i, input [31:0] dat_i, output [31:0] dat_o, output ack_o,
   input sys_clk_i, input [31:0] master_time_i,
   output int_o );

   reg [31:0] time_wb;
   always @(posedge wb_clk_i)
     time_wb <= master_time_i;

   assign     ack_o = stb_i;

   reg [31:0] int_time;
   reg 	      int_reg;
   
   always @(posedge sys_clk_i)
     if(rst_i)
       begin
	  int_time <= 0;
	  int_reg <= 0;
       end
     else if(|int_time && (master_time_i == int_time))
       begin
	  int_time <= 0;
	  int_reg <= 1;
       end
     else if(stb_i & we_i)
       begin
	  int_time <= dat_i;
	  int_reg <= 0;
       end

   assign dat_o = time_wb;
   assign int_o = int_reg;
   
endmodule // timer

