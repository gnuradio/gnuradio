
module cascadefifo_2clock
  #(parameter DWIDTH=32, AWIDTH=9)
    (input wclk, input [DWIDTH-1:0] datain, input src_rdy_i, output dst_rdy_o, output [AWIDTH-1:0] level_wclk,
     input rclk, output [DWIDTH-1:0] dataout, output src_rdy_o, input dst_rdy_i, output [AWIDTH-1:0] level_rclk,
     input arst);

   wire [DWIDTH-1:0] data_int1, data_int2;
   wire src_rdy_int1, src_rdy_int2, dst_rdy_int1, dst_rdy_int2;
   
   fifo_short #(.WIDTH(DWIDTH)) shortfifo
     (.clk(wclk), .reset(arst), .clear(0),
      .datain(datain), .src_rdy_i(src_rdy_i), .dst_rdy_o(dst_rdy_o),
      .dataout(data_int1), .src_rdy_o(src_rdy_int1), .dst_rdy_i(dst_rdy_int1) );
   
   newfifo_2clock #(.DWIDTH(DWIDTH),.AWIDTH(AWIDTH)) fifo_2clock
     (.wclk(wclk), .datain(data_int1), .src_rdy_i(src_rdy_int1), .dst_rdy_o(dst_rdy_int1), .level_wclk(level_wclk),
      .rclk(rclk), .dataout(data_int2), .src_rdy_o(src_rdy_int2), .dst_rdy_i(dst_rdy_int2), .level_rclk(level_rclk),
      .arst(arst) );

   fifo_short #(.WIDTH(DWIDTH)) shortfifo2
     (.clk(rclk), .reset(arst), .clear(0),
      .datain(data_int2), .src_rdy_i(src_rdy_int2), .dst_rdy_o(dst_rdy_int2),
      .dataout(dataout), .src_rdy_o(src_rdy_o), .dst_rdy_i(dst_rdy_i) );
   
endmodule // fifo_2clock_casc

