

module delay_line
  #(parameter WIDTH=32)
   (input clk,
    input [3:0] delay,
    input [WIDTH-1:0] din,
    output [WIDTH-1:0] dout);
    
   genvar 	       i;
   generate
      for (i=0;i<WIDTH;i=i+1)
	begin : gen_delay
	   SRL16E
	     srl16e(.Q(dout[i]),
		    .A0(delay[0]),.A1(delay[1]),.A2(delay[2]),.A3(delay[3]),
		    .CE(1),.CLK(clk),.D(din[i]));
	end
   endgenerate

endmodule // delay_line
