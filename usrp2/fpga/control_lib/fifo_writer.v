
module fifo_writer
  #(parameter rate=4)
    (input clk,
     output [31:0] data_out,
     output write_o,
     input ready_i,
     input done_i
     );
   
   reg [7:0] state = 0;


   // FIXME change this to write
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
   
endmodule // fifo_writer


   
