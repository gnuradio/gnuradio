

// This FIFO exists to provide an intermediate point for the data on its
// long trek from one RAM (in the buffer pool) to another (in the longfifo)
// The shortfifo is more flexible in its placement since it is based on
// distributed RAM
// This one should only be used on transmit side applications.  I.e. tx_mac, tx_dsp, etc.
//   Spartan 3's have slow routing....
// If we REALLY need to, we could also do this on the output side, 
// with for the receive side stuff

module cascadefifo
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

   wire [WIDTH-1:0] data_int;
   wire 	    empty_int, full_int, transfer;
   wire [4:0] 	    short_space, short_occupied;
   wire [15:0] 	    long_space, long_occupied;
   
   shortfifo #(.WIDTH(WIDTH)) shortfifo
     (.clk(clk),.rst(rst),.clear(clear),
      .datain(datain), .write(write), .full(full),
      .dataout(data_int), .read(transfer), .empty(empty_int),
      .space(short_space),.occupied(short_occupied) );

   longfifo #(.WIDTH(WIDTH),.SIZE(SIZE)) longfifo
     (.clk(clk),.rst(rst),.clear(clear),
      .datain(data_int), .write(transfer), .full(full_int),
      .dataout(dataout), .read(read), .empty(empty),
      .space(long_space),.occupied(long_occupied) );

   assign 	    transfer = ~empty_int & ~full_int; 	    

   assign 	    space = {11'b0,short_space} + long_space;
   assign 	    occupied = {11'b0,short_occupied} + long_occupied;
   
endmodule // cascadefifo



