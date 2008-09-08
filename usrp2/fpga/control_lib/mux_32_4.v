

module mux_32_4
  (input [1:0] sel,
   input [31:0] in0,
   input [31:0] in1,
   input [31:0] in2,
   input [31:0] in3,
   output [31:0] out);

   assign 	 out = sel[1] ? (sel[0] ? in3 : in2) : (sel[0] ? in1 : in0);

endmodule // mux_32_4
