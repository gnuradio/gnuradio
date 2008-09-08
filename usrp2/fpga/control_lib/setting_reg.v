

module setting_reg
  #(parameter my_addr = 0)
    (input clk, input rst, input strobe, input wire [7:0] addr,
     input wire [31:0] in, output reg [31:0] out, output reg changed);
   
   always @(posedge clk)
     if(rst)
       begin
	  out <= 32'd0;
	  changed <= 1'b0;
       end
     else
       if(strobe & (my_addr==addr))
	 begin
	    out <= in;
	    changed <= 1'b1;
	 end
       else
	 changed <= 1'b0;
   
endmodule // setting_reg
