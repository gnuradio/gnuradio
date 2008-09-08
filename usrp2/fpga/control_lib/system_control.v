// System bootup order:
//    0 - Internal POR to reset this block.  Maybe control it from CPLD in the future?
//    1 - Everything in reset
//    2 - Take RAM Loader out of reset
//    3 - When RAM Loader done, take processor and wishbone out of reset

module system_control 
  (input wb_clk_i,
   output reg ram_loader_rst_o,
   output reg wb_rst_o,
   input ram_loader_done_i
   );

   reg 		POR = 1'b1;
   reg [3:0] 	POR_ctr;

   initial POR_ctr = 4'd0;
   always @(posedge wb_clk_i)
     if(POR_ctr == 4'd15)
       POR <= 1'b0;
     else
       POR_ctr <= POR_ctr + 4'd1;
   
   always @(posedge POR or posedge wb_clk_i)
     if(POR)
       ram_loader_rst_o <= 1'b1;
     else
       ram_loader_rst_o <= #1 1'b0;

   // Main system reset
   reg 		delayed_rst;
   
   always @(posedge POR or posedge wb_clk_i)
     if(POR)
       begin
	  wb_rst_o <= 1'b1;
	  delayed_rst <= 1'b1;
       end
     else if(ram_loader_done_i)
       begin
	  delayed_rst <= 1'b0;
	  wb_rst_o <= delayed_rst;
       end

endmodule // system_control


