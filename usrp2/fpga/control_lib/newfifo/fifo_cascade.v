

// This FIFO exists to provide an intermediate point for the data on its
// long trek from one RAM (in the buffer pool) to another (in the longfifo)
// The shortfifo is more flexible in its placement since it is based on
// distributed RAM

// This one has the shortfifo on both the in and out sides.
module fifo_cascade
  #(parameter WIDTH=32, SIZE=9)
    (input clk, input reset, input clear,
     input [WIDTH-1:0] datain,
     input src_rdy_i,
     output dst_rdy_o,
     output [WIDTH-1:0] dataout,
     output src_rdy_o,
     input dst_rdy_i,
     output [15:0] space,
     output [15:0] occupied);

   wire [WIDTH-1:0] data_int, data_int2;
   wire src_rdy_1, dst_rdy_1, src_rdy_2, dst_rdy_2;
   
   wire [4:0] 	    s1_space, s1_occupied, s2_space, s2_occupied;
   wire [15:0] 	    l_space, l_occupied;
   
   fifo_short #(.WIDTH(WIDTH)) head_fifo
     (.clk(clk),.reset(reset),.clear(clear),
      .datain(datain), .src_rdy_i(src_rdy_i), .dst_rdy_o(dst_rdy_o),
      .dataout(data_int), .src_rdy_o(src_rdy_1), .dst_rdy_i(dst_rdy_1),
      .space(s1_space),.occupied(s1_occupied) );
      
   fifo_long #(.WIDTH(WIDTH),.SIZE(SIZE)) middle_fifo
     (.clk(clk),.reset(reset),.clear(clear),
      .datain(data_int), .src_rdy_i(src_rdy_1), .dst_rdy_o(dst_rdy_1),
      .dataout(data_int2), .src_rdy_o(src_rdy_2), .dst_rdy_i(dst_rdy_2),
      .space(l_space),.occupied(l_occupied) );
   
   fifo_short #(.WIDTH(WIDTH)) tail_fifo
     (.clk(clk),.reset(reset),.clear(clear),
      .datain(data_int2), .src_rdy_i(src_rdy_2), .dst_rdy_o(dst_rdy_2),
      .dataout(dataout), .src_rdy_o(src_rdy_o), .dst_rdy_i(dst_rdy_i),
      .space(s2_space),.occupied(s2_occupied) );
   
   assign 	    space = {11'b0,s1_space} + {11'b0,s2_space} + l_space;
   assign 	    occupied = {11'b0,s1_occupied} + {11'b0,s2_occupied} + l_occupied;
      
endmodule // cascadefifo2




