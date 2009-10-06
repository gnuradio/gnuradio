
module fifo_2clock_cascade
  #(parameter WIDTH=32, SIZE=9)
   (input wclk, input [WIDTH-1:0] datain, input src_rdy_i, output dst_rdy_o, output [15:0] space,
    input rclk, output [WIDTH-1:0] dataout, output src_rdy_o, input dst_rdy_i, output [15:0] occupied,
    input arst);
   
   wire [WIDTH-1:0] data_int1, data_int2;
   wire 	    src_rdy_int1, src_rdy_int2, dst_rdy_int1, dst_rdy_int2;
   wire [SIZE-1:0]  level_wclk, level_rclk;
   wire [4:0] 	    s1_space, s1_occupied, s2_space, s2_occupied;
   wire [15:0] 	    l_space, l_occupied;
   
   fifo_short #(.WIDTH(WIDTH)) shortfifo
     (.clk(wclk), .reset(arst), .clear(0),
      .datain(datain), .src_rdy_i(src_rdy_i), .dst_rdy_o(dst_rdy_o),
      .dataout(data_int1), .src_rdy_o(src_rdy_int1), .dst_rdy_i(dst_rdy_int1),
      .space(s1_space), .occupied(s1_occupied) );
   
   fifo_2clock #(.WIDTH(WIDTH),.SIZE(SIZE)) fifo_2clock
     (.wclk(wclk), .datain(data_int1), .src_rdy_i(src_rdy_int1), .dst_rdy_o(dst_rdy_int1), .space(l_space),
      .rclk(rclk), .dataout(data_int2), .src_rdy_o(src_rdy_int2), .dst_rdy_i(dst_rdy_int2), .occupied(l_occupied),
      .arst(arst) );
   
   fifo_short #(.WIDTH(WIDTH)) shortfifo2
     (.clk(rclk), .reset(arst), .clear(0),
      .datain(data_int2), .src_rdy_i(src_rdy_int2), .dst_rdy_o(dst_rdy_int2),
      .dataout(dataout), .src_rdy_o(src_rdy_o), .dst_rdy_i(dst_rdy_i),
      .space(s2_space), .occupied(s2_occupied));

   // Be conservative -- Only advertise space from input side of fifo, occupied from output side
   assign 	    space = {11'b0,s1_space} + l_space;
   assign 	    occupied = {11'b0,s2_occupied} + l_occupied;
   
endmodule // fifo_2clock_cascade
