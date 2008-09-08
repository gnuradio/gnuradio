
module fifo_reader
  #(parameter rate=4)
    (input clk,
     input [31:0] data_in,
     output read_o
     input ready_i,
     input done_i
     );

   reg [7:0] state = 0;
   
   always @(posedge clk)
     if(ready)
       if(state == rate)
	 state <= 0;
       else
	 state <= state + 1;
     else
       state <= 0;

   assign    read = (state == rate);

   initial $monitor(data_in);
   
endmodule // fifo_reader

   
