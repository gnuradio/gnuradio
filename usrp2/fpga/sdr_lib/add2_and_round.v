
module add2_and_round
  #(parameter WIDTH=16)
    (input [WIDTH-1:0] in1,
     input [WIDTH-1:0] in2,
     output [WIDTH-1:0] sum);

   wire [WIDTH:0] 	sum_int = {in1[WIDTH-1],in1} + {in2[WIDTH-1],in2};
   assign 		sum = sum_int[WIDTH:1] + (sum_int[WIDTH] & sum_int[0]);
   
endmodule // add2_and_round
