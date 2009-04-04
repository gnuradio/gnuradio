

module reset_sync
  (input clk,
   input reset_in,
   output reg reset_out);

   reg 	      reset_int;

   always @(posedge clk or posedge reset_in)
     if(reset_in)
       {reset_out,reset_int} <= 2'b11;
     else
       {reset_out,reset_int} <= {reset_int,1'b0};
   
endmodule // reset_sync
