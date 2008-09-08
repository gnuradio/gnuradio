

// This FIFO exists to provide an intermediate point for the data on its
// long trek from one RAM (in the buffer pool) to another (in the longfifo)
// The shortfifo is more flexible in its placement since it is based on
// distributed RAM

// This one has the shortfifo on both the in and out sides.
module cascadefifo2
  #(parameter WIDTH=32, SIZE=9)
    (input clk, input rst,
     input [WIDTH-1:0] datain,
     output [WIDTH-1:0] dataout,
     input read,
     input write,
     input clear,
     output full,
     output empty,
     output [15:0] space,
     output [15:0] occupied);

   wire [WIDTH-1:0] data_int, data_int2;
   wire 	    empty_int, full_int, transfer;
   wire 	    empty_int2, full_int2, transfer2;
   wire [4:0] 	    s1_space, s1_occupied, s2_space, s2_occupied;
   wire [15:0] 	    l_space, l_occupied;
   
   shortfifo #(.WIDTH(WIDTH)) shortfifo
     (.clk(clk),.rst(rst),.clear(clear),
      .datain(datain), .write(write), .full(full),
      .dataout(data_int), .read(transfer), .empty(empty_int),
      .space(s1_space),.occupied(s1_occupied) );
      
   longfifo #(.WIDTH(WIDTH),.SIZE(SIZE)) longfifo
     (.clk(clk),.rst(rst),.clear(clear),
      .datain(data_int), .write(transfer), .full(full_int),
      .dataout(data_int2), .read(transfer2), .empty(empty_int2),
      .space(l_space),.occupied(l_occupied) );
   
   shortfifo #(.WIDTH(WIDTH)) shortfifo2
     (.clk(clk),.rst(rst),.clear(clear),
      .datain(data_int2), .write(transfer2), .full(full_int2),
      .dataout(dataout), .read(read), .empty(empty),
      .space(s2_space),.occupied(s2_occupied) );
   
   assign 	    transfer = ~empty_int & ~full_int; 	    
   assign 	    transfer2 = ~empty_int2 & ~full_int2; 	    
   
   assign 	    space = {11'b0,s1_space} + {11'b0,s2_space} + l_space;
   assign 	    occupied = {11'b0,s1_occupied} + {11'b0,s2_occupied} + l_occupied;
      
endmodule // cascadefifo2




