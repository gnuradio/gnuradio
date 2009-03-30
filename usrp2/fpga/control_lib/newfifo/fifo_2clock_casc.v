
module fifo_2clock_casc
  #(parameter DWIDTH=32, AWIDTH=9)
    (input wclk, input [DWIDTH-1:0] datain, input write, output full, output [AWIDTH-1:0] level_wclk,
     input rclk, output [DWIDTH-1:0] dataout, input read, output empty, output [AWIDTH-1:0] level_rclk,
     input arst);

   wire    full_int, empty_int, full_int2, empty_int2, transfer, transfer2;
   wire [DWIDTH-1:0] data_int, data_int2;
   
   shortfifo #(.WIDTH(DWIDTH)) shortfifo
     (.clk(wclk), .rst(arst), .clear(0),
      .datain(datain), .write(write), .full(full),
      .dataout(data_int), .read(transfer), .empty(empty_int) );

   assign  transfer = ~full_int & ~empty_int;
   
   fifo_2clock #(.DWIDTH(DWIDTH),.AWIDTH(AWIDTH)) fifo_2clock
     (.wclk(wclk), .datain(data_int), .write(transfer), .full(full_int), .level_wclk(level_wclk),
      .rclk(rclk), .dataout(data_int2), .read(transfer2), .empty(empty_int2), .level_rclk(level_rclk),
      .arst(arst) );

   assign  transfer2 = ~full_int2 & ~empty_int2;

   shortfifo #(.WIDTH(DWIDTH)) shortfifo2
     (.clk(rclk), .rst(arst), .clear(0),
      .datain(data_int2), .write(transfer2), .full(full_int2),
      .dataout(dataout), .read(read), .empty(empty) );
   
endmodule // fifo_2clock_casc

