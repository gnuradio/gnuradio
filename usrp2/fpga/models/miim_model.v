
// Skeleton PHY interface simulator

module miim_model(input mdc_i, 
		  inout mdio, 
		  input phy_resetn_i, 
		  input phy_clk_i, 
		  output phy_intn_o,
		  output [2:0] speed_o);

   assign 		       phy_intn_o = 1;    // No interrupts
   assign 		       speed_o = 3'b100;  // 1G mode
   
endmodule // miim_model
