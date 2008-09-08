

module clock_control_tb();
   
  clock_control clock_control
    (.reset(reset),
     .aux_clk(aux_clk),  
     .clk_fpga(clk_fpga),
     .clk_en(clk_en),    
     .clk_sel(clk_sel),  
     .clk_func(clk_func), 
     .clk_status(clk_status),
     
     .sen(sen),   
     .sclk(sclk), 
     .sdi(sdi),
     .sdo(sdo)
     );

   reg reset, aux_clk;
   
   wire [1:0] clk_sel, clk_en;
   
   initial reset = 1'b1;
   initial #1000 reset = 1'b0;
   
   initial aux_clk = 1'b0;
   always #10 aux_clk = ~aux_clk;
   
   initial $dumpfile("clock_control_tb.vcd");
   initial $dumpvars(0,clock_control_tb);
   
   initial #10000 $finish;
   
endmodule // clock_control_tb
