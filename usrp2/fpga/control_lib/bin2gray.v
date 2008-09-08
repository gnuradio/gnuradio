

module bin2gray
  #(parameter WIDTH=8)
    (input [WIDTH-1:0] bin,
     output [WIDTH-1:0] gray);

   assign 		gray = (bin >> 1) ^ bin;
   
endmodule // bin2gray
