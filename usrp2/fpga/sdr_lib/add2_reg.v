
module add2_reg
  #(parameter WIDTH=16)
    (input clk,
     input [WIDTH-1:0] in1,
     input [WIDTH-1:0] in2,
     output reg [WIDTH-1:0] sum);

   wire [WIDTH-1:0] sum_int;
   
   add2 #(.WIDTH(WIDTH)) add2 (.in1(in1),.in2(in2),.sum(sum_int));

   always @(posedge clk)
     sum <= sum_int;
   
endmodule // add2_reg

