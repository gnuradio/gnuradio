

module setting_reg
  ( input clock, input reset, input strobe, input wire [6:0] addr,
    input wire [31:0] in, output reg [31:0] out, output reg changed);
   parameter my_addr = 0;
   
   always @(posedge clock)
     if(reset)
       begin
	  out <= #1 32'd0;
	  changed <= #1 1'b0;
       end
     else
       if(strobe & (my_addr==addr))
	 begin
	    out <= #1 in;
	    changed <= #1 1'b1;
	 end
       else
	 changed <= #1 1'b0;
   
endmodule // setting_reg
