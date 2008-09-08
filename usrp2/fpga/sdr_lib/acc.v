
module acc
  #(parameter IWIDTH=16, OWIDTH=30)
    (input clk,
     input clear,
     input acc,
     input [IWIDTH-1:0] in,
     output reg [OWIDTH-1:0] out);

   wire [OWIDTH-1:0] in_signext;
   sign_extend #(.bits_in(IWIDTH),.bits_out(OWIDTH)) 
     acc_signext (.in(in),.out(in_signext));
   
   //  CLEAR & ~ACC  -->  clears the accumulator
   //  CLEAR & ACC -->    loads the accumulator
   //  ~CLEAR & ACC -->   accumulates
   //  ~CLEAR & ~ACC -->  hold
   
   wire [OWIDTH-1:0] addend1 = clear ? 0 : out;
   wire [OWIDTH-1:0] addend2 = ~acc ? 0 : in_signext;
   wire [OWIDTH-1:0] sum_int = addend1 + addend2;

   always @(posedge clk)
     out <= sum_int;
   
endmodule // acc


