
`timescale 1ns/1ns
module rx_dcoffset_tb();
   
   reg clk, rst;

   initial rst = 1;
   initial #1000 rst = 0;
   initial clk = 0;
   always #5 clk = ~clk;
   
   initial $dumpfile("rx_dcoffset_tb.vcd");
   initial $dumpvars(0,rx_dcoffset_tb);

   reg [13:0] adc_in = 7;
   wire [13:0] adc_out;

   always @(posedge clk)
     $display("%d\t%d",adc_in,adc_out);
   
   rx_dcoffset #(.WIDTH(14),.ADDR(0))
     rx_dcoffset(.clk(clk),.rst(rst),.set_stb(0),.set_addr(0),.set_data(0),
		 .adc_in(adc_in),.adc_out(adc_out));
   
endmodule // longfifo_tb
