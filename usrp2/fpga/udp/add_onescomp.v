

module add_onescomp
  #(parameter WIDTH = 16)
   (input [WIDTH-1:0] A,
    input [WIDTH-1:0] B,
    output [WIDTH-1:0] SUM);

   wire [WIDTH:0] SUM_INT = {1'b0,A} + {1'b0,B};
   assign SUM  = SUM_INT[WIDTH-1:0] + {{WIDTH-1{1'b0}},SUM_INT[WIDTH]};
   
endmodule // add_onescomp
