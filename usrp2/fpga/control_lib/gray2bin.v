

module gray2bin
  #(parameter WIDTH=8)
    (input [WIDTH-1:0] gray,
     output reg [WIDTH-1:0] bin);

   integer 		i;
   always @(gray)
     for(i = 0;i<WIDTH;i=i+1)
	  bin[i] = ^(gray>>i);
   
endmodule // gray2bin
